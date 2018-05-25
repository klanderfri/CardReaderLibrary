#pragma once
#include "TrendLine.h"
#include "Hierarchy.h"
#include "LetterAreas.h"
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

	//Draws a line between two points on an image.
	cv::Mat DrawLine(const cv::Mat image, cv::Point point1, cv::Point point2, Colour colour = Orange, int borderThickness = DEFAULT_BORDER_THICKNESS);
	//Draws a line on an image.
	cv::Mat DrawLine(const cv::Mat image, TrendLine line, Colour colour = Orange, int borderThickness = DEFAULT_BORDER_THICKNESS);
	//Draws the specified limits on an image.
	cv::Mat DrawLimits(const cv::Mat image, const Contour contourLimit = Contour(), const cv::RotatedRect rotatedRectangleLimit = cv::RotatedRect(), const cv::Rect straightRectangleLimit = cv::Rect());
	//Draws a rectangle on an image.
	cv::Mat DrawRectangle(const cv::Mat image, const cv::RotatedRect rectangle, Colour colour = DarkBlue);
	//Draws a rectangle on an image.
	cv::Mat DrawRectangle(const cv::Mat image, const cv::Rect rectangle, Colour colour = Green);
	//Draws the specified contours on an image.
	cv::Mat DrawContours(const cv::Mat image, const Contours contours, bool useRandomColours = false, const Hierarchy hierarchy = Hierarchy());
	//Draws the specified letter areas on an image.
	cv::Mat DrawLetterAreas(const cv::Mat image, const LetterAreas letters, int letterCenterRadius);
	//Draws a circle on an image.
	cv::Mat DrawCircle(const cv::Mat image, const cv::Point imageCenter, int radius = 3, const Colour colour = Red);
	//Fills a contour in the image with the specified colour.
	void FillContour(cv::Mat& image, const Contour contour, const Colour colour);

private:

	//Pointer to object doing conversions.
	ConversionHelper* converter;
};

