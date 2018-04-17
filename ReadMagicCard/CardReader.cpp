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

	//Extract the title text.
	vector<ReadingConfiguration> configs = createReadingConfigurations();
	finalResult = readUnrotatedCardTitle(cardImage, configs, NormalTitle, true);
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

vector<ReadingConfiguration> CardReader::createReadingConfigurations() {

	vector<ReadingConfiguration> configs{
		ReadingConfiguration(120, false),
		ReadingConfiguration(120, true),
		ReadingConfiguration(80, false),
		ReadingConfiguration(140, false),
		ReadingConfiguration(80, true),
		ReadingConfiguration(140, true)
	};

	return configs;
}

CardNameInfo CardReader::readUnrotatedCardTitle(const Mat cardImage, vector<ReadingConfiguration> configs, CardTitleType cardType, bool searchForAkhSplitHalf) {

	CardNameInfo bestResult;
	Mat cardImageGivingBestResult;

	for (size_t i = 0; i < configs.size(); i++) {

		ReadingConfiguration currentReadingConfig = configs[i];

		//Copy the card image to a temporary variable to avoid messing up the original image.
		Mat cardIterationCopy;
		cardImage.copyTo(cardIterationCopy);

		//Sometimes cards get rotated. Rotate it back to get the title.
		if (currentReadingConfig.Rotate180Degrees) {
			rotate(cardIterationCopy, cardIterationCopy, ROTATE_180);
		}

		CardNameInfo tempResult = readStraightCardTitle(cardIterationCopy, currentReadingConfig, cardType);

		//Store the best result for later use.
		if (bestResult.Confidence < tempResult.Confidence) {
			bestResult = tempResult;
			cardIterationCopy.copyTo(cardImageGivingBestResult);
		}

		//It's only useful to search for Amonkhet split card in certain circumstances.
		bool doExecuteSearch = shouldWeExecuteAmonkhetSplitHalfSearch(searchForAkhSplitHalf, bestResult, i, cardType);
		if (doExecuteSearch) {

			//Read the split half.
			CardNameInfo splitHalfResult = readAmonkhetSplitTitle(cardImageGivingBestResult, currentReadingConfig, bestResult);

			//Join the titles to a split card name.
			if (splitHalfResult.IsConfidentTitle(systemMethods, NORMAL_OCR_CONFIDENCE_THRESH)) {
			
				bestResult = joinSplitCardTitles(bestResult, splitHalfResult);
				bestResult.CardType = AkhSplitCardTitle;
			}
		}

		//This is a little un-intuitive, but this is the case:
		//This loop is executed an extra time when we find a title, to get any Amonkhet split card title.
		//We only want to store the confidence for entire titles.
		//This means we shouldn't store the confidence while we are running the loop fetching the Amonkhet split card title.
		bool cardReadIterationFinished = searchForAkhSplitHalf;
		if (cardReadIterationFinished) {

			numberOfCardReadTries++;
			storeOcrConfidence(bestResult, numberOfCardReadTries);
		}

		//Sometimes we can be pretty sure we got the title.
		if (isResultGoodEnoughToQuit(bestResult)) { break; }
	}

	return bestResult;
}

bool CardReader::isResultGoodEnoughToQuit(CardNameInfo result) {

	return result.Confidence > HIGH_OCR_CONFIDENCE_THRESH;
}

bool CardReader::shouldWeExecuteAmonkhetSplitHalfSearch(const bool searchForAkhSplitHalf, const CardNameInfo currentBestResult, const size_t currentIterationIndex, const CardTitleType cardType) {

	bool acceptableResult = (currentBestResult.Confidence > NORMAL_OCR_CONFIDENCE_THRESH);
	bool isLastIteration = (currentIterationIndex == createReadingConfigurations().size() - 1);

	bool executeSearchForAkhSplitHalf =
		searchForAkhSplitHalf &&
		(isResultGoodEnoughToQuit(currentBestResult) || acceptableResult && isLastIteration) &&
		cardType == NormalTitle;

	return executeSearchForAkhSplitHalf;
}

CardNameInfo CardReader::readAmonkhetSplitTitle(const Mat cardImageForBestResult, const ReadingConfiguration currentConfig, CardNameInfo &bestResult)
{
	//Get the half that might contain the Amonkhet split card.
	Mat akhSplitHalf = getSplitCardHalves(cardImageForBestResult, AkhSplitCardTitle)[1];

	//Extract all search configurations that are not rotating the card.
	//This because if the title is OK then we know where the Amonkhet split title is.
	vector<ReadingConfiguration> splitSearchConfigs;

	//The split title probably has the same light as the title so try the same threshold first.
	splitSearchConfigs.push_back(ReadingConfiguration(currentConfig.BinaryThreshold, false));

	//Add other relevant configurations to the collection.
	for (ReadingConfiguration config : createReadingConfigurations()) {

		if (!config.Rotate180Degrees &&
			config.BinaryThreshold != currentConfig.BinaryThreshold) {

			splitSearchConfigs.push_back(config);
		}
	}

	//Fetch the result for the split half.
	CardNameInfo splitHalfResult = readUnrotatedCardTitle(akhSplitHalf, splitSearchConfigs, AkhSplitCardTitle, false);
	bool successfullResult = splitHalfResult.IsSuccessful() && splitHalfResult.IsConfidentTitle(systemMethods, NORMAL_OCR_CONFIDENCE_THRESH);

	//Return the result or indicate read failure with empty result.
	return successfullResult ? splitHalfResult : CardNameInfo();
}

CardNameInfo CardReader::readStraightCardTitle(const Mat cardImage, const ReadingConfiguration config, const CardTitleType titleType) {

	CardNameInfo result;
	bool couldExtractTitleImage = false, couldExtractCardTitleText = false;

	//Get title image assuming we got a normal card.
	vector<Mat> ocrReadyTitles;
	couldExtractTitleImage = extractOcrReadyTitle(cardImage, ocrReadyTitles, titleType, config.BinaryThreshold);
	
	if (couldExtractTitleImage) {

		//Try decode the title image, i.e getting the card name using OCR.
		result = ocrReadTitle(ocrReadyTitles);
		couldExtractCardTitleText = result.IsConfidentTitle(systemMethods, NORMAL_OCR_CONFIDENCE_THRESH);
	}
	
	if (!couldExtractCardTitleText && titleType == NormalTitle) {

		//OK. Perhaps it's a split card?
		result = readSplitCardTitle(cardImage, config);
		couldExtractCardTitleText = result.IsSuccessful();
	}

	//Return empty result to indicate failure if no card title text could be extracted.
	return couldExtractCardTitleText ? result : CardNameInfo();
}

CardNameInfo CardReader::readSplitCardTitle(Mat cardImage, const ReadingConfiguration config)
{
	CardNameInfo result;

	//Read the titles of the both halves.
	vector<Mat> halves = getSplitCardHalves(cardImage, SplitCardTitle);
	CardNameInfo resultA, resultB;
	resultA = readStraightCardTitle(halves[0], config, SplitCardTitle);

	//Result B is useless if result A has failed.
	if (resultA.IsSuccessful()) {
		resultB = readStraightCardTitle(halves[1], config, SplitCardTitle);
	}

	//Check if we got success.
	if (resultB.IsSuccessful()) {

		//Join the titles to a split card name.
		result = joinSplitCardTitles(resultA, resultB);
		result.CardType = SplitCardTitle;
	}

	return result;
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
