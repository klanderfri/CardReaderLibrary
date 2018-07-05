#include "stdafx.h"
#include "CardExtractor.h"
#include "LoadOcvImage.h"
#include "SaveOcvImage.h"
#include "PointToPoint.h"

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
	Mat originalCardImage = session->fileSystem->imageLoader->LoadImageData(imageFilePath);

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
	session->imageHelper->imageEditor->ResizeImage(originalImageData, outImage, WORKING_IMAGE_HEIGHT);

	//Create grey scale copy
	outImage = session->imageHelper->converter->ToGreyImage(outImage);

	//Find the card in the image.
	FigureArea cardArea = getCardArea(outImage, originalImageData.size());

	//Store result for debugging.
	if (session->inputData->runDebugging) {
		Mat debugImage = session->imageHelper->drawingMethods->DrawRectangle(originalImageData, cardArea.Box);
		session->fileSystem->imageSaver->SaveImageData(debugImage, imageFileName, L"2 - HD with Limit");
	}

	//Extract the card.
	outImage = getCardImage(originalImageData, cardArea);

	return true;
}

FigureArea CardExtractor::getCardArea(const Mat thumbImage, const Size originalImageSize) {

	//Find the area that contains the card in the image.
	FigureArea workingCardArea = findCardArea(thumbImage, 80);

	float longSide = max(workingCardArea.Box.size.height, workingCardArea.Box.size.width);
	float shortSide = min(workingCardArea.Box.size.height, workingCardArea.Box.size.width);
	float sideFactor = longSide / shortSide;

	if (session->inputData->runDebugging) {
		session->fileSystem->dataStorer->StoreSideRelations(imageFilePath, sideFactor);
	}

	//The sides relative size indicate if we got the card or just parts of it.
	//That in turn might indicate that the card has white or silver borders.
	if (sideFactor < 1.36 || sideFactor > 1.44) {
		workingCardArea = findCardArea(thumbImage, 170);
	}

	//Get the factors showing the size difference between working image and HD image.
	float xFactor = originalImageSize.width / (float)thumbImage.size().width;
	float yFactor = originalImageSize.height / (float)thumbImage.size().height;

	//Create an area container for the HD card image.
	FigureArea originalCardArea;
	session->imageHelper->rectangleMethods->StretchRectangle(workingCardArea.Box, originalCardArea.Box, xFactor, yFactor);
	session->imageHelper->contourMethods->StretchContour(workingCardArea.TightContour, originalCardArea.TightContour, xFactor, yFactor);

	return originalCardArea;
}

Mat CardExtractor::getCardImage(const Mat rawImage, const FigureArea cardArea) {

	//Rotate the image to make it straight.
	Mat workingImage;
	Rect2f straightCardArea;
	session->imageHelper->imageEditor->StraightenUpImage(rawImage, workingImage, cardArea.Box, straightCardArea);

	//Store result for debugging.
	if (session->inputData->runDebugging) {

		//Create image showing the rotation.
		Mat rotatedCard = session->imageHelper->drawingMethods->DrawCircle(workingImage, cardArea.Box.center);
		rotatedCard = session->imageHelper->drawingMethods->DrawRectangle(rotatedCard, straightCardArea);
		session->fileSystem->imageSaver->SaveImageData(rotatedCard, imageFileName, L"3a - Rotated Cards");

		//Create image showing the card without transformation.
		Mat originalCrop;
		session->imageHelper->imageEditor->CropImage(workingImage, originalCrop, straightCardArea);
		session->fileSystem->imageSaver->SaveImageData(originalCrop, imageFileName, L"3b - Untransformed Extracted Cards");
	}

	//Get the straight contour of the card.
	Contour straightCardContour;
	session->imageHelper->contourMethods->StraightenUpContour(cardArea.TightContour, straightCardContour, true);

	//Extract the corner points.
	Point contourCenter = minAreaRect(straightCardContour).center;
	vector<Point> corners = getCloseCornerPoints(straightCardContour, contourCenter);

	//Store result for debugging.
	if (session->inputData->runDebugging) {

		//Paint the corner points (for debug).
		Mat cardCornerPointsImage = session->imageHelper->drawingMethods->DrawContours(workingImage, { straightCardContour });
		cardCornerPointsImage = session->imageHelper->drawingMethods->DrawCircle(cardCornerPointsImage, contourCenter);
		for (Point c : corners) {
			cardCornerPointsImage = session->imageHelper->drawingMethods->DrawCircle(cardCornerPointsImage, c);
			cardCornerPointsImage = session->imageHelper->drawingMethods->DrawCircle(cardCornerPointsImage, c, 15);
		}
		session->fileSystem->imageSaver->SaveImageData(cardCornerPointsImage, imageFileName, L"3c - Card Corner Points");
	}

	//Transform the card image part to a straight rectangle. This will also crop out the card.
	vector<Point2d> dCorners = session->imageHelper->converter->ConvertToDoubleFromInt(corners);
	workingImage = session->imageHelper->imageEditor->transformations->TransformToRectangle(workingImage, dCorners, 0.021357, 0.008306);

	//Store result for debugging.
	if (session->inputData->runDebugging) {
		session->fileSystem->imageSaver->SaveImageData(workingImage, imageFileName, L"3d - Extracted Cards");
	}

	return workingImage;
}

vector<Point> CardExtractor::getCloseCornerPoints(Contour cardContour, const Point contourCenter) {
	
	//Relate all side points to the center point.
	vector<PointToPoint> sidePoints;
	for (Point p : cardContour) {
		sidePoints.push_back(PointToPoint(p, contourCenter));
	}

	//Extract the corner points.
	vector<Point> corners;
	const int numberOfCorners = 4;
	for (int i = 0; i < numberOfCorners; i++) {

		//Find the corner point (the most distant point).
		PointToPoint mostDistant = sidePoints[0];
		for (PointToPoint p : sidePoints) {

			if (mostDistant.Distance() < p.Distance()) {
				mostDistant = p;
			}
		}
		corners.push_back(mostDistant.P1());

		//Remove the points close to the corner since they are not corners.
		if (i < numberOfCorners - 1) { //We don't need to remove anything in the last iteration since we are done by then.

			vector<PointToPoint> remainingSidePoints;
			double threshDistance = mostDistant.Distance() * 0.4;
			for (PointToPoint p : sidePoints) {

				PointToPoint pointToCorner(p.P1(), mostDistant.P1());

				if (pointToCorner.Distance() > threshDistance) {
					remainingSidePoints.push_back(p);
				}
			}
			sidePoints = remainingSidePoints;
		}
	}

	return corners;
}

FigureArea CardExtractor::findCardArea(const Mat rawImage, int thresh) {

	//Copy the raw image to a separate variable to keep the original from changes.
	Mat workingImage;
	rawImage.copyTo(workingImage);

	//Blur the image to smoothing it out.
	GaussianBlur(workingImage, workingImage, Size(11, 11), 0, 0);

	//Threshold the image to get the important pixels.
	threshold(workingImage, workingImage, thresh, 255, THRESH_BINARY);

	//Store result for debugging.
	if (session->inputData->runDebugging) {
		session->fileSystem->imageSaver->SaveImageData(workingImage, imageFileName, L"1a - Border Detection");
	}

	//Find the countours of the card.
	Contours contours = session->imageHelper->contourMethods->GetCannyContours(workingImage, thresh);

	//Find the outer card contour.
	Contour maxContour = findMaxContour(contours, workingImage);

	//Get the smallest rectangle containing the card.
	RotatedRect rotatedCardRectangle = minAreaRect(Mat(maxContour));

	//Store result for debugging.
	if (session->inputData->runDebugging) {
		Rect straightCardRectangle = getContourRectangle(maxContour); //Get the smallest straight rectangle containing the card.
		Mat debugImage = session->imageHelper->drawingMethods->DrawLimits(rawImage, maxContour);// , rotatedCardRectangle, straightCardRectangle);
		session->fileSystem->imageSaver->SaveImageData(debugImage, imageFileName, L"1b - Cards with Limits");
	}

	FigureArea cardArea;
	cardArea.Box = rotatedCardRectangle;
	cardArea.TightContour = maxContour;

	//Return the result.
	return cardArea;
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

		maxContour = session->imageHelper->contourMethods->GetConvexHullContours(contours);
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
