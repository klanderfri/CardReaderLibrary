#pragma once
#include "TrendLine.h"
//Class for handling rectangles.
class RectangleHelper
{
public:
	RectangleHelper();
	~RectangleHelper();

	//Creates the rectangle bound by the the specified borders.
	cv::RotatedRect GetRotatedRectangle(std::vector<TrendLine> verticalBorders, std::vector<TrendLine> horizontalBorders, double angleAdjustment = 0.0);
	//Gives the smallest distance between the center and the limit of a rectangle.
	static float SmallestDistanceCenterToLimit(cv::RotatedRect rectangle);
	//Checks if a rotated rectangle has been initialized.
	bool IsInitialized(cv::RotatedRect rectangle);
	//Checks if a rectangle contains the specified point.
	bool DoesRectangleContainPoint(cv::RotatedRect rectangle, cv::Point2f point);
	//Gets how many degrees the rectangle needs to be rotated in positive direction (counter-clockwise) to be straight.
	double GetAnglesToStrightenUp(const cv::RotatedRect rotatedRectangle, bool enforcePortraitMode = true);
	//Stretches a rectangle according to the provided factors.
	void StretchRectangle(const cv::RotatedRect rectangleToStretch, cv::RotatedRect& outRectangle, float xFactor, float yFactor);
};

