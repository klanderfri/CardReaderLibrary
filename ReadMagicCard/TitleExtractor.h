#pragma once
#include "SectionExtractor.h"
#include "LetterArea.h"
#include "TrendLine.h"
#include "CardTitleType.h"
//Class for extracting the title section from a card.
class TitleExtractor :
	public SectionExtractor
{
public:
	TitleExtractor(std::wstring imageFileName, cv::Mat originalImageData, SystemMethods* systemMethods, bool runDebugging);
	~TitleExtractor();

	//Extracts an image containing the card title.
	bool ExtractTitle(std::vector<cv::Mat>& outImages, int binaryThreshold, int& numberOfTries, CardTitleType titleType);

private:

	//Creates a black'n'white copy of the image.
	cv::Mat getBinaryImage(const cv::Mat titleImage, int binaryThreshold, CardTitleType titleType);
	//Makes sure the amount of gauss is valid to use when adding Gaussian blur.
	int errorProtectGaussAmount(int amountOfGauss);
	//Extracts the title text area out of an image containing the entire title area.
	bool getTitleText(const cv::Mat titleImage, std::vector<cv::Mat>& textImages, int& numberOfTries);
	//Gets the rectangle bounding the text area.
	cv::RotatedRect getTextArea(Contour letters, TrendLine centerLine, cv::Mat titleImage, int numberOfTries);
};
