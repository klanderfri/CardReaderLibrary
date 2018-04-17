#pragma once
#include "BasicReaderData.h"
#include "CardTitleType.h"
#include "CardNameInfo.h"
#include "ReadingConfiguration.h"
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
	void ReadCardName();
	//Get the result of the card reading.
	CardNameInfo GetResult();

private:

	//The level of confidence to be considered a successful result.
	int NORMAL_OCR_CONFIDENCE_THRESH;
	//The level of confidence to be considered so good a result that no future processing is needed.
	int HIGH_OCR_CONFIDENCE_THRESH;

	//Extracts the part of the image containing the card.
	cv::Mat extractCardImage(cv::Mat originalImage);
	//Reads the title of the card.
	CardNameInfo readTitle1(cv::Mat cardImage, bool searchForAkhSplitHalf, std::vector<ReadingConfiguration> configs, CardTitleType cardType);
	//Reads the title of the card.
	CardNameInfo readTitle2(cv::Mat cardImage, ReadingConfiguration config, CardTitleType cardType);
	//Reads the title of the card.
	CardNameInfo readTitle3(const cv::Mat cardImage, const CardTitleType titleType, const int binaryThreshold);
	//Joins the titles of two halves of a split card into one title.
	CardNameInfo joinSplitCardTitles(CardNameInfo resultA, CardNameInfo resultB);
	//Store the confidence of the OCR read.
	void storeOcrConfidence(CardNameInfo result, int numberOfTries);
	//Splits a card into the two split card halves.
	std::vector<cv::Mat> getSplitCardHalves(const cv::Mat& originalCardImage, CardTitleType titleType);
	//Tells if a result is to be considered a failure.
	bool hasResultFailed(CardNameInfo result);
	//Reads the OCR preparied titles and returnes the best result.
	CardNameInfo ocrReadTitle(std::vector<cv::Mat> ocrTitles);
	//Extracts and preprocesses the title.
	bool extractOcrReadyTitle(const cv::Mat cardImage, std::vector<cv::Mat>& outImages, const CardTitleType type, const int binaryThreshold);
	//Crops out the card title.
	void cropImageToTitleSection(const cv::Mat rawCardImage, cv::Mat& outImages, CardTitleType type);

	//The final result of the card reading.
	CardNameInfo finalResult;
	//The number of OCR-prepared title images of the card the debug algorithm has stored to disk.
	int numberOfOcrTitlesStoredForDebug;
	//The number of title images of the card the debug algorithm has stored to disk.
	int numberOfTitleImagesStoredForDebug;
	//The number of times the algorithm has tried to read the card.
	int numberOfCardReadTries;
};
