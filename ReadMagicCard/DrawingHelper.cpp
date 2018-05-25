#include "stdafx.h"
#include "DrawingHelper.h"
#include <time.h>
#include <opencv2\imgproc.hpp>

using namespace cv;
using namespace std;

const int DrawingHelper::DEFAULT_BORDER_THICKNESS = 1;
const int DrawingHelper::DEFAULT_BORDER_LINE_TYPE = 8;

DrawingHelper::DrawingHelper()
{
	this->converter = new ConversionHelper();
}

DrawingHelper::~DrawingHelper()
{
	delete converter;
}

Mat DrawingHelper::DrawLine(const Mat image, Point point1, Point point2, Colour colour, int borderThickness) {

	//Create a working RGB image so the border lines are colour even if the image isn't.
	Mat outImage = converter->ToColourImage(image);
	Scalar sColour = converter->ToScalarColour(colour);

	line(outImage, point1, point2, sColour, borderThickness, DEFAULT_BORDER_THICKNESS);

	return outImage;
}

Mat DrawingHelper::DrawLine(const Mat image, TrendLine line, Colour colour, int borderThickness) {

	vector<Point2d> points = line.GetEndPoints(0, image.cols);
	Mat outImage = DrawLine(image, points[0], points[1], colour, borderThickness);

	return outImage;
}

Mat DrawingHelper::DrawLimits(const Mat image, const Contour contourLimit, const RotatedRect rotatedRectangleLimit, const Rect straightRectangleLimit)
{
	//Create a working RGB image so the border lines are colour even if the image isn't.
	Mat workingImage = converter->ToColourImage(image);

	//Draw the contours.
	if (!contourLimit.empty()) {
		workingImage = DrawContours(workingImage, Contours{ contourLimit });
	}

	//Draw the straight rectangle.
	if (!straightRectangleLimit.empty()) {
		workingImage = DrawRectangle(workingImage, straightRectangleLimit);
	}

	//Draw the rotated rectangle
	if (rotatedRectangleLimit.size.area() > 0) {
		workingImage = DrawRectangle(workingImage, rotatedRectangleLimit);
	}

	return workingImage;
}

Mat DrawingHelper::DrawRectangle(const Mat image, const RotatedRect rectangle, Colour colour) {

	Mat workingImage = converter->ToColourImage(image);

	Point2f rect_points[4];
	rectangle.points(rect_points);

	for (int i = 0; i < 4; i++) {
		
		int j = (i + 1) % 4;
		workingImage = DrawLine(workingImage, rect_points[i], rect_points[j], colour, DEFAULT_BORDER_THICKNESS);
	}

	return workingImage;
}

Mat DrawingHelper::DrawRectangle(const Mat image, const Rect rectangle, Colour colour) {

	Mat workingImage = converter->ToColourImage(image);

	Scalar sColour = converter->ToScalarColour(colour);
	cv::rectangle(workingImage, rectangle.tl(), rectangle.br(), sColour, DEFAULT_BORDER_THICKNESS, DEFAULT_BORDER_LINE_TYPE, 0);

	return workingImage;
}

Mat DrawingHelper::DrawContours(const Mat image, const Contours contours, bool useRandomColours, Hierarchy hierarchy) {

	Mat workingImage = converter->ToColourImage(image);
	RNG rng(time(0));

	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar colour = useRandomColours ?
			Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)) :
			converter->ToScalarColour(Red);

		drawContours(workingImage, contours, i, colour, DEFAULT_BORDER_THICKNESS, DEFAULT_BORDER_LINE_TYPE, hierarchy, 0, Point());
	}

	return workingImage;
}

Mat DrawingHelper::DrawLetterAreas(const Mat image, const LetterAreas letters, int letterCenterRadius) {

	Mat workingImage = converter->ToColourImage(image);

	for (size_t i = 0; i < letters.size(); i++) {

		LetterArea area = letters[i];

		//Draw the area.
		workingImage = workingImage.empty() ? image : workingImage;
		workingImage = DrawLimits(workingImage, area.OuterContour, area.Box);

		//Draw the center.
		workingImage = DrawCircle(workingImage, area.Box.center, letterCenterRadius, Green);
	}

	return workingImage;
}

Mat DrawingHelper::DrawCircle(const Mat image, const Point imageCenter, int radius, const Colour colour) {

	//Create a working rgb image so the border lines are colour even if the image isn't.
	Mat workingImage = converter->ToColourImage(image);
	Scalar sColour = converter->ToScalarColour(colour);

	circle(workingImage, imageCenter, radius, sColour, DEFAULT_BORDER_THICKNESS, DEFAULT_BORDER_LINE_TYPE);

	return workingImage;
}

void DrawingHelper::FillContour(Mat& image, const Contour contour, const Colour colour) {

	Scalar sColour = converter->ToScalarColour(colour);
	fillConvexPoly(image, contour, sColour);
}
