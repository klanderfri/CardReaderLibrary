#pragma once
#include "BasicReaderData.h"
#include "CardTitleType.h"
//Class for reading a card.
class CardReader :
	BasicReaderData
{
public:
	CardReader(std::wstring imageFileName, SystemMethods* systemMethods, bool doDebugging);
	~CardReader();

	//Get the name of the file containing the image.
	std::wstring GetImageFileName();
	//Extracts the card name from the image.
	std::wstring ExtractCardName();
	//Get the success of the extraction.
	bool GetSuccess();
	//Get the confidence of the Tesseract title read.
	int GetConfidence();

private:

	//Crops out the card title.
	void cropImageToTitleSection(const cv::Mat rawCardImage, cv::Mat& outImage, CardTitleType type);
	//Reads the title of the card.
	std::pair<std::wstring, int> readTitle(cv::Mat cardImage, int numberOfTries);
	//Extracts and preprocesses the title.
	bool extractOcrReadyTitle(const cv::Mat cardImage, cv::Mat& outImage, CardTitleType type);
	//Store the confidence of the OCR read.
	void storeConfidence(int numberOfTries, std::wstring ocrResult, int ocrConfidence);
	//Checks if we got a confident decode of the title.
	bool isConfidentOfTitleDecode(std::wstring title, int confidence);
	//Checks if there are any illeagal characters in the title.
	bool containsInvalidCharacters(std::wstring title);

	//Tells if the extraction was successfull.
	bool m_success = false;
	//Tells how confident Tesseract was of decoding the title.
	int m_confidence = 0;
};
