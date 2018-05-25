#pragma once
#include "TrendLine.h"
#include "Hierarchy.h"
#include "LetterArea.h"
#include "ConversionHelper.h"
//Class for drawing on images.
class DrawingHelper
{
public:
	DrawingHelper();
	~DrawingHelper();

	//The thickness of borders drawn on images for debugging.
	static const int DEFAULT_BORDER_THICKNESS;
	//The line type (8-connected line, aka "solid") of borders drawn on images for debugging.
	static const int DEFAULT_BORDER_LINE_TYPE;

	//Draws a line on an image between two points.
	cv::Mat DrawLine(const cv::Mat image, cv::Point point1, cv::Point point2, Colour colour = Purple, int borderThickness = DEFAULT_BORDER_THICKNESS);
	//Draws a line on an image.
	cv::Mat DrawLine(const cv::Mat image, TrendLine line, Colour colour = Purple, int borderThickness = DEFAULT_BORDER_THICKNESS);
	//Draws the specified limits to the image.
	cv::Mat DrawLimits(const cv::Mat image, const cv::RotatedRect rotatedLimitRectangle, const cv::Rect straightLimitRectangle = cv::Rect(), const Contour limitContour = Contour());
	//Draws the specified limits to the image.
	cv::Mat DrawLimits(const cv::Mat image, const Contours contours, const Hierarchy hierarchy, bool useRandomColours = true);
	//Creates an image showing the limits of the individual letters.
	cv::Mat DrawLimits(const cv::Mat image, const LetterAreas letters, int letterCenterRadius);
	//Draws the specified center point to the image.
	cv::Mat DrawCenterPoint(const cv::Mat image, const cv::Point imageCenter, const Colour colour = Red, int radius = 3);
	//Fills a contour in the image with the specified colour.
	void FillContour(cv::Mat& image, const Contour contour, const Colour colour);

private:

	//Pointer to object doing conversions.
	ConversionHelper* converter;
};

