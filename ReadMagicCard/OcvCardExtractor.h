#pragma once
#include "OcvSectionExtractor.h"
//Class for extracting the card from an image.
class OcvCardExtractor :
	public OcvSectionExtractor
{
public:
	OcvCardExtractor(std::wstring imageFileName, cv::Mat originalImageData, OcvSystemDependencyClass* systemMethods, bool doDebugging);
	~OcvCardExtractor();

	//Extracts an image containing the card.
	bool ExtractCard(cv::Mat& outImage);

private:

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_IMAGE_HEIGHT;

	//Finds the area in the original card image that contains the card.
	cv::RotatedRect getOriginalCardArea(const cv::Mat workingImage, const cv::Size originalImageSize);
	//Extracts the card part of the original card image.
	cv::Mat getCardPart(const cv::Mat rawImage, const cv::RotatedRect cardArea);
	//Finds the smallest square containing the card.
	cv::RotatedRect findCardSquare(const cv::Mat rawImage, int thresh);
	//Finds the biggest contour amoung the contours. That contour generally contains the others.
	Contour findMaxContour(const Contours contours, const cv::Mat workingImage);
	//Tells if a contour could contain the card.
	bool isValidCardContour(const Contour contour, const cv::Size imageSize);
	//Gets a rectangle containing the specified contour.
	cv::Rect getContourRectangle(const Contour contour);
};
