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

vector<Point2f> AlgorithmHelper::TrendLinePoints(const vector<Point2f>& points) {

	Point2f origo(0, 0);
	if (points.empty()) {
		return vector<Point2f> { origo, origo };
	}
	if (points.size() == 1) {

		vector<Point2f> newVector{ origo, points[0] };
		return TrendLinePoints(newVector);
	}

	Point2f firstPoint = points[0];
	Point2f lastPoint = points[points.size() - 1];

	TrendLine trendLine = TrendLineValues(points);
	vector<Point2f> trendPoints = trendLine.GetEndPoints(firstPoint.x, lastPoint.x);

	return trendPoints;
}

TrendLine AlgorithmHelper::TrendLineValues(const vector<Point2f>& points) {

	if (points.empty()) {
		return TrendLine(0, 0);
	}
	if (points.size() == 1) {

		Point2f origo(0, 0);
		vector<Point2f> newVector{ origo, points[0] };
		return TrendLineValues(newVector);
	}

	vector<double> x, y;

	for (Point2f point : points) {

		x.push_back(point.x);
		y.push_back(point.y);
	}

	

	double slope = Slope(x, y);
	Point2f point((float)Average(x), (float)Average(y));
	double offset = point.y - (slope * point.x);

	TrendLine line(slope, offset);
	return line;
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
