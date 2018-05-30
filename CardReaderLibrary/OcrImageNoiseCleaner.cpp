#include "stdafx.h"
#include "OcrImageNoiseCleaner.h"

using namespace cv;
using namespace std;

OcrImageNoiseCleaner::OcrImageNoiseCleaner(Session* session, wstring imageFilePath, Mat originalImageData)
	: ImageHandlerBase(session, imageFilePath, originalImageData)
{
}

OcrImageNoiseCleaner::~OcrImageNoiseCleaner()
{
}

void OcrImageNoiseCleaner::CleanImage(Mat& dirtyImage) {

	Mat originalImage, binaryImage;
	dirtyImage.copyTo(originalImage);

	threshold(dirtyImage, binaryImage, 240, 255, THRESH_BINARY);
	Contours contours = session->imageHelper->contourMethods->GetCannyContours(binaryImage, 120);
	LetterAreas figures(contours);

	for (size_t i = 0; i < figures.size(); i++) {

		handleFigure(figures[i], dirtyImage);
	}
}

void OcrImageNoiseCleaner::handleFigure(LetterArea figure, Mat& dirtyImage) {

	if (figure.Box.size.area() == 0) { return; }

	Mat contourImage = session->imageHelper->drawingMethods->DrawContours(dirtyImage, { figure.TightContour });

	if (isNoise(figure, dirtyImage.size())) {

		session->imageHelper->drawingMethods->FillContour(dirtyImage, figure.OuterContour, White);
	}
}

bool OcrImageNoiseCleaner::isNoise(LetterArea figure, Size imageArea) {

	if (isOblong(figure)) { return true; }
	if (isSmallEnoughToBeNoise(figure) &&
		!isInCenter(figure, imageArea)) { return true; }

	return false;
}

bool OcrImageNoiseCleaner::isOblong(LetterArea figure) {

	double height = min(figure.Box.size.height, figure.Box.size.width);
	double width = max(figure.Box.size.height, figure.Box.size.width);
	double relation = width / height;

	return (relation > 5.5);
}

bool OcrImageNoiseCleaner::isSmallEnoughToBeNoise(LetterArea figure) {

	bool isOpen = !isContourConvex(figure.TightContour);
	double area = contourArea(isOpen ? figure.OuterContour : figure.TightContour);
	int maxNoiseArea = (int)(session->WORKING_CARD_HEIGHT / 8.5); //80

	return (area <= maxNoiseArea);
}

bool OcrImageNoiseCleaner::isInCenter(LetterArea figure, Size imageArea) {

	int borderMargin = (int)(session->WORKING_CARD_HEIGHT / 27.2); //25
	Rect rCenter(borderMargin, borderMargin, imageArea.width - 2 * borderMargin, imageArea.height - 2 * borderMargin);
	RotatedRect rrCenter = session->imageHelper->converter->ToRotatedRectangle(rCenter);
	bool placedInCenter = session->imageHelper->rectangleMethods->DoesRectangleContainPoint(rrCenter, figure.Box.center);

	return placedInCenter;
}
