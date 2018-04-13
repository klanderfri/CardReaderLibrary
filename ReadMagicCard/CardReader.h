#pragma once
#include "BasicReaderData.h"
#include "CardTitleType.h"
#include "OcrDecodeResult.h"
//Class for reading a card.
class CardReader :
	BasicReaderData
{
public:
	CardReader(std::wstring imageFileName, SystemMethods* systemMethods, bool runDebugging);
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

	//Extracts the part of the image containing the card.
	cv::Mat extractCardImage(cv::Mat originalImage);
	//Reads the title of the card.
	std::wstring readTitle(cv::Mat cardImage);
	//Reads the title of the card.
	OcrDecodeResult readTitle(cv::Mat cardImage, CardTitleType titleType);
	//Joins the titles of two halves of a split card into one title.
	OcrDecodeResult joinSplitCardTitles(OcrDecodeResult resultA, OcrDecodeResult resultB);
	//Store the confidence of the OCR read.
	void storeOcrConfidence(OcrDecodeResult result);
	//Splits a card into the two split card halves.
	std::vector<cv::Mat> getSplitCardHalves(const cv::Mat& originalCardImage, CardTitleType titleType);
	//Tells if a result is to be considered a failure.
	bool hasResultFailed(OcrDecodeResult result);
	//Reads the OCR preparied titles and returnes the best result.
	OcrDecodeResult ocrReadTitle(std::vector<cv::Mat> ocrTitles);
	//Extracts and preprocesses the title.
	bool extractOcrReadyTitle(const cv::Mat cardImage, std::vector<cv::Mat>& outImages, CardTitleType type);
	//Crops out the card title.
	void cropImageToTitleSection(const cv::Mat rawCardImage, cv::Mat& outImages, CardTitleType type);

	//Tells if the extraction was successfull.
	bool m_success = false;
	//Tells how confident Tesseract was of decoding the title.
	int m_confidence = 0;
	//The number of title images of the card the debug algorithm has stored to disk.
	int numberOfOcrTitlesStoredForDebug;
	//The number of times the algorithm has tried to read the card.
	int numberOfCardReadTries;
};
