#include "stdafx.h"
#include "TrendLine.h"
#include "AlgorithmHelper.h"
#include "boost/lexical_cast.hpp"

using namespace cv;
using namespace std;
using boost::lexical_cast;

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

wstring TrendLine::GetEquation() {

	wstring offset = convertDoubleToWstring(abs(Offset), 3);
	wstring slope = convertDoubleToWstring(Slope, 3);
	wstring sign = (Offset < 0) ? L"- " : L"+ ";
	wstring equation = L"y = " + slope + L"x " + sign + offset;

	return equation;
}

wstring TrendLine::GetZeroEquation() {

	wstring offset = convertDoubleToWstring(abs(Offset), 3);
	wstring slope = convertDoubleToWstring(abs(Slope), 3);
	wstring offsetSign = (Offset > 0) ? L"- " : L"+ ";
	wstring slopeSign = (Slope > 0) ? L"- " : L"+ ";
	wstring equation = L"y " + slopeSign + slope + L"x " + offsetSign + offset + L" = 0";

	return equation;
}

double TrendLine::GetPerpendicularDistance(Point point) {

	//Check if the line is parallel with the X axis.
	if (Slope == 0) {
		
		return abs(Offset - point.y);
	}

	//Check if the point is origo
	if (point.x == 0 && point.y == 0) {

		TrendLine perpendicularLine = GetPerpendicularLine(point);
		Point2d intersection = TrendLine::GetIntersectionPoint(*this, perpendicularLine);
		double distance = AlgorithmHelper::FindDistance(point, intersection);

		return distance;
	}

	//Implemented according to:
	//https://www.slideshare.net/nsimmons/11-x1-t05-05-perpendicular-distance

	double a = (-1) * Slope * point.x;
	double b = (1) * point.y;
	double c = (-1) * Offset;

	double numerator = abs(a + b + c);
	double denominator = sqrt(pow(a, 2) + pow(b, 2)); //Will never be negative or zero.

	double distance = numerator / denominator;

	return distance;
}

TrendLine TrendLine::GetPerpendicularLine(Point pointOnPerpendicularLine) {

	//Implemented according to section "Perpendicular Lines" at
	//https://www.mathsisfun.com/algebra/line-parallel-perpendicular.html

	if (Slope == 0) {
		throw OperationException("Perpendicular lines can't be calculated when the slope is zero!");
	}

	double slope = abs(1 / Slope);
	double offset = slope * (-1) * pointOnPerpendicularLine.x + pointOnPerpendicularLine.y;

	TrendLine perpendicularLine(slope, offset);

	return perpendicularLine;
}

Point2d TrendLine::GetIntersectionPoint(TrendLine lineA, TrendLine lineB) {

	//Implemented according to:
	//https://en.wikipedia.org/w/index.php?title=Line%E2%80%93line_intersection&oldid=815048738#Given_the_equations_of_the_lines

	if (lineA.Slope == lineB.Slope) {
		throw OperationException("The slopes are identical, meaning the lines are parallel and has no intersection point!");
	}

	double x = (lineB.Offset - lineA.Offset) / (lineA.Slope - lineB.Slope); //Will never be dicision by zero due to exception above.
	double y = lineA.GetY(x);

	Point2d intersection(x, y);

	return intersection;
}

wstring TrendLine::convertDoubleToWstring(double number, int decimalPrecision) {

	//Rounding implemented as suggested at:
	//https://stackoverflow.com/a/1344261/1997617
	
	double roundFactor = pow(10, decimalPrecision);
	double nearest = round(number * roundFactor) / roundFactor;
	
	wstring numberStr = boost::lexical_cast<wstring>(nearest);
	numberStr = numberStr.substr(0, numberStr.find(L'.') + decimalPrecision + 1);
	return numberStr;
}
