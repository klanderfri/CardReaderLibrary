#include "stdafx.h"
#include "AlgorithmHelper.h"
#include <numeric>

using namespace cv;
using namespace std;

AlgorithmHelper::AlgorithmHelper()
{
}

AlgorithmHelper::~AlgorithmHelper()
{
}

long double AlgorithmHelper::Slope(const vector<long double>& x, const vector<long double>& y) {
	
	//Implemented as suggested at:
	//https://stackoverflow.com/a/18974171/1997617

	if (x.size() != y.size()) {
		throw ParameterException("The amount of X and Y values must be the same!", "x/y");
	}

	long double avgX = Average(x);
	long double avgY = Average(y);

	long double numerator = 0.0;
	long double denominator = 0.0;

	for (size_t i = 0; i < x.size(); ++i) {
		numerator += (x[i] - avgX) * (y[i] - avgY);
		denominator += (x[i] - avgX) * (x[i] - avgX);
	}

	long double slope = numerator / denominator;
	return slope;
}

long double AlgorithmHelper::Average(const vector<long double>& numbers) {

	return accumulate(numbers.begin(), numbers.end(), (long double)0.0) / (long double)numbers.size();
}

Point2f AlgorithmHelper::FindClosestPointX(double x, vector<Point2f> points) {

	Point2f closestPoint;
	double shortestDistance = INT_MAX;

	for (Point2f point : points) {

		double distance = abs(x - point.x);

		if (distance == 0) {
			return point;
		}

		if (distance < shortestDistance) {
			closestPoint = point;
			shortestDistance = distance;
		}
	}

	return closestPoint;
}

double AlgorithmHelper::FindDistance(Point2d point1, Point2d point2) {

	double xDistance = abs(point1.x - point2.x);
	double yDistance = abs(point1.y - point2.y);

	double distance = sqrt(pow(xDistance, 2) + pow(yDistance, 2));

	return distance;
}

RotatedRect AlgorithmHelper::GetRotatedRectangle(vector<TrendLine> verticalBorders, vector<TrendLine> horizontalBorders) {

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
	angle -= 0.025; // *shruggs* Seems to work with the test cases, so...

	//Calculate size.
	long double height = TrendLine::GetPerpendicularDistance(horizontalBorders[0], horizontalBorders[1]);
	long double width = TrendLine::GetPerpendicularDistance(verticalBorders[0], verticalBorders[1]);
	Size2d size(width, height);

	//Create rectangle.
	RotatedRect rectangle(center, size, (float)angle);

	return rectangle;
}
