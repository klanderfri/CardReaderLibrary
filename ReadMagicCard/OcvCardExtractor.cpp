#include "stdafx.h"
#include "OcvCardExtractor.h"
#include "OcvFileHandling.h"
#include "OcvSaveImage.h"

using namespace cv;
using namespace std;

OcvCardExtractor::OcvCardExtractor(wstring imageFileName, Mat originalImageData, OcvSystemDependencyClass* systemMethods, bool doDebugging) :
	OcvSectionExtractor(imageFileName, originalImageData, systemMethods, doDebugging),
	WORKING_IMAGE_HEIGHT((int)(WORKING_CARD_HEIGHT * 1.4))
{
}

OcvCardExtractor::~OcvCardExtractor()
{
}

bool OcvCardExtractor::ExtractCard(Mat& outImage) {

	//Resize to make the images less resource consuming.
	OcvImageHelper::ResizeImage(originalImageData, outImage, WORKING_IMAGE_HEIGHT);

	//Create grey scale copy
	outImage = OcvImageHelper::ToGreyImage(outImage);

	//Find the card in the image.
	RotatedRect cardArea = getOriginalCardArea(outImage, originalImageData.size());

	//Store result for debugging.
	if (doDebugging) {
		Mat debugImage = OcvImageHelper::DrawLimits(originalImageData, cardArea);
		OcvSaveImage::SaveImageData(systemMethods, debugImage, imageFileName, L"3 - HD with Limit");
	}

	//Extract the card.
	outImage = getCardPart(originalImageData, cardArea);

	return true;
}

RotatedRect OcvCardExtractor::getOriginalCardArea(const Mat thumbImage, const Size originalImageSize) {

	//Find the area that contains the card in the image.
	RotatedRect workingCardSquare = findCardSquare(thumbImage, 80);

	float longSide = max(workingCardSquare.size.height, workingCardSquare.size.width);
	float shortSide = min(workingCardSquare.size.height, workingCardSquare.size.width);
	float sideFactor = longSide / shortSide;

	if (doDebugging) {
		wstring rowToAdd = imageFileName + L"\t" + systemMethods->ToWString(systemMethods->ToString(sideFactor));
		OcvFileHandling::AddRowToFile(systemMethods, rowToAdd, L"CardSidesRelations.txt", L"Image Data");
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

Mat OcvCardExtractor::getCardPart(const Mat rawImage, const RotatedRect rawCardArea) {

	//Rotate the image to make it straight.
	Mat workingImage;
	Rect2f straightCardArea;
	OcvImageHelper::StraightenUpImage(rawImage, workingImage, rawCardArea, straightCardArea);

	//Store result for debugging.
	if (doDebugging) {
		Mat debugImage = OcvImageHelper::DrawCenterPoint(workingImage, rawCardArea.center);
		debugImage = OcvImageHelper::DrawLimits(debugImage, RotatedRect(), straightCardArea, Contour());
		OcvSaveImage::SaveImageData(systemMethods, debugImage, imageFileName, L"4 - Rotated Cards");
	}

	//Crop out the card.
	OcvImageHelper::CropImage(workingImage, workingImage, straightCardArea);

	//Store result for debugging.
	if (doDebugging) {
		OcvSaveImage::SaveImageData(systemMethods, workingImage, imageFileName, L"5 - Extracted Cards");
	}

	return workingImage;
}

RotatedRect OcvCardExtractor::findCardSquare(const Mat rawImage, int thresh) {

	//Copy the raw image to a separate variable to keep the original from changes.
	Mat workingImage;
	rawImage.copyTo(workingImage);

	//Blur the image to smoothing it out.
	GaussianBlur(workingImage, workingImage, Size(11, 11), 0, 0);

	//Threshold the image to get the important pixels.
	threshold(workingImage, workingImage, thresh, 255, THRESH_BINARY);

	//Store result for debugging.
	if (doDebugging) {
		OcvSaveImage::SaveImageData(systemMethods, workingImage, imageFileName, L"1 - Black and White");
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
	if (doDebugging) {
		Rect straightCardRectangle = getContourRectangle(maxContour); //Get the smallest straight rectangle containing the card.
		Mat debugImage = OcvImageHelper::DrawLimits(rawImage, rotatedCardRectangle, straightCardRectangle, maxContour);
		OcvSaveImage::SaveImageData(systemMethods, debugImage, imageFileName, L"2 - Cards with Limits");
	}

	//Return the result.
	return rotatedCardRectangle;
}

Contour OcvCardExtractor::findMaxContour(const Contours contours, const Mat workingImage)
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

		maxContour = ContoursConvexHull(contours);
	}

	return maxContour;
}

bool OcvCardExtractor::isValidCardContour(const Contour contour, const Size imageSize) {

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

Rect OcvCardExtractor::getContourRectangle(const Contour contour) {

	Contour tempContour;
	approxPolyDP(Mat(contour), tempContour, 3, true);
	Rect rectangle = boundingRect(Mat(tempContour));

	return rectangle;
}
