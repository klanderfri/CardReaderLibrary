#include "stdafx.h"
#include "TrendLine.h"
#include "AlgorithmHelper.h"

using namespace cv;
using namespace std;

TrendLine::TrendLine()
{
}

TrendLine::TrendLine(double slope, double offset)
{
	setupMemberVariables(slope, offset);
}

TrendLine::TrendLine(const vector<Point2f>& points) {

	if (points.empty()) {

		setupMemberVariables(0, 0);
	}
	else if (points.size() == 1) {

		Point2f origo(0, 0);
		vector<Point2f> newVector{ origo, points[0] };
		TrendLine line(newVector);

		setupMemberVariables(line.Slope, line.Offset);
	}
	else {

		vector<double> x, y;

		for (Point2f point : points) {

			x.push_back(point.x);
			y.push_back(point.y);
		}

		double slope = AlgorithmHelper::Slope(x, y);
		Point2f point((float)AlgorithmHelper::Average(x), (float)AlgorithmHelper::Average(y));
		double offset = point.y - (slope * point.x);

		setupMemberVariables(slope, offset);
	}
}

TrendLine::~TrendLine()
{
}

void TrendLine::setupMemberVariables(double slope, double offset) {

	this->Slope = slope;
	this->Offset = offset;
}

vector<Point2f> TrendLine::GetEndPoints(float leftLimitX, float rightLimitX) {

	Point2f linePoint1(leftLimitX, (float)(Offset + Slope * leftLimitX));
	Point2f linePoint2(rightLimitX, (float)(Offset + Slope * rightLimitX));

	vector<Point2f> linePoints;
	linePoints.push_back(linePoint1);
	linePoints.push_back(linePoint2);

	return linePoints;
}

double TrendLine::GetY(double x) {

	return Slope * x + Offset;
}

double TrendLine::GetX(double y) {

	return (y - Offset) / Slope;
}

bool TrendLine::IsZeroLine() {

	return (Offset == 0 && Slope == 0);
}
