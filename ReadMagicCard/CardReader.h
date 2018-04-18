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
	cv::Mat extractCardImage(const cv::Mat originalCardImage);
	//Creates the reading configurations to use when searching for the title.
	std::vector<ReadingConfiguration> createReadingConfigurations();
	//Reads the title of the card when we don't know if the card is upside-down or not.
	CardNameInfo readUnrotatedCardTitle(const cv::Mat cardImage, const std::vector<ReadingConfiguration> configs, const CardTitleType cardTitleTypeToSearch);
	//Checks if the result is so confident that we can assume we got the title.
	bool isResultGoodEnoughToQuit(const CardNameInfo result);
	//Checks if we have come to a situation when it's time to search for any secondary Amonkhet split card title .
	bool shouldWeExecuteAmonkhetSplitHalfSearch(const CardNameInfo currentBestResult, const size_t currentIterationIndex, const CardTitleType cardTitleTypeToSearch);
	//Reads the secondary title of an Amonkhet split card.
	CardNameInfo readAmonkhetSplitTitle(const cv::Mat cardImageGivingBestResult, const ReadingConfiguration currentConfig);
	//Reads the title of the card when we can assume it's not upside down.
	CardNameInfo readStraightCardTitle(const cv::Mat cardImage, const ReadingConfiguration config, const CardTitleType cardTitleTypeToSearch);
	//Reads the title of a split card.
	CardNameInfo readSplitCardTitle(const cv::Mat cardImage, const ReadingConfiguration config);
	//Joins the titles of two halves of a split card into one title.
	CardNameInfo joinSplitCardTitles(const CardNameInfo resultA, const CardNameInfo resultB);
	//Store the confidence of the OCR read.
	void storeOcrConfidence(const CardNameInfo result, const int numberOfTries);
	//Splits a card into the two split card halves.
	std::vector<cv::Mat> getSplitCardHalves(const cv::Mat cardImage, const CardTitleType titleType);
	//Reads the OCR preparied titles and returnes the best result.
	CardNameInfo ocrReadTitle(const std::vector<cv::Mat> ocrTitles);
	//Extracts and preprocesses the title.
	bool extractOcrReadyTitle(const cv::Mat cardImage, std::vector<cv::Mat>& outImages, const CardTitleType titleType, const int binaryThreshold);
	//Crops out the card title.
	void cropImageToTitleSection(const cv::Mat cardImage, cv::Mat& outImage, const CardTitleType titleType);

	//The final result of the card reading.
	CardNameInfo finalResult;
	//The number of OCR-prepared title images of the card the debug algorithm has stored to disk.
	int numberOfOcrTitlesStoredForDebug;
	//The number of title images of the card the debug algorithm has stored to disk.
	int numberOfTitleImagesStoredForDebug;
	//The number of times the algorithm has tried to read the card.
	int numberOfCardReadTries;
};
