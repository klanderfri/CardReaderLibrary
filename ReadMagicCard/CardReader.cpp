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
#include "boost\algorithm\string.hpp"

using namespace cv;
using namespace std;

CardReader::CardReader(wstring imageFileName, SystemMethods* systemMethods, bool runDebugging)
	: BasicReaderData(imageFileName, Mat(), systemMethods, runDebugging)
{
}

CardReader::~CardReader()
{
}

wstring CardReader::GetImageFileName() {

	return imageFileName;
}

bool CardReader::GetSuccess() {

	return m_success;
}

int CardReader::GetConfidence() {

	return m_confidence;
}

wstring CardReader::ExtractCardName() {

	//Load the image.
	Mat originalImage = LoadOcvImage::LoadImageData(systemMethods, imageFileName);

	//Extract the card part.
	CardExtractor cardExtractor(imageFileName, originalImage, systemMethods, runDebugging);
	Mat cardImage;
	bool success = cardExtractor.ExtractCard(cardImage);

	//See if we need to stop.
	if (!success) {
		throw OperationException("ERROR: Could not extract the card section!");
	}

	//Try reading the title.
	int numberOfTries = 0;
	auto result = readTitle(cardImage, numberOfTries, NormalTitle);

	//The method reading the title returns an empty result if it failed.
	if (result.Confidence > 0) {

		m_success = true;
		return result.Text;
	}

	//Try rotate the card. It might be that easy...
	rotate(cardImage, cardImage, ROTATE_180);

	//Try reading the title again.
	result = readTitle(cardImage, numberOfTries, NormalTitle);

	//The method reading the title returns an empty result if it failed.
	if (result.Confidence > 0) {

		m_success = true;
		return result.Text;
	}

	throw OperationException("ERROR: Could not OCR-read the title!");
}

OcrDecodeResult CardReader::readTitle(Mat cardImage, int& numberOfTries, CardTitleType titleType) {

	OcrDecodeResult result;

	//Get title assuming we got a normal card.
	vector<Mat> ocrReadyTitles;
	bool success = extractOcrReadyTitle(cardImage, ocrReadyTitles, titleType);
	
	if (success) {

		//Read the title.
		result = ocrReadTitle(ocrReadyTitles);
		success = isConfidentOfTitleDecode(result.Text, result.Confidence);

		if (success && titleType == NormalTitle) {

			//Great! But could it be an Amonkhet split card?

			vector<Mat> halves = getSplitCardHalves(cardImage, AkhSplitCardTitle);
			OcrDecodeResult splitResultB = readTitle(halves[1], numberOfTries, AkhSplitCardTitle);

			if (!hasResultFailed(splitResultB)) {

				//Join the titles to a split card name.
				OcrDecodeResult splitResult;
				splitResult.Text = result.Text + L" // " + splitResultB.Text;
				splitResult.Confidence = min(result.Confidence, splitResultB.Confidence);

				bool splitSuccess = isConfidentOfTitleDecode(splitResult.Text, splitResult.Confidence);
				if (splitSuccess) {
					return splitResult;
				}
			}
		}
	}

	if (!success && titleType == NormalTitle) {

		//OK. Perhaps it's a split card?
		
		//Read the titles of the both halves.
		vector<Mat> halves = getSplitCardHalves(cardImage, SplitCardTitle);
		OcrDecodeResult resultA = readTitle(halves[0], numberOfTries, SplitCardTitle);
		OcrDecodeResult resultB = readTitle(halves[1], numberOfTries, SplitCardTitle);

		//Check if we got success.
		success = !hasResultFailed(resultA) && !hasResultFailed(resultB);

		if (success) {
			//Join the titles to a split card name.
			result.Text = resultA.Text + L" // " + resultB.Text;
			result.Confidence = min(resultA.Confidence, resultB.Confidence);

			success = isConfidentOfTitleDecode(result.Text, result.Confidence);
		}
	}

	if (!success) {
		//We couldn't extract the title so return failure.
		return OcrDecodeResult();
	}

	//Store the confidence
	if (runDebugging) {
		StoreCardProcessingData storer = StoreCardProcessingData(systemMethods);
		storer.StoreOcrConfidence(imageFileName, ++numberOfTries, result.Text, result.Confidence);
	}
	m_confidence = result.Confidence;

	return result;
}

vector<Mat> CardReader::getSplitCardHalves(const Mat& originalCardImage, CardTitleType titleType) {

	Mat splitCard, halfA, halfB;

	originalCardImage.copyTo(splitCard);
	int rotation = (titleType == AkhSplitCardTitle) ? ROTATE_90_COUNTERCLOCKWISE : ROTATE_90_CLOCKWISE;
	rotate(splitCard, splitCard, rotation);

	//The first half is not needed for Amonkhet split cards.
	if (titleType != AkhSplitCardTitle) {

		//The extra border limit is because the card border is bigger in relation to the split card half.
		int extraBorderLimit = (int)(WORKING_CARD_HEIGHT / 17.0); //40
		Rect limitsHalfA(extraBorderLimit, 0, splitCard.cols / 2, splitCard.rows);
		ImageHelper::CropImage(splitCard, halfA, limitsHalfA);
	}

	Rect limitsHalfB((splitCard.cols / 2), 0, splitCard.cols / 2, splitCard.rows);
	ImageHelper::CropImage(splitCard, halfB, limitsHalfB);

	vector<Mat> halves{ halfA, halfB };
	return halves;
}

bool CardReader::hasResultFailed(OcrDecodeResult result) {

	if (result.Confidence > 0) { return false; }
	if (result.Text != L"") { return false; }

	//Failure! We got a failure!
	return true;
}

OcrDecodeResult CardReader::ocrReadTitle(vector<Mat> ocrTitles) {

	OcrDecodeResult bestResult;
	ImageOcrHelper ocrReader(systemMethods);
	
	for (Mat ocrTitle : ocrTitles) {

		OcrDecodeResult result = ocrReader.DecodeImage(ocrTitle);
		result.Text = MtgCardInfoHelper::ChangeNumbersToLetters(systemMethods, result.Text);

		if (bestResult.Confidence < result.Confidence) {
			bestResult = result;
		}
	}

	return bestResult;
}

bool CardReader::extractOcrReadyTitle(const Mat cardImage, vector<Mat>& outImages, CardTitleType type) {

	//Extract the title part.
	Mat titleSection;
	cropImageToTitleSection(cardImage, titleSection, type);

	//Prepare the title for OCR reading.
	TitleExtractor titleExtractor(imageFileName, titleSection, systemMethods, runDebugging);
	bool success = titleExtractor.ExtractTitle(outImages);

	//See if we need to stop.
	if (!success) {
		return false;
	}

	//Make white text on black background.
	for (size_t i = 0; i < outImages.size(); i++) {

		ImageHelper::SetBackgroundByInverting(outImages[i], true);

		//Store result for debugging.
		if (runDebugging) {
			wstring filename = systemMethods->AddToEndOfFilename(imageFileName, L"_" + to_wstring(i + 1));
			SaveOcvImage::SaveImageData(systemMethods, outImages[i], filename, L"10 - OCR Prepared Title");
		}
	}

	return true;
}

bool CardReader::isConfidentOfTitleDecode(wstring title, int confidence) {

	//The Tesseract algorithm needs to be confident with the result otherwise we shouldn't either.
	if (confidence < 75) { return false; }

	//The shortest card name is three letters.
	if (title.length() < (size_t)MtgCardInfoHelper::LettersInShortestCardName()) { return false; }

	//Check for illegal characters.
	if (MtgCardInfoHelper::ContainsInvalidCharacters(systemMethods, title)) { return false; }

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
		SaveOcvImage::SaveImageData(systemMethods, outImage, imageFileName, L"6 - Title Section");
	}
}
