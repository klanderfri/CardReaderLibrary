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
#include "AlgorithmHelper.h"

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
	numberOfOcrTitleImagesStoredForDebug = 0;
	numberOfTitleImagesStoredForDebug = 0;
	numberOfCardReadTries = 0;
	numberOfTitleImageExtractions = 0;

	//Load the image.
	Mat originalCardImage = LoadOcvImage::LoadImageData(systemMethods, imageFileName);

	//Extract the card part.
	Mat cardImage = extractCardImage(originalCardImage);

	//Extract the title text.
	vector<ReadingConfiguration> configs = createReadingConfigurations();
	finalResult = readUnrotatedCardTitle(cardImage, configs, NormalTitle);
	finalResult.FileName = imageFileName;

	//Oops! Seems like we couldn't get any title text.
	if (!finalResult.IsSuccessful()) {
		finalResult.CardName = L"ERROR: Could not OCR-read the title!";
	}

	//Store result for debugging.
	if (runDebugging) {

		size_t size = finalResult.OcrTitleImages.size();
		for (size_t i = 0; i < size; i++) {

			wstring imageNumberPostfix = L" (" + to_wstring(i + 1) + L" of " + to_wstring(size) + L")";
			wstring filename = systemMethods->AddToEndOfFilename(finalResult.FileName, imageNumberPostfix);
			SaveOcvImage::SaveImageData(systemMethods, finalResult.OcrTitleImages[i], filename, L"11 - Best OCR Title");
		}
	}
}

Mat CardReader::extractCardImage(const Mat originalCardImage) {

	Mat cardImage;
	CardExtractor cardExtractor(imageFileName, originalCardImage, systemMethods, runDebugging);
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

CardNameInfo CardReader::readUnrotatedCardTitle(const Mat cardImage, const vector<ReadingConfiguration> configs, const CardTitleType cardTitleTypeToSearch) {

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

		//Try reading the card.
		CardNameInfo tempResult = readStraightCardTitle(cardIterationCopy, currentReadingConfig, cardTitleTypeToSearch);

		//Store the best result for later use.
		if (bestResult.Confidence < tempResult.Confidence) {
			bestResult = tempResult;
			cardIterationCopy.copyTo(cardImageGivingBestResult);
		}

		//It's only useful to search for Amonkhet split card in certain circumstances.
		bool doExecuteSearch = shouldWeExecuteAmonkhetSplitHalfSearch(bestResult, i, cardTitleTypeToSearch);
		if (doExecuteSearch) {

			//Read the split half.
			CardNameInfo splitHalfResult = readAmonkhetSplitTitle(cardImageGivingBestResult, currentReadingConfig);

			//Join the titles to a split card name.
			if (splitHalfResult.IsConfidentTitle(NORMAL_OCR_CONFIDENCE_THRESH)) {
			
				bestResult = joinSplitCardTitles(bestResult, splitHalfResult);
				bestResult.CardType = AkhSplitCardTitle;
			}
		}

		//This is a little un-intuitive, but this is the case:
		//This loop is executed an extra time when we find a title, to get any Amonkhet split card title.
		//We only want to store the confidence for entire titles.
		//This means we shouldn't store the confidence while we are running the loop fetching the Amonkhet split card title.
		bool cardReadIterationFinished = (cardTitleTypeToSearch == NormalTitle);
		if (cardReadIterationFinished) {

			numberOfCardReadTries++;
			storeOcrConfidence(bestResult, numberOfCardReadTries);
		}

		//Sometimes we can be pretty sure we got the title.
		if (isResultGoodEnoughToQuit(bestResult)) { break; }
	}

	return bestResult;
}

bool CardReader::isResultGoodEnoughToQuit(const CardNameInfo result) {

	return result.Confidence >= HIGH_OCR_CONFIDENCE_THRESH;
}

bool CardReader::shouldWeExecuteAmonkhetSplitHalfSearch(const CardNameInfo currentBestResult, const size_t currentIterationIndex, const CardTitleType cardTitleTypeToSearch) {

	//First and formost, the loop most be the main one and not the one searching for the Amonkhet split card title.
	if (cardTitleTypeToSearch != NormalTitle) { return false; }

	//Continue searching for a better main title if the current one us just acceptable.
	bool acceptableResult = (currentBestResult.Confidence >= NORMAL_OCR_CONFIDENCE_THRESH);
	if (!acceptableResult) { return false; }

	//Emblems are not split cards.
	bool isEmblem = MtgCardInfoHelper::IsEmblem(currentBestResult.CardName);
	if (isEmblem) { return false; }

	//We can skip any further searches for the main title and go for the Amonkhet split one if the main one is excellent clear.
	bool excellentResult = isResultGoodEnoughToQuit(currentBestResult);
	if (excellentResult) { return true; }
	
	//The last iteration should include an Amonkhet title search as long as the main one is acceptable.
	bool isLastIteration = (currentIterationIndex == createReadingConfigurations().size() - 1);
	if (isLastIteration) { return true; }

	return false;
}

CardNameInfo CardReader::readAmonkhetSplitTitle(const Mat cardImageGivingBestResult, const ReadingConfiguration currentConfig)
{
	//Get the half that might contain the Amonkhet split card.
	Mat akhSplitHalf = getSplitCardHalves(cardImageGivingBestResult, AkhSplitCardTitle)[1];

	//The split title probably has the same light as the title so try the same threshold.
	vector<ReadingConfiguration> splitSearchConfigs { ReadingConfiguration(currentConfig.BinaryThreshold, false) };

	//Fetch the result for the split half.
	CardNameInfo splitHalfResult = readUnrotatedCardTitle(akhSplitHalf, splitSearchConfigs, AkhSplitCardTitle);
	bool successfullResult = splitHalfResult.IsSuccessful() && splitHalfResult.IsConfidentTitle(NORMAL_OCR_CONFIDENCE_THRESH);

	//Return the result or indicate read failure with empty result.
	return successfullResult ? splitHalfResult : CardNameInfo();
}

CardNameInfo CardReader::readStraightCardTitle(const Mat cardImage, const ReadingConfiguration config, const CardTitleType cardTitleTypeToSearch) {

	CardNameInfo result;
	bool couldExtractTitleImage = false, couldExtractCardTitleText = false;

	//Get title image assuming we got a normal card.
	vector<Mat> ocrReadyTitles;
	couldExtractTitleImage = extractOcrReadyTitle(cardImage, ocrReadyTitles, cardTitleTypeToSearch, config.BinaryThreshold);
	
	if (couldExtractTitleImage) {

		//Try decode the title image, i.e getting the card name using OCR.
		result = ocrReadTitle(ocrReadyTitles);
		couldExtractCardTitleText = result.IsConfidentTitle(NORMAL_OCR_CONFIDENCE_THRESH);
	}
	
	if (!couldExtractCardTitleText && cardTitleTypeToSearch == NormalTitle) {

		//OK. Perhaps it's a split card?
		result = readSplitCardTitle(cardImage, config);
		couldExtractCardTitleText = result.IsSuccessful();
	}

	//Return empty result to indicate failure if no card title text could be extracted.
	return couldExtractCardTitleText ? result : CardNameInfo();
}

CardNameInfo CardReader::readSplitCardTitle(const Mat cardImage, const ReadingConfiguration config)
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

CardNameInfo CardReader::joinSplitCardTitles(const CardNameInfo resultA, const CardNameInfo resultB) {

	CardNameInfo result;
	result.CardName = resultA.CardName + L" // " + resultB.CardName;
	result.Confidence = min(resultA.Confidence, resultB.Confidence);
	result.OcrTitleImages = AlgorithmHelper::JoinVectors(resultA.OcrTitleImages, resultB.OcrTitleImages);

	return result;
}

void CardReader::storeOcrConfidence(const CardNameInfo result, const int numberOfTries) {

	if (runDebugging) {

		StoreCardProcessingData storer = StoreCardProcessingData(systemMethods);
		storer.StoreOcrConfidence(imageFileName, numberOfTries, result.CardName, result.Confidence);
	}
}

vector<Mat> CardReader::getSplitCardHalves(const Mat cardImage, const CardTitleType titleType) {

	Mat splitCard, halfA, halfB;

	cardImage.copyTo(splitCard);
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

CardNameInfo CardReader::ocrReadTitle(const vector<Mat> ocrTitles) {

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
	cardInfo.OcrTitleImages.push_back(bestResult.Image);

	return cardInfo;
}

bool CardReader::extractOcrReadyTitle(const Mat cardImage, vector<Mat>& outImages, const CardTitleType titleType, const int binaryThreshold) {

	//Extract the title part.
	Mat titleSection;
	cropImageToTitleSection(cardImage, titleSection, titleType);

	//Prepare the title for OCR reading.
	TitleExtractor titleExtractor(imageFileName, titleSection, systemMethods, runDebugging);
	bool success = titleExtractor.ExtractTitle(outImages, binaryThreshold, numberOfTitleImageExtractions);

	//See if we need to stop.
	if (!success) {
		return false;
	}

	//Make white text on black background.
	for (size_t i = 0; i < outImages.size(); i++) {

		ImageHelper::SetBackgroundByInverting(outImages[i], true);

		//Store result for debugging.
		if (runDebugging) {
			numberOfOcrTitleImagesStoredForDebug++;
			wstring filename = systemMethods->AddToEndOfFilename(imageFileName, L"_" + to_wstring(numberOfOcrTitleImagesStoredForDebug));
			SaveOcvImage::SaveImageData(systemMethods, outImages[i], filename, L"10 - OCR Prepared Title");
		}
	}

	return true;
}

void CardReader::cropImageToTitleSection(const Mat cardImage, Mat& outImage, const CardTitleType titleType) {

	Rect titleBox;
	switch (titleType) {
	case NormalTitle:
		titleBox = MtgCardInfoHelper::GetNormalTitleSectionBox(cardImage.size());
		break;
	case SplitCardTitle:
		titleBox = MtgCardInfoHelper::GetSplitTitleSectionBox(cardImage.size());
		break;
	case AkhSplitCardTitle:
		titleBox = MtgCardInfoHelper::GetAmonkhetSplitTitleSectionBox(cardImage.size());
		break;
	}

	ImageHelper::CropImage(cardImage, outImage, titleBox);

	//Store result for debugging.
	if (runDebugging) {
		numberOfTitleImagesStoredForDebug++;
		wstring filename = systemMethods->AddToEndOfFilename(imageFileName, L"_" + to_wstring(numberOfTitleImagesStoredForDebug));
		SaveOcvImage::SaveImageData(systemMethods, outImage, filename, L"5 - Title Section");
	}
}
