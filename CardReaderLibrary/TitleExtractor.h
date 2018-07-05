#pragma once
#include "ImageHandlerBase.h"
#include "FigureArea.h"
#include "TrendLine.h"
#include "CardTitleType.h"
//Class for extracting the title section from a card.
class TitleExtractor :
	public ImageHandlerBase
{
public:
	TitleExtractor(Session* session, std::wstring imageFilePath, cv::Mat originalImageData);
	~TitleExtractor();

	//Extracts an image containing the card title.
	bool ExtractTitle(std::vector<cv::Mat>& outImages, int binaryThreshold, int& numberOfTries, CardTitleType titleType);
	//Tells if the original title image has a black background.
	bool HasOriginalTitleBlackBackground();

private:

	//Creates a black'n'white copy of the image.
	cv::Mat getBinaryImage(const cv::Mat titleImage, int binaryThreshold, CardTitleType titleType);
	//Makes sure the amount of gauss is valid to use when adding Gaussian blur.
	int errorProtectGaussAmount(int amountOfGauss);
	//Extracts the title text area out of an image containing the entire title area.
	bool getTitleText(const cv::Mat titleImage, std::vector<cv::Mat>& textImages, int& numberOfTries);
	//Gets the rectangle bounding the text area.
	cv::RotatedRect getTextArea(Contour letters, TrendLine centerLine, TrendLine baseLine, cv::Mat titleImage, int numberOfTries);
	//Cleans the text images from noise and clutter.
	void cleanOcrImages(std::vector<cv::Mat>& outImages);

	//Tells if the original title image has a black background.
	bool hasOriginalTitleBlackBackground;
	//Tells if the mehtod extracting the title has been run.
	bool hasRunTitleExtraction = false;
};
