#include "stdafx.h"
#include "OcvCardReader.h"
#include "OcvAlgorithmHelper.h"
#include "OcvCardExtractor.h"
#include "OcvFileHandling.h"
#include "OcvLoadImage.h"
#include "OcvMtgCardInfoHelper.h"
#include "OcvSaveImage.h"
#include "OcvTitleExtractor.h"
#include "OcvTitleOcrReader.h"

using namespace cv;
using namespace std;

OcvCardReader::OcvCardReader(wstring imageFileName, OcvSystemDependencyClass* systemMethods, bool doDebugging)
	: OcvCardHandler(imageFileName, Mat(), systemMethods, doDebugging),
	TITLE_FORM({ 0.0441176, 0.0661765, 1 })
{
}

OcvCardReader::~OcvCardReader()
{
}

wstring OcvCardReader::GetImageFileName() {

	return imageFileName;
}

bool OcvCardReader::GetSuccess() {

	return m_success;
}

wstring OcvCardReader::ExtractCardName() {

	//Load the image.
	Mat originalImage = OcvLoadImage::LoadImageData(systemMethods, imageFileName);

	//Extract the card part.
	OcvCardExtractor cardExtractor(imageFileName, originalImage, systemMethods, doDebugging);
	Mat cardImage;
	bool success = cardExtractor.ExtractCard(cardImage);

	//See if we need to stop.
	if (!success) {
		throw OcvOperationException("ERROR: Could not extract the card section!");
	}

	//Try reading the title.
	auto result = readTitle(cardImage, 1);

	//Check if the algorithm is confident enought of the decoding that we might return the title string.
	if (isConfidentOfTitleDecode(result.first, result.second)) {

		m_success = true;
		return result.first;
	}

	//Try rotate the card. It might be that easy...
	rotate(cardImage, cardImage, ROTATE_180);

	//Try reading the title again.
	result = readTitle(cardImage, 2);

	//Check if the algorithm is confident enought of the decoding that we might return the title string.
	if (isConfidentOfTitleDecode(result.first, result.second)) {

		m_success = true;
		return result.first;
	}

	throw OcvOperationException("ERROR: Could not OCR-read the title!");
}

pair<wstring, int> OcvCardReader::readTitle(Mat cardImage, int numberOfTries) {

	//Get the OCR ready title.
	Mat ocrTitle;
	bool success = extractOcrReadyTitle(cardImage, ocrTitle);

	if (!success) {
		//We couldn't extract the title so return failure.
		return make_pair(L"", 0);
	}

	//Read the title.
	OcvTitleOcrReader titleReader(imageFileName, ocrTitle, systemMethods, doDebugging);
	auto result = titleReader.DecodeTitle();

	//Store the confidence
	storeConfidence(numberOfTries, result.first, result.second);

	return result;
}

bool OcvCardReader::extractOcrReadyTitle(const Mat cardImage, Mat& outImage) {

	//Extract the title part.
	cropImageToTitleSection(cardImage, outImage);

	//Prepare the tile for OCR reading.
	OcvTitleExtractor titleExtractor(imageFileName, outImage, systemMethods, doDebugging);
	bool success = titleExtractor.ExtractTitle(outImage);

	//See if we need to stop.
	if (!success) {
		return false;
	}

	//Make white text on black background.
	OcvImageHelper::SetBackgroundByInverting(outImage, true);

	//Store result for debugging.
	if (doDebugging) {
		OcvSaveImage::SaveImageData(systemMethods, outImage, imageFileName, L"10 - OCR Prepared Title");
	}

	return true;
}

void OcvCardReader::storeConfidence(int numberOfTries, wstring ocrResult, int ocrConfidence) {

	if (doDebugging) {

		wstring textfileName = L"TitleDecodeConfidence.txt";
		wstring subfolderName = L"Image Data";
		wstring numberOfTriesStr = systemMethods->ToWString(to_string(numberOfTries));
		wstring confidenceStr = systemMethods->ToWString(to_string(ocrConfidence));

		wstring rowToAdd = numberOfTriesStr + L"\t" + imageFileName + L"\t" + ocrResult + L"\t" + confidenceStr;
		OcvFileHandling::AddRowToFile(systemMethods, rowToAdd, textfileName, subfolderName);
	}
}

bool OcvCardReader::isConfidentOfTitleDecode(wstring title, int confidence) {

	//The Tesseract algorithm needs to be confident with the result otherwise we shouldn't either.
	if (confidence < 75) { return false; }

	//The shortest card name is three letters.
	if (title.length() < (size_t)OcvMtgCardInfoHelper::LettersInShortestCardName()) { return false; }

	//Check for illegal characters.
	if (containsInvalidCharacters(title)) { return false; }

	return true;
}

bool OcvCardReader::containsInvalidCharacters(wstring title) {

	//Make the title lowercase to make it easier to work with.
	string titleStr = systemMethods->ToString(title);
	transform(titleStr.begin(), titleStr.end(), titleStr.begin(), ::tolower);

	vector<char> allowedCharacters
	{
		' ', '-', '\'', ',', 'æ',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};

	//Check for illegal characters.
	for (char letter : titleStr) {
		if (!OcvAlgorithmHelper::VectorContains(allowedCharacters, letter)) {
			return true;
		}
	}

	return false;
}

void OcvCardReader::cropImageToTitleSection(const Mat rawCardImage, Mat& outImage) {

	Rect titleBox = OcvMtgCardInfoHelper::GetNormalTitleSectionBox(rawCardImage.size());
	OcvImageHelper::CropImage(rawCardImage, outImage, titleBox);

	//Store result for debugging.
	if (doDebugging) {
		OcvSaveImage::SaveImageData(systemMethods, outImage, imageFileName, L"6 - Title Section");
	}
}
