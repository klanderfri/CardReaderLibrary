#include "stdafx.h"
#include "CardReader.h"
#include "CardExtractor.h"
#include "FileHandling.h"
#include "LoadOcvImage.h"
#include "MtgCardInfoHelper.h"
#include "SaveOcvImage.h"
#include "TitleExtractor.h"
#include "ImageOcrHelper.h"
#include "StoreCardProcessingData.h"
#include "TitleLetterFixer.h"

using namespace cv;
using namespace std;

CardReader::CardReader(wstring imageFileName, SystemMethods* systemMethods, bool runDebugging)
	: BasicReaderData(imageFileName, Mat(), systemMethods, runDebugging),
	NORMAL_OCR_CONFIDENCE_THRESH(75),
	HIGH_OCR_CONFIDENCE_THRESH(80)
{
}

CardReader::~CardReader()
{
}

wstring CardReader::GetImageFileName() {

	return imageFileName;
}

CardNameInfo CardReader::GetResult() {

	return finalResult;
}

void CardReader::ReadCardName() {

	//Reset variables keeping track of the number of X tries.
	numberOfOcrTitlesStoredForDebug = 0;
	numberOfTitleImagesStoredForDebug = 0;
	numberOfCardReadTries = 0;

	//Load the image.
	Mat originalImage = LoadOcvImage::LoadImageData(systemMethods, imageFileName);

	//Extract the card part.
	Mat cardImage = extractCardImage(originalImage);

	vector<ReadingConfiguration> configs {
		ReadingConfiguration(120, false),
		ReadingConfiguration(120, true),
		ReadingConfiguration(80, false),
		ReadingConfiguration(140, false),
		ReadingConfiguration(80, true),
		ReadingConfiguration(140, true)
	};

	//Extract the title text.
	finalResult = readTitle1(cardImage, true, configs, NormalTitle);
	finalResult.FileName = imageFileName;

	//Oops! Seems like we couldn't get any title text.
	if (!finalResult.IsSuccessful()) {
		finalResult.CardName = L"ERROR: Could not OCR-read the title!";
	}
}

Mat CardReader::extractCardImage(Mat originalImage) {

	Mat cardImage;
	CardExtractor cardExtractor(imageFileName, originalImage, systemMethods, runDebugging);
	bool success = cardExtractor.ExtractCard(cardImage);

	//See if we need to stop.
	if (!success) {
		throw OperationException("ERROR: Could not extract the card section!");
	}

	return cardImage;
}

CardNameInfo CardReader::readTitle1(Mat cardImage, bool searchForAkhSplitHalf, vector<ReadingConfiguration> configs, CardTitleType cardType) {

	CardNameInfo bestResult;
	Mat cardImageForBestResult;

	bool hasFoundGoodResult = false;
	size_t numberOfConfigs = configs.size();

	for (size_t i = 0; i < numberOfConfigs; i++) {

		ReadingConfiguration currentReadingConfig = configs[i];

		Mat cardIterationCopy;
		cardImage.copyTo(cardIterationCopy);

		if (currentReadingConfig.Rotate180Degrees) {
			rotate(cardIterationCopy, cardIterationCopy, ROTATE_180);
		}

		CardNameInfo tempResult = readTitle2(cardIterationCopy, currentReadingConfig, cardType);

		if (bestResult.Confidence < tempResult.Confidence) {
			bestResult = tempResult;
			cardIterationCopy.copyTo(cardImageForBestResult);
		}

		//Sometimes we can be pretty sure we got the title.
		hasFoundGoodResult = (bestResult.Confidence > HIGH_OCR_CONFIDENCE_THRESH);

		//It's only useful to search for Amonkhet split card in certain circumstances.
		bool acceptableResult = (bestResult.Confidence > NORMAL_OCR_CONFIDENCE_THRESH);
		bool isLastIteration = (i == numberOfConfigs - 1);
		bool executeSearchForAkhSplitHalf = searchForAkhSplitHalf && (hasFoundGoodResult || acceptableResult && isLastIteration) && bestResult.CardType == NormalTitle;

		if (executeSearchForAkhSplitHalf) {

			Mat akhSplitHalf = getSplitCardHalves(cardImageForBestResult, AkhSplitCardTitle)[1];

			//Extract all search configurations that are not rotating the card.
			//This because if the title is OK then we know where the Amonkhet split title is.
			vector<ReadingConfiguration> splitSearchConfigs;
			splitSearchConfigs.push_back(ReadingConfiguration(currentReadingConfig.BinaryThreshold, false)); //The split title probably has the same light as the title so try the same threshold first.
			for (ReadingConfiguration config : configs) {
				if (!config.Rotate180Degrees &&
					config.BinaryThreshold != currentReadingConfig.BinaryThreshold) {

					splitSearchConfigs.push_back(config);
				}
			}

			//Fetch the result for the split half.
			CardNameInfo splithalfResult = readTitle1(akhSplitHalf, false, splitSearchConfigs, AkhSplitCardTitle);
			bool successfullResult = !hasResultFailed(splithalfResult) && splithalfResult.IsConfidentTitle(systemMethods, NORMAL_OCR_CONFIDENCE_THRESH);

			if (successfullResult) {

				//Join the titles to a split card name.
				bestResult = joinSplitCardTitles(bestResult, splithalfResult);
				bestResult.CardType = AkhSplitCardTitle;
			}
		}
		else {
			storeOcrConfidence(bestResult, ++numberOfCardReadTries);
		}

		//Quit searching when we have find an acceptable result.
		if (hasFoundGoodResult) { break; }
	}

	return bestResult;
}

CardNameInfo CardReader::readTitle2(Mat cardImage, ReadingConfiguration config, CardTitleType cardType) {

	//Try reading the title.
	return readTitle3(cardImage, cardType, config.BinaryThreshold);
}

CardNameInfo CardReader::readTitle3(const Mat cardImage, const CardTitleType titleType, const int binaryThreshold) {

	CardNameInfo result;
	bool couldExtractTitleImage = false, couldExtractCardTitleText = false;

	//Get title image assuming we got a normal card.
	vector<Mat> ocrReadyTitles;
	couldExtractTitleImage = extractOcrReadyTitle(cardImage, ocrReadyTitles, titleType, binaryThreshold);
	
	if (couldExtractTitleImage) {

		result = ocrReadTitle(ocrReadyTitles);
		couldExtractCardTitleText = result.IsConfidentTitle(systemMethods, NORMAL_OCR_CONFIDENCE_THRESH);
	}
	
	if (!couldExtractCardTitleText && titleType == NormalTitle) {

		//OK. Perhaps it's a split card?

		//Read the titles of the both halves.
		vector<Mat> halves = getSplitCardHalves(cardImage, SplitCardTitle);
		CardNameInfo resultA, resultB;
		resultA = readTitle3(halves[0], SplitCardTitle, binaryThreshold);

		//Result B is useless if result A has failed.
		bool couldGetSplitTitle = !hasResultFailed(resultA);
		if (couldGetSplitTitle) {
			resultB = readTitle3(halves[1], SplitCardTitle, binaryThreshold);
		}

		//Check if we got success.
		couldGetSplitTitle = !hasResultFailed(resultB);
		if (couldGetSplitTitle) {

			couldExtractCardTitleText = true;

			//Join the titles to a split card name.
			result = joinSplitCardTitles(resultA, resultB);
			result.CardType = SplitCardTitle;
		}

		//It should be considered as another try to read the card
		//if we have tried to read it as a split card.
		storeOcrConfidence(result, ++numberOfCardReadTries);
	}

	//Return empty result to indicate failure if no card title text could be extracted.
	return couldExtractCardTitleText ? result : CardNameInfo();
}

CardNameInfo CardReader::joinSplitCardTitles(CardNameInfo resultA, CardNameInfo resultB) {

	CardNameInfo result;
	result.CardName = resultA.CardName + L" // " + resultB.CardName;
	result.Confidence = min(resultA.Confidence, resultB.Confidence);

	return result;
}

void CardReader::storeOcrConfidence(CardNameInfo result, int numberOfTries) {

	if (runDebugging) {

		StoreCardProcessingData storer = StoreCardProcessingData(systemMethods);
		storer.StoreOcrConfidence(imageFileName, numberOfTries, result.CardName, result.Confidence);
	}
}

vector<Mat> CardReader::getSplitCardHalves(const Mat& originalCardImage, CardTitleType titleType) {

	Mat splitCard, halfA, halfB;

	originalCardImage.copyTo(splitCard);
	int rotation = (titleType == AkhSplitCardTitle) ? ROTATE_90_COUNTERCLOCKWISE : ROTATE_90_CLOCKWISE;
	rotate(splitCard, splitCard, rotation);

	//The first half is not needed for Amonkhet split cards.
	if (titleType != AkhSplitCardTitle) {

		//The extra border limit is because the card border is bigger in relation to the split card half.
		int extraBorderLimit = (int)(WORKING_CARD_HEIGHT / 9.7); //70
		Rect limitsHalfA(extraBorderLimit, 0, splitCard.cols / 2, splitCard.rows);
		ImageHelper::CropImage(splitCard, halfA, limitsHalfA);
	}

	Rect limitsHalfB((splitCard.cols / 2), 0, splitCard.cols / 2, splitCard.rows);
	ImageHelper::CropImage(splitCard, halfB, limitsHalfB);

	vector<Mat> halves{ halfA, halfB };
	return halves;
}

bool CardReader::hasResultFailed(CardNameInfo result) {

	if (result.Confidence > 0) { return false; }
	if (result.CardName != L"") { return false; }

	//Failure! We got a failure!
	return true;
}

CardNameInfo CardReader::ocrReadTitle(vector<Mat> ocrTitles) {

	OcrDecodeResult bestResult;
	ImageOcrHelper ocrReader(systemMethods);
	TitleLetterFixer titleFixer(systemMethods);
	
	for (Mat ocrTitle : ocrTitles) {

		OcrDecodeResult result = ocrReader.DecodeImage(ocrTitle);
		result.Text = titleFixer.FixTitleLetters(result.Text);

		if (bestResult.Confidence < result.Confidence) {
			bestResult = result;
		}
	}

	CardNameInfo cardInfo;
	cardInfo.CardName = bestResult.Text;
	cardInfo.Confidence = bestResult.Confidence;

	return cardInfo;
}

bool CardReader::extractOcrReadyTitle(const Mat cardImage, vector<Mat>& outImages, const CardTitleType type, const int binaryThreshold) {

	//Extract the title part.
	Mat titleSection;
	cropImageToTitleSection(cardImage, titleSection, type);

	//Prepare the title for OCR reading.
	TitleExtractor titleExtractor(imageFileName, titleSection, systemMethods, runDebugging);
	bool success = titleExtractor.ExtractTitle(outImages, binaryThreshold);

	//See if we need to stop.
	if (!success) {
		return false;
	}

	//Make white text on black background.
	for (size_t i = 0; i < outImages.size(); i++) {

		ImageHelper::SetBackgroundByInverting(outImages[i], true);

		//Store result for debugging.
		if (runDebugging) {
			wstring filename = systemMethods->AddToEndOfFilename(imageFileName, L"_" + to_wstring(++numberOfOcrTitlesStoredForDebug));
			SaveOcvImage::SaveImageData(systemMethods, outImages[i], filename, L"10 - OCR Prepared Title");
		}
	}

	return true;
}

void CardReader::cropImageToTitleSection(const Mat rawCardImage, Mat& outImage, CardTitleType type) {

	Rect titleBox;
	switch (type) {
	case NormalTitle:
		titleBox = MtgCardInfoHelper::GetNormalTitleSectionBox(rawCardImage.size());
		break;
	case SplitCardTitle:
		titleBox = MtgCardInfoHelper::GetSplitTitleSectionBox(rawCardImage.size());
		break;
	case AkhSplitCardTitle:
		titleBox = MtgCardInfoHelper::GetAmonkhetSplitTitleSectionBox(rawCardImage.size());
		break;
	}

	ImageHelper::CropImage(rawCardImage, outImage, titleBox);

	//Store result for debugging.
	if (runDebugging) {
		wstring filename = systemMethods->AddToEndOfFilename(imageFileName, L"_" + to_wstring(++numberOfTitleImagesStoredForDebug));
		SaveOcvImage::SaveImageData(systemMethods, outImage, filename, L"6 - Title Section");
	}
}
