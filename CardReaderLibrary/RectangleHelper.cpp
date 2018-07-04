#include "stdafx.h"
#include "RectangleHelper.h"
#include <opencv2\imgproc.hpp>

using namespace cv;
using namespace std;

RectangleHelper::RectangleHelper()
{
}

RectangleHelper::~RectangleHelper()
{
}

RotatedRect RectangleHelper::GetRotatedRectangle(vector<TrendLine> verticalBorders, vector<TrendLine> horizontalBorders, double angleAdjustment) {

	//Make sure the borders are correct.
	bool isRightSize = verticalBorders.size() == 2 && horizontalBorders.size() == 2;
	if (!isRightSize) {
		throw ParameterException("Wrong number of borders! There should be exactly 2 vertical borders and 2 horizontal borders!");
	}

	bool isParallel = TrendLine::IsParallel(verticalBorders[0], verticalBorders[1]);
	isParallel = TrendLine::IsParallel(horizontalBorders[0], horizontalBorders[1]) && isParallel;
	if (!isParallel) {
		throw ParameterException("The side borders are not paralell!");
	}

	long double angle1 = TrendLine::GetAngleBetweenLines(verticalBorders[0], horizontalBorders[0]);
	long double angle2 = TrendLine::GetAngleBetweenLines(verticalBorders[1], horizontalBorders[1]);
	bool isPerpendicular = abs(angle1) == 90.0 && abs(angle2) == 90;
	if (!isPerpendicular) {
		throw ParameterException("The horizontal and vertical borders are not perpendicular!");
	}

	//Get the corners.
	Point2d corner = TrendLine::GetIntersectionPoint(horizontalBorders[0], verticalBorders[0]);
	Point2d oppositeCorner = TrendLine::GetIntersectionPoint(horizontalBorders[1], verticalBorders[1]);

	//Calculate center.
	Point2d center = 0.5f * (corner + oppositeCorner);

	//Calculate angle.
	long double angle = (-1) * horizontalBorders[0].GetAngleToAxisX();
	angle += angleAdjustment;

	//Calculate size.
	long double height = TrendLine::GetPerpendicularDistance(horizontalBorders[0], horizontalBorders[1]);
	long double width = TrendLine::GetPerpendicularDistance(verticalBorders[0], verticalBorders[1]);
	Size2d size(width, height);

	//Create rectangle.
	RotatedRect rectangle(center, size, (float)angle);

	return rectangle;
}

double RectangleHelper::GetAnglesToStrightenUp(const RotatedRect rotatedRectangle, bool enforcePortraitMode) {

	if (enforcePortraitMode) {

		return (rotatedRectangle.size.height < rotatedRectangle.size.width) ? rotatedRectangle.angle + 90 : rotatedRectangle.angle;
	}
	else {

		double rotateAlternative1 = rotatedRectangle.angle; //Should always be negative.
		double rotateAlternative2 = rotatedRectangle.angle + 90; //Should always be positive.
		double smallestRotation = abs(rotateAlternative1) < rotateAlternative2 ? rotateAlternative1 : rotateAlternative2;

		return smallestRotation;
	}
}

float RectangleHelper::SmallestDistanceCenterToLimit(RotatedRect rectangle) {

	return min(rectangle.size.height, rectangle.size.width) / 2;
}

bool RectangleHelper::IsInitialized(RotatedRect rectangleToCheck) {

	float sum = abs(rectangleToCheck.angle)
		+ abs(rectangleToCheck.center.x)
		+ abs(rectangleToCheck.center.y)
		+ abs(rectangleToCheck.size.area());

	return sum != 0;
}

bool RectangleHelper::DoesRectangleContainPoint(RotatedRect rectangle, Point2f point) {

	//Implemented as suggested at:
	//http://answers.opencv.org/question/30330/check-if-a-point-is-inside-a-rotatedrect/?answer=190773#post-id-190773

	//Get the corner points.
	Point2f corners[4];
	rectangle.points(corners);

	//Convert the point array to a vector.
	//https://stackoverflow.com/a/8777619/1997617
	Point2f* lastItemPointer = (corners + sizeof corners / sizeof corners[0]);
	vector<Point2f> contour(corners, lastItemPointer);

	//Check if the point is within the rectangle.
	double indicator = pointPolygonTest(contour, point, false);
	bool rectangleContainsPoint = (indicator >= 0);

	return rectangleContainsPoint;
}

void RectangleHelper::StretchRectangle(const RotatedRect rectangleToStretch, RotatedRect& outRectangle, float xFactor, float yFactor) {

	Point2f center(rectangleToStretch.center.x * xFactor, rectangleToStretch.center.y * yFactor);
	Size2f size(rectangleToStretch.size.width * xFactor, rectangleToStretch.size.height * yFactor);
	float angle = rectangleToStretch.angle;

	outRectangle = RotatedRect(center, size, angle);
}
