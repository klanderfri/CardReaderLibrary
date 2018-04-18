#include "stdafx.h"
#include "CardExtractor.h"
#include "FileHandling.h"
#include "SaveOcvImage.h"
#include "StoreCardProcessingData.h"

using namespace cv;
using namespace std;

CardExtractor::CardExtractor(wstring imageFileName, Mat originalImageData, SystemMethods* systemMethods, bool runDebugging) :
	SectionExtractor(imageFileName, originalImageData, systemMethods, runDebugging),
	WORKING_IMAGE_HEIGHT((int)(WORKING_CARD_HEIGHT * 1.4))
{
}

CardExtractor::~CardExtractor()
{
}

bool CardExtractor::ExtractCard(Mat& outImage) {

	//Resize to make the images less resource consuming.
	ImageHelper::ResizeImage(originalImageData, outImage, WORKING_IMAGE_HEIGHT);

	//Create grey scale copy
	outImage = ImageHelper::ToGreyImage(outImage);

	//Find the card in the image.
	RotatedRect cardArea = getOriginalCardArea(outImage, originalImageData.size());

	//Store result for debugging.
	if (runDebugging) {
		Mat debugImage = ImageHelper::DrawLimits(originalImageData, cardArea);
		SaveOcvImage::SaveImageData(systemMethods, debugImage, imageFileName, L"3 - HD with Limit");
	}

	//Extract the card.
	outImage = getCardPart(originalImageData, cardArea);

	return true;
}

RotatedRect CardExtractor::getOriginalCardArea(const Mat thumbImage, const Size originalImageSize) {

	//Find the area that contains the card in the image.
	RotatedRect workingCardSquare = findCardSquare(thumbImage, 80);

	float longSide = max(workingCardSquare.size.height, workingCardSquare.size.width);
	float shortSide = min(workingCardSquare.size.height, workingCardSquare.size.width);
	float sideFactor = longSide / shortSide;

	if (runDebugging) {
		StoreCardProcessingData storer = StoreCardProcessingData(systemMethods);
		storer.StoreSideRelations(imageFileName, sideFactor);
	}

	//The sides relative size indicate if we got the card or just parts of it.
	//That in turn might indicate that the card has white or silver borders.
	if (sideFactor < 1.36 || sideFactor>1.44) {
		workingCardSquare = findCardSquare(thumbImage, 170);
	}

	float xFactor = originalImageSize.width / (float)thumbImage.size().width;
	float yFactor = originalImageSize.height / (float)thumbImage.size().height;

	Point2f center(workingCardSquare.center.x * xFactor, workingCardSquare.center.y * yFactor);
	float width = workingCardSquare.size.width * xFactor;
	float height = workingCardSquare.size.height * yFactor;
	float angle = workingCardSquare.angle;
	RotatedRect originalCardArea(center, Size2f(width, height), angle);

	return originalCardArea;
}

Mat CardExtractor::getCardPart(const Mat rawImage, const RotatedRect rawCardArea) {

	//Rotate the image to make it straight.
	Mat workingImage;
	Rect2f straightCardArea;
	ImageHelper::StraightenUpImage(rawImage, workingImage, rawCardArea, straightCardArea);

	//Store result for debugging.
	if (runDebugging) {
		Mat debugImage = ImageHelper::DrawCenterPoint(workingImage, rawCardArea.center);
		debugImage = ImageHelper::DrawLimits(debugImage, RotatedRect(), straightCardArea, Contour());
		SaveOcvImage::SaveImageData(systemMethods, debugImage, imageFileName, L"4 - Rotated Cards");
	}

	//Crop out the card.
	ImageHelper::CropImage(workingImage, workingImage, straightCardArea);

	//Store the extracted card so the user can use it (for example, to showing before selling it).
	SaveOcvImage::SaveImageData(systemMethods, workingImage, imageFileName, L"Image Data\\Extracted Cards");

	return workingImage;
}

RotatedRect CardExtractor::findCardSquare(const Mat rawImage, int thresh) {

	//Copy the raw image to a separate variable to keep the original from changes.
	Mat workingImage;
	rawImage.copyTo(workingImage);

	//Blur the image to smoothing it out.
	GaussianBlur(workingImage, workingImage, Size(11, 11), 0, 0);

	//Threshold the image to get the important pixels.
	threshold(workingImage, workingImage, thresh, 255, THRESH_BINARY);

	//Store result for debugging.
	if (runDebugging) {
		SaveOcvImage::SaveImageData(systemMethods, workingImage, imageFileName, L"1 - Black and White");
	}

	//Find the countours of the card.
	Contours contours;
	Hierarchy hierarchy;
	Canny(workingImage, workingImage, thresh, thresh * 2, 3);
	findContours(workingImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//Find the outer card contour.
	Contour maxContour = findMaxContour(contours, workingImage);

	//Get the smallest rectangle containing the card.
	RotatedRect rotatedCardRectangle = minAreaRect(Mat(maxContour));

	//Store result for debugging.
	if (runDebugging) {
		Rect straightCardRectangle = getContourRectangle(maxContour); //Get the smallest straight rectangle containing the card.
		Mat debugImage = ImageHelper::DrawLimits(rawImage, rotatedCardRectangle, straightCardRectangle, maxContour);
		SaveOcvImage::SaveImageData(systemMethods, debugImage, imageFileName, L"2 - Cards with Limits");
	}

	//Return the result.
	return rotatedCardRectangle;
}

Contour CardExtractor::findMaxContour(const Contours contours, const Mat workingImage)
{
	Contour maxContour;
	bool foundMaxContour = false;

	for (auto contour : contours) {

		if (!isValidCardContour(contour, workingImage.size())) { continue; }

		if (maxContour.empty() || contourArea(maxContour) < contourArea(contour)) {
			maxContour = contour;
			foundMaxContour = true;
		}
	}

	//Try convex hull if no outer contour was valid.
	if (!foundMaxContour) {

		maxContour = ImageHelper::ContoursConvexHull(contours);
	}

	return maxContour;
}

bool CardExtractor::isValidCardContour(const Contour contour, const Size imageSize) {

	if (contour.size() == 0) { return false; }

	//The maximum relative size of the card in the image.
	const double MAX_LIMIT = 0.95;
	//The minimum relative size of the card in the image.
	const double MIN_LIMIT = 0.3;

	const double MAX_AREA = imageSize.area() * MAX_LIMIT;
	const double MIN_AREA = imageSize.area() * MIN_LIMIT;
	const double MAX_WIDTH = MAX_LIMIT * imageSize.width;
	const double MAX_HEIGHT = MAX_LIMIT * imageSize.height;
	const double MIN_WIDTH = MIN_LIMIT * imageSize.width;
	const double MIN_HEIGHT = MIN_LIMIT * imageSize.height;

	//Check the area.
	double areaOfContour = contourArea(contour);
	if (areaOfContour > MAX_AREA) { return false; }
	if (areaOfContour < MIN_AREA) { return false; }

	//Check the width and height.
	Rect box = getContourRectangle(contour);
	if (box.width > MAX_WIDTH) { return false; }
	if (box.height > MAX_HEIGHT) { return false; }
	if (box.width < MIN_WIDTH) { return false; }
	if (box.height < MIN_HEIGHT) { return false; }

	return true;
}

Rect CardExtractor::getContourRectangle(const Contour contour) {

	Contour tempContour;
	approxPolyDP(Mat(contour), tempContour, 3, true);
	Rect rectangle = boundingRect(Mat(tempContour));

	return rectangle;
}
