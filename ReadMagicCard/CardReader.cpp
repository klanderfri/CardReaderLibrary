#include "stdafx.h"
#include "CardReader.h"
#include "CardExtractor.h"
#include "FileHandling.h"
#include "MtgCardInfoHelper.h"
#include "SaveOcvImage.h"
#include "TitleExtractor.h"
#include "ImageOcrHelper.h"
#include "StoreCardProcessingData.h"
#include "TitleLetterFixer.h"
#include "AlgorithmHelper.h"

using namespace cv;
using namespace std;

CardReader::CardReader(Session* session, wstring imageFilePath)
	: ImageHandlerBase(session, imageFilePath),
	NORMAL_OCR_CONFIDENCE_THRESH(75),
	HIGH_OCR_CONFIDENCE_THRESH(80)
{
}

CardReader::~CardReader()
{
}

wstring CardReader::GetimageFilePath() {

	return imageFilePath;
}

CardNameInfo CardReader::GetResult() {

	return finalResult;
}

void CardReader::ReadCardName(Mat cardImage) {

	//Reset variables keeping track of the number of X tries.
	numberOfOcrTitleImagesStoredForDebug = 0;
	numberOfTitleImagesStoredForDebug = 0;
	numberOfCardReadTries = 0;
	numberOfTitleImageExtractions = 0;

	//Extract the title text.
	vector<ReadingConfiguration> configs = createReadingConfigurations();
	finalResult = readTitleOfCardWithUnknownOrientation(cardImage, configs, NormalTitle);

	//Add additional data to the result container.
	finalResult.FilePath = imageFilePath;
	finalResult.FileName = session->systemMethods->GetFileNameFromFilePath(finalResult.FilePath);
	finalResult.CardType = getTitleType(finalResult);
	if (rotateFinalCardImage180Degrees) {
		rotate(cardImage, cardImage, ROTATE_180);
	}
	finalResult.ExtractedCardImage = cardImage;

	//Oops! Seems like we couldn't get any title text.
	if (!finalResult.HasGotResult()) {
		finalResult.CardName = L"ERROR: Could not OCR-read the title!";
		finalResult.Confidence = 0;
	}

	//Store result for debugging.
	if (session->runDebugging) {

		size_t size = finalResult.OcrTitleImages.size();
		for (size_t i = 0; i < size; i++) {

			SaveOcvImage::SaveImageData(session, finalResult.OcrTitleImages[i], finalResult.FileName, L"13 - Best OCR Title", i, size);
		}
	}
}

vector<ReadingConfiguration> CardReader::createReadingConfigurations() {

	vector<ReadingConfiguration> configs {
		ReadingConfiguration(120, false),
		ReadingConfiguration(120, true),
		ReadingConfiguration(80, false),
		ReadingConfiguration(140, false),
		ReadingConfiguration(80, true),
		ReadingConfiguration(140, true)
	};

	assert(!configs.empty());
	return configs;
}

CardNameInfo CardReader::readTitleOfCardWithUnknownOrientation(const Mat cardImage, const vector<ReadingConfiguration> configs, const CardTitleType cardTitleTypeToSearch) {

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
		CardNameInfo iterationResult = readTitleOfStraightCard(cardIterationCopy, currentReadingConfig, cardTitleTypeToSearch);

		//Store the best result for later use.
		if (bestResult.Confidence < iterationResult.Confidence) {

			bestResult = iterationResult;
			cardIterationCopy.copyTo(cardImageGivingBestResult);

			if (cardTitleTypeToSearch == NormalTitle) { //The need to rotate the final image should not be reset just because the sub-card did not need to be rotated!
				rotateFinalCardImage180Degrees = currentReadingConfig.Rotate180Degrees;
			}
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
		//This means we shouldn't store the confidence while we are running the loop fetching any titles of sub-cards of split cards.
		bool cardReadIterationFinished = (cardTitleTypeToSearch == NormalTitle);
		if (cardReadIterationFinished) {

			numberOfCardReadTries++;
			storeOcrConfidence(iterationResult, numberOfCardReadTries);
		}

		//Sometimes we can be pretty sure we got the title.
		if (isResultGoodEnoughToQuit(bestResult)) { break; }
	}

	return bestResult;
}

CardTitleType CardReader::getTitleType(const CardNameInfo info) {

	bool isEmblem = info.HasDarkTitleBackground && MtgCardInfoHelper::IsEmblem(info.CardName);
	bool isToken = !isEmblem && info.HasDarkTitleBackground && MtgCardInfoHelper::IsToken(info.CardName);
	bool isTransformed = !isToken && !info.HasDarkTitleBackground && MtgCardInfoHelper::IsToken(info.CardName);

	assert(isToken + isEmblem + isTransformed <= 1);

	if (isEmblem) {
		return Emblem;
	}
	else if (isToken) {
		return Token;
	}
	else if (isTransformed) {
		return TransformedTitle;
	}
	else {
		return info.CardType;
	}
}

bool CardReader::isResultGoodEnoughToQuit(const CardNameInfo result) {

	return result.Confidence >= HIGH_OCR_CONFIDENCE_THRESH;
}

bool CardReader::shouldWeExecuteAmonkhetSplitHalfSearch(const CardNameInfo currentBestResult, const size_t currentIterationIndex, const CardTitleType cardTitleTypeOfParentCard) {

	//First and formost, the loop most be the main one
	//(i.e working with a card that might contain an Amonkhet split card)
	//and not the one searching for the Amonkhet split card title.
	bool couldHaveAmonkhetSubCard =
		cardTitleTypeOfParentCard == NormalTitle &&
		getTitleType(currentBestResult) == NormalTitle;
	if (!couldHaveAmonkhetSubCard) { return false; }
	
	//No use to look for the Amonkhet split card title if the main title extraction was a complete failure.
	if (!currentBestResult.HasGotResult()) { return false; }

	//We can skip any further searches for the main title and go for the Amonkhet split one if the main one is excellent clear.
	bool excellentResult = isResultGoodEnoughToQuit(currentBestResult);
	if (excellentResult) { return true; }

	//The last iteration should include an Amonkhet title search as long as the main one is acceptable.
	bool isLastIterationSearchingForTitle = (currentIterationIndex == createReadingConfigurations().size() - 1);
	if (isLastIterationSearchingForTitle) { return true; }

	return false;
}

CardNameInfo CardReader::readAmonkhetSplitTitle(const Mat cardImageGivingBestResult, const ReadingConfiguration currentConfig)
{
	//Get the half that might contain the Amonkhet split card.
	Mat akhSplitHalf = getSplitCardHalves(cardImageGivingBestResult, AkhSplitCardTitle)[1];

	//The split title probably has the same light as the title so try the same threshold.
	ReadingConfiguration splitSearchConfig(currentConfig.BinaryThreshold, false);

	//Fetch the result for the split half.
	CardNameInfo splitHalfResult = readTitleOfStraightCard(akhSplitHalf, splitSearchConfig, AkhSplitCardTitle);
	bool successfullResult = splitHalfResult.IsConfidentTitle(NORMAL_OCR_CONFIDENCE_THRESH);

	//Return the result or indicate read failure with empty result.
	return successfullResult ? splitHalfResult : CardNameInfo();
}

CardNameInfo CardReader::readTitleOfStraightCard(const Mat cardImage, const ReadingConfiguration config, const CardTitleType cardTitleTypeToSearch) {

	CardNameInfo resultNormalCard, resultSplitCard, result;
	bool couldExtractTitleImage = false, couldExtractCardTitleText = false;

	//Get title image assuming we got a normal card.
	vector<Mat> ocrReadyTitles;
	bool hasDarkBackground;
	couldExtractTitleImage = extractOcrReadyTitle(cardImage, ocrReadyTitles, cardTitleTypeToSearch, config.BinaryThreshold, hasDarkBackground);
	
	if (couldExtractTitleImage) {

		//Try decode the title image, i.e getting the card name using OCR.
		resultNormalCard = ocrReadTitle(ocrReadyTitles);
		couldExtractCardTitleText = resultNormalCard.IsConfidentTitle(NORMAL_OCR_CONFIDENCE_THRESH);
	}
	
	if (!couldExtractCardTitleText && cardTitleTypeToSearch == NormalTitle) {

		//OK. Perhaps it's a split card?
		resultSplitCard = readSplitCardTitle(cardImage, config);
	}

	result = (resultNormalCard.Confidence > resultSplitCard.Confidence) ? resultNormalCard : resultSplitCard;
	result.HasDarkTitleBackground = hasDarkBackground;
	return result;
}

CardNameInfo CardReader::readSplitCardTitle(const Mat cardImage, const ReadingConfiguration config)
{
	CardNameInfo result;

	//Read the titles of the both halves.
	vector<Mat> halves = getSplitCardHalves(cardImage, SplitCardTitle);
	CardNameInfo resultA, resultB;
	resultA = readTitleOfStraightCard(halves[0], config, SplitCardTitle);

	//Result B is useless if result A has failed.
	if (resultA.HasGotResult()) {
		resultB = readTitleOfStraightCard(halves[1], config, SplitCardTitle);
	}

	//Check if we got success.
	if (resultB.HasGotResult()) {

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

	if (session->runDebugging) {

		StoreCardProcessingData storer = StoreCardProcessingData(session);
		storer.StoreOcrConfidence(imageFilePath, numberOfTries, result.CardName, result.Confidence);
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
		int extraBorderLimit = (int)(session->WORKING_CARD_HEIGHT / 9.7); //70
		Rect limitsHalfA(extraBorderLimit, 0, splitCard.cols / 2, splitCard.rows);
		session->imageHelper->imageEditor->CropImage(splitCard, halfA, limitsHalfA);
	}

	Rect limitsHalfB((splitCard.cols / 2), 0, splitCard.cols / 2, splitCard.rows);
	session->imageHelper->imageEditor->CropImage(splitCard, halfB, limitsHalfB);

	vector<Mat> halves{ halfA, halfB };
	return halves;
}

CardNameInfo CardReader::ocrReadTitle(const vector<Mat> ocrTitles) {

	OcrDecodeResult bestResult;
	ImageOcrHelper ocrReader(session);
	TitleLetterFixer titleFixer(session);
	
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

bool CardReader::extractOcrReadyTitle(const Mat cardImage, vector<Mat>& outImages, const CardTitleType titleType, const int binaryThreshold, bool& titleHasDarkBackground) {

	//Extract the title part.
	Mat titleSection;
	cropImageToTitleSection(cardImage, titleSection, titleType);

	//Prepare the title for OCR reading.
	TitleExtractor titleExtractor(session, imageFilePath, titleSection);
	bool success = titleExtractor.ExtractTitle(outImages, binaryThreshold, numberOfTitleImageExtractions, titleType);

	//See if we need to stop.
	if (!success) {
		return false;
	}

	//Store the background colour for later "Token or Transformed" check.
	titleHasDarkBackground = titleExtractor.HasOriginalTitleBlackBackground();

	//Make white text on black background.
	for (size_t i = 0; i < outImages.size(); i++) {

		session->imageHelper->imageEditor->SetBackgroundByInverting(outImages[i], true);

		//Store result for debugging.
		if (session->runDebugging) {
			SaveOcvImage::SaveImageData(session, outImages[i], imageFileName, L"12 - OCR Prepared Title", ++numberOfOcrTitleImagesStoredForDebug);
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

	session->imageHelper->imageEditor->CropImage(cardImage, outImage, titleBox);

	//Store result for debugging.
	if (session->runDebugging) {
		SaveOcvImage::SaveImageData(session, outImage, imageFileName, L"6 - Title Section", ++numberOfTitleImagesStoredForDebug);
	}
}
