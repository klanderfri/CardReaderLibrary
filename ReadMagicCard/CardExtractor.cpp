#include "stdafx.h"
#include "CardExtractor.h"
#include "FileHandling.h"
#include "LoadOcvImage.h"
#include "SaveOcvImage.h"
#include "StoreCardProcessingData.h"

using namespace cv;
using namespace std;

CardExtractor::CardExtractor(Session* session, wstring imageFilePath, Mat originalImageData) :
	ImageHandlerBase(session, imageFilePath, originalImageData),
	WORKING_IMAGE_HEIGHT((int)(session->WORKING_CARD_HEIGHT * 1.4))
{
}

CardExtractor::~CardExtractor()
{
}

Mat CardExtractor::ExtractCard(Session* session, wstring imageFilePath) {

	//Load the image.
	Mat originalCardImage = LoadOcvImage::LoadImageData(session, imageFilePath);

	//Extract the card part.
	Mat cardImage;
	CardExtractor cardExtractor(session, imageFilePath, originalCardImage);
	bool success = cardExtractor.ExtractCard(cardImage);

	//See if we need to stop.
	if (!success) {
		throw OperationException("ERROR: Could not extract the card section!");
	}

	return cardImage;
}

bool CardExtractor::ExtractCard(Mat& outImage) {

	//Resize to make the images less resource consuming.
	session->imageMethods->ResizeImage(originalImageData, outImage, WORKING_IMAGE_HEIGHT);

	//Create grey scale copy
	outImage = session->imageMethods->ToGreyImage(outImage);

	//Find the card in the image.
	RotatedRect cardArea = getOriginalCardArea(outImage, originalImageData.size());

	//Store result for debugging.
	if (session->runDebugging) {
		Mat debugImage = session->imageMethods->DrawLimits(originalImageData, cardArea);
		SaveOcvImage::SaveImageData(session, debugImage, imageFileName, L"3 - HD with Limit");
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

	if (session->runDebugging) {
		StoreCardProcessingData storer = StoreCardProcessingData(session);
		storer.StoreSideRelations(imageFilePath, sideFactor);
	}

	//The sides relative size indicate if we got the card or just parts of it.
	//That in turn might indicate that the card has white or silver borders.
	if (sideFactor < 1.36 || sideFactor > 1.44) {
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
	session->imageMethods->StraightenUpImage(rawImage, workingImage, rawCardArea, straightCardArea);

	//Store result for debugging.
	if (session->runDebugging) {
		Mat debugImage = session->imageMethods->DrawCenterPoint(workingImage, rawCardArea.center);
		debugImage = session->imageMethods->DrawLimits(debugImage, RotatedRect(), straightCardArea, Contour());
		SaveOcvImage::SaveImageData(session, debugImage, imageFileName, L"4 - Rotated Cards");
	}

	//Crop out the card.
	session->imageMethods->CropImage(workingImage, workingImage, straightCardArea);

	//Store result for debugging.
	if (session->runDebugging) {
		SaveOcvImage::SaveImageData(session, workingImage, imageFileName, L"5 - Extracted Cards");
	}

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
	if (session->runDebugging) {
		SaveOcvImage::SaveImageData(session, workingImage, imageFileName, L"1 - Black and White");
	}

	//Find the countours of the card.
	Contours contours = session->imageMethods->GetCannyContours(workingImage, thresh);

	//Find the outer card contour.
	Contour maxContour = findMaxContour(contours, workingImage);

	//Get the smallest rectangle containing the card.
	RotatedRect rotatedCardRectangle = minAreaRect(Mat(maxContour));

	//Store result for debugging.
	if (session->runDebugging) {
		Rect straightCardRectangle = getContourRectangle(maxContour); //Get the smallest straight rectangle containing the card.
		Mat debugImage = session->imageMethods->DrawLimits(rawImage, rotatedCardRectangle, straightCardRectangle, maxContour);
		SaveOcvImage::SaveImageData(session, debugImage, imageFileName, L"2 - Cards with Limits");
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

		maxContour = session->imageMethods->ContoursConvexHull(contours);
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
