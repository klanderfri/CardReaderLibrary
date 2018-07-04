#pragma once
#include "ImageHandlerBase.h"
//Class for extracting the card from an image.
class CardExtractor :
	public ImageHandlerBase
{
public:
	CardExtractor(Session* session, std::wstring imageFilePath, cv::Mat originalImageData);
	~CardExtractor();

	//Extracts the part of the image containing the card.
	static cv::Mat ExtractCard(Session* session, std::wstring imageFilePath);
	//Extracts the part of the image containing the card.
	bool ExtractCard(cv::Mat& outImage);

private:

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_IMAGE_HEIGHT;

	//Finds the area in the card image that contains the card.
	LetterArea getCardArea(const cv::Mat workingImage, const cv::Size originalImageSize);
	//Extracts the cropped image of the card from the original card image.
	cv::Mat getCardImage(const cv::Mat rawImage, const LetterArea cardArea);
	//Finds the corner points of the card (those points will be inside the card border).
	std::vector<cv::Point> getCloseCornerPoints(Contour cardContour, const cv::Point contourCenter);
	//Finds the smallest square containing the card.
	LetterArea findCardArea(const cv::Mat rawImage, int thresh);
	//Finds the biggest contour amoung the contours. That contour generally contains the others.
	Contour findMaxContour(const Contours contours, const cv::Mat workingImage);
	//Tells if a contour could contain the card.
	bool isValidCardContour(const Contour contour, const cv::Size imageSize);
	//Gets a rectangle containing the specified contour.
	cv::Rect getContourRectangle(const Contour contour);
};
