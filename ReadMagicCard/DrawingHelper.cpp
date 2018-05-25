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

Mat DrawingHelper::DrawLimits(const Mat image, const RotatedRect rotatedLimitRectangle, const Rect straightLimitRectangle, const Contour limitContour)
{
	//Create a working RGB image so the border lines are colour even if the image isn't.
	Mat workingImage = converter->ToColourImage(image);

	Scalar yellow = converter->ToScalarColour(Yellow);
	Scalar green = converter->ToScalarColour(Green);
	Scalar darkBlue = converter->ToScalarColour(DarkBlue);

	//Draw the contours.
	if (!limitContour.empty()) {
		drawContours(workingImage, Contours{ limitContour }, 0, yellow, DEFAULT_BORDER_THICKNESS, DEFAULT_BORDER_LINE_TYPE, Hierarchy(), 0, Point());
	}
	//Draw the straight rectangle.
	if (!straightLimitRectangle.empty()) {
		rectangle(workingImage, straightLimitRectangle.tl(), straightLimitRectangle.br(), green, DEFAULT_BORDER_THICKNESS, DEFAULT_BORDER_LINE_TYPE, 0);
	}
	//Draw the rotated rectangle
	Point2f rect_points[4];
	rotatedLimitRectangle.points(rect_points);
	for (int j = 0; j < 4; j++) {
		line(workingImage, rect_points[j], rect_points[(j + 1) % 4], darkBlue, DEFAULT_BORDER_THICKNESS, DEFAULT_BORDER_LINE_TYPE);
	}

	return workingImage;
}

Mat DrawingHelper::DrawLimits(const Mat image, const Contours contours, bool useRandomColours, Hierarchy hierarchy) {

	RNG rng(time(0));
	Mat drawing = converter->ToColourImage(image);

	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar colour = useRandomColours ?
			Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)) :
			converter->ToScalarColour(Red);

		drawContours(drawing, contours, i, colour, DEFAULT_BORDER_THICKNESS, DEFAULT_BORDER_LINE_TYPE, hierarchy, 0, Point());
	}

	return drawing;
}

Mat DrawingHelper::DrawLimits(const Mat image, const LetterAreas letters, int letterCenterRadius) {

	Mat drawing = converter->ToColourImage(image);

	for (size_t i = 0; i < letters.size(); i++) {

		LetterArea area = letters[i];

		//Draw the area.
		drawing = drawing.empty() ? image : drawing;
		drawing = DrawLimits(drawing, area.Box, Rect(), area.OuterContour);

		//Draw the center.
		drawing = DrawCenterPoint(drawing, area.Box.center, DarkBlue, letterCenterRadius);
	}

	return drawing;
}

Mat DrawingHelper::DrawCenterPoint(const Mat image, const Point imageCenter, const Colour colour, int radius) {

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
