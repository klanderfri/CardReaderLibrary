#include "stdafx.h"
#include "OcvImageHelper.h"
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

const int OcvImageHelper::DEBUG_BORDER_THICKNESS = 1;
const int OcvImageHelper::DEBUG_BORDER_LINE_TYPE = 8;

OcvImageHelper::OcvImageHelper()
{
}

OcvImageHelper::~OcvImageHelper()
{
}

Rect2f OcvImageHelper::ToStraightRectangle(const RotatedRect rawRectangle, bool enforcePortraitMode) {

	//Get the size of the rectangle.
	float width = rawRectangle.size.width;
	float height = rawRectangle.size.height;

	//Change to portait.
	if (enforcePortraitMode && width > height) {
		swap(width, height);
	}

	//Caclulate the coordinates.
	float x = rawRectangle.center.x - (width / 2);
	float y = rawRectangle.center.y - (height / 2);

	return Rect2f(x, y, width, height);
}

Mat OcvImageHelper::ToGreyImage(const Mat imageToConvert) {

	return changeImageMode(imageToConvert, COLOR_BGR2GRAY);
}

Mat OcvImageHelper::ToColourImage(const Mat imageToConvert) {

	return changeImageMode(imageToConvert, COLOR_GRAY2BGR);
}

Mat OcvImageHelper::changeImageMode(const Mat imageToConvert, ColorConversionCodes conversionCode) {

	Mat convertedImage(imageToConvert.size(), CV_8UC3);
	int amountOfChannels = imageToConvert.channels();

	if (conversionCode == COLOR_GRAY2BGR && amountOfChannels == 3 ||
		conversionCode == COLOR_BGR2GRAY && amountOfChannels == 1) {

		//The image already has the correct mode.
		imageToConvert.copyTo(convertedImage);
	}
	else {

		//Convert the colour mode.
		cvtColor(imageToConvert, convertedImage, conversionCode);
	}

	return convertedImage;
}

int OcvImageHelper::ShowImage(const Mat imageToShow) {

	//Create a window for display.
	namedWindow("Display Window", WINDOW_AUTOSIZE);

	//Show our image inside it.
	imshow("Display Window", imageToShow);

	return 0;
}

void OcvImageHelper::RotateImage(const Mat rawImage, Mat& outImage, const double angleToRotate, const Point2f centerPoint)
{
	//Implemented as suggested at:
	//http://opencvexamples.blogspot.com/2014/01/rotate-image.html

	Mat r = getRotationMatrix2D(centerPoint, angleToRotate, 1.0);

	//Seems like INTER_LANCZOS4 causes the least blur.
	//See https://stackoverflow.com/questions/24953935/opencv-image-getting-blurred-when-rotating-with-warpaffine
	warpAffine(rawImage, outImage, r, Size(rawImage.cols, rawImage.rows), INTER_LANCZOS4);
}

Mat OcvImageHelper::DrawLimits(const Mat image, const RotatedRect rotatedLimitRectangle, const Rect straightLimitRectangle, const vector<Point> limitContour)
{
	//Create a working RGB image so the border lines are colour even if the image isn't.
	Mat workingImage = ToColourImage(image);

	//Create colours to indicate the border lines' types.
	Scalar colourContour = Scalar(0, 255, 255); //Yellow
	Scalar colourStraightRectangle = Scalar(0, 120, 0); //Green
	Scalar colourRotatedRectangle = Scalar(180, 130, 70); //Blue
														  //Scalar(BLUE, GREEN, RED)

														  //Draw the contours.
	if (!limitContour.empty()) {
		drawContours(workingImage, vector<vector<Point>>{ limitContour }, 0, colourContour, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE, vector<Vec4i>(), 0, Point());
	}
	//Draw the straight rectangle.
	if (!straightLimitRectangle.empty()) {
		rectangle(workingImage, straightLimitRectangle.tl(), straightLimitRectangle.br(), colourStraightRectangle, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE, 0);
	}
	//Draw the rotated rectangle
	Point2f rect_points[4];
	rotatedLimitRectangle.points(rect_points);
	for (int j = 0; j < 4; j++) {
		line(workingImage, rect_points[j], rect_points[(j + 1) % 4], colourRotatedRectangle, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE);
	}

	return workingImage;
}

Mat OcvImageHelper::DrawLimits(const Mat image, const vector<vector<Point>> contours, vector<Vec4i> hierarchy, bool useRandomColours) {

	RNG rng(time(0));
	Mat drawing = ToColourImage(image);

	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar colour;
		if (useRandomColours) {
			colour = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		}
		else {
			colour = Scalar(0, 0, 200);
		}

		drawContours(drawing, contours, i, colour, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE, hierarchy, 0, Point());
	}

	return drawing;
}

Mat OcvImageHelper::DrawCenterPoint(const Mat image, const Point imageCenter, Scalar colour, int radius) {

	//Create a working rgb image so the border lines are colour even if the image isn't.
	Mat workingImage = ToColourImage(image);

	circle(workingImage, imageCenter, radius, colour, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE);

	return workingImage;
}

double OcvImageHelper::GetAnglesToStrightenUp(const RotatedRect rotatedRectangle, bool enforcePortraitMode) {

	if (enforcePortraitMode) {

		return rotatedRectangle.angle < -45 ? rotatedRectangle.angle + 90 : rotatedRectangle.angle;
	}
	else {

		double rotateAlternative1 = rotatedRectangle.angle; //Should always be negative.
		double rotateAlternative2 = rotatedRectangle.angle + 90; //Should allways be positive.
		double smallestRotation = abs(rotateAlternative1) < rotateAlternative2 ? rotateAlternative1 : rotateAlternative2;

		return smallestRotation;
	}
}

Point2f OcvImageHelper::GetImageCenter(const Mat image) {

	return Point2f(image.cols / (float)2, image.rows / (float)2);
}

void OcvImageHelper::StraightenUpImage(const Mat rawImage, Mat& outImage, const RotatedRect rawCardArea, Rect2f& outCardArea, bool enforcePortraitMode) {

	//Rotate the image to straighten up the card.
	double angleToRotate = GetAnglesToStrightenUp(rawCardArea, enforcePortraitMode);
	RotateImage(rawImage, outImage, angleToRotate, rawCardArea.center);

	//Rotate the card area rectangle.
	outCardArea = ToStraightRectangle(rawCardArea, enforcePortraitMode);
}

void OcvImageHelper::CropImage(const Mat rawImage, Mat& outImage, const Rect cropArea) {

	float centerX = (float)cropArea.x + cropArea.size().width / 2;
	float centerY = (float)cropArea.y + cropArea.size().height / 2;
	Point2f center(centerX, centerY);

	getRectSubPix(rawImage, cropArea.size(), center, outImage);
}

void OcvImageHelper::CropImageWithSolidBorder(const Mat rawImage, Mat& outImage, const Rect cropArea, int borderThickness) {

	//Crop the image.
	CropImage(rawImage, outImage, cropArea);

	//Add the border.
	copyMakeBorder(
		outImage, outImage,
		borderThickness, borderThickness, borderThickness, borderThickness,
		BORDER_ISOLATED, Scalar(255));
}

void OcvImageHelper::ResizeImage(const Mat rawImage, Mat& outImage, int height) {

	float ratio = (float)rawImage.size().height / rawImage.size().width;
	int width = (int)round(height / ratio);
	Size newSize(width, height);

	resize(rawImage, outImage, newSize);
}

void OcvImageHelper::SetBackgroundByInverting(Mat& image, bool setblackBackground) {

	int white = countNonZero(image);
	int black = image.size().area() - white;

	if (setblackBackground && white > black ||
		!setblackBackground && white < black) {
		image = ~image;
	}
}
