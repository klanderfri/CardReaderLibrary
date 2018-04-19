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

double AlgorithmHelper::Slope(const vector<double>& x, const vector<double>& y) {
	
	//Implemented as suggested at:
	//https://stackoverflow.com/a/18974171/1997617

	if (x.size() != y.size()) {
		throw ParameterException("The amount of X and Y values must be the same!", "x/y");
	}

	double avgX = Average(x);
	double avgY = Average(y);

	double numerator = 0.0;
	double denominator = 0.0;

	for (size_t i = 0; i < x.size(); ++i) {
		numerator += (x[i] - avgX) * (y[i] - avgY);
		denominator += (x[i] - avgX) * (x[i] - avgX);
	}

	double slope = numerator / denominator;
	return slope;
}

double AlgorithmHelper::Average(const vector<double>& numbers) {

	return accumulate(numbers.begin(), numbers.end(), 0.0) / numbers.size();
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
