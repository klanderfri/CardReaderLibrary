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
