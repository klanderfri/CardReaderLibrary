#include "stdafx.h"
#include "TrendLine.h"

using namespace cv;
using namespace std;

TrendLine::TrendLine()
{
}

TrendLine::TrendLine(double slope, double offset)
{
	this->Slope = slope;
	this->Offset = offset;
}

TrendLine::~TrendLine()
{
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
