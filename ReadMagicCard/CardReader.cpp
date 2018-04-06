#include "stdafx.h"
#include "CardReader.h"
#include "AlgorithmHelper.h"
#include "CardExtractor.h"
#include "FileHandling.h"
#include "LoadOcvImage.h"
#include "MtgCardInfoHelper.h"
#include "SaveOcvImage.h"
#include "TitleExtractor.h"
#include "ImageOcrHelper.h"
#include "boost\algorithm\string.hpp"

using namespace cv;
using namespace std;

CardReader::CardReader(wstring imageFileName, SystemMethods* systemMethods, bool doDebugging)
	: BasicReaderData(imageFileName, Mat(), systemMethods, doDebugging)
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
	CardExtractor cardExtractor(imageFileName, originalImage, systemMethods, doDebugging);
	Mat cardImage;
	bool success = cardExtractor.ExtractCard(cardImage);

	//See if we need to stop.
	if (!success) {
		throw OperationException("ERROR: Could not extract the card section!");
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

	throw OperationException("ERROR: Could not OCR-read the title!");
}

pair<wstring, int> CardReader::readTitle(Mat cardImage, int numberOfTries) {

	//Get the OCR ready title.
	Mat ocrTitle;
	bool success = extractOcrReadyTitle(cardImage, ocrTitle);

	if (!success) {
		//We couldn't extract the title so return failure.
		return make_pair(L"", 0);
	}

	//Read the title.
	auto result = ImageOcrHelper::DecodeTitle(ocrTitle, systemMethods);

	//Store the confidence
	storeConfidence(numberOfTries, result.first, result.second);
	m_confidence = result.second;

	return result;
}

bool CardReader::extractOcrReadyTitle(const Mat cardImage, Mat& outImage) {

	//Extract the title part.
	cropImageToTitleSection(cardImage, outImage);

	//Prepare the tile for OCR reading.
	TitleExtractor titleExtractor(imageFileName, outImage, systemMethods, doDebugging);
	bool success = titleExtractor.ExtractTitle(outImage);

	//See if we need to stop.
	if (!success) {
		return false;
	}

	//Make white text on black background.
	ImageHelper::SetBackgroundByInverting(outImage, true);

	//Store result for debugging.
	if (doDebugging) {
		SaveOcvImage::SaveImageData(systemMethods, outImage, imageFileName, L"10 - OCR Prepared Title");
	}

	return true;
}

void CardReader::storeConfidence(int numberOfTries, wstring ocrResult, int ocrConfidence) {

	if (doDebugging) {

		wstring textfileName = L"TitleDecodeConfidence.txt";
		wstring subfolderName = L"Image Data";
		wstring numberOfTriesStr = systemMethods->ToWString(to_string(numberOfTries));
		wstring confidenceStr = systemMethods->ToWString(to_string(ocrConfidence));

		wstring rowToAdd = numberOfTriesStr + L"\t" + imageFileName + L"\t" + ocrResult + L"\t" + confidenceStr;
		FileHandling::AddRowToFile(systemMethods, rowToAdd, textfileName, subfolderName);
	}
}

bool CardReader::isConfidentOfTitleDecode(wstring title, int confidence) {

	//The Tesseract algorithm needs to be confident with the result otherwise we shouldn't either.
	if (confidence < 75) { return false; }

	//The shortest card name is three letters.
	if (title.length() < (size_t)MtgCardInfoHelper::LettersInShortestCardName()) { return false; }

	//Check for illegal characters.
	if (containsInvalidCharacters(title)) { return false; }

	return true;
}

bool CardReader::containsInvalidCharacters(wstring title) {

	//Make the title lowercase to make it easier to work with.
	string titleStr = systemMethods->ToString(title);
	boost::algorithm::to_lower(titleStr);

	vector<char> allowedCharacters
	{
		' ', '-', '\'', ',', 'æ',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};

	//Check for illegal characters.
	for (char letter : titleStr) {
		if (!AlgorithmHelper::VectorContains(allowedCharacters, letter)) {
			return true;
		}
	}

	return false;
}

void CardReader::cropImageToTitleSection(const Mat rawCardImage, Mat& outImage) {

	Rect titleBox = MtgCardInfoHelper::GetNormalTitleSectionBox(rawCardImage.size());
	ImageHelper::CropImage(rawCardImage, outImage, titleBox);

	//Store result for debugging.
	if (doDebugging) {
		SaveOcvImage::SaveImageData(systemMethods, outImage, imageFileName, L"6 - Title Section");
	}
}
