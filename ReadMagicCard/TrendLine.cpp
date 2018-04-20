#include "stdafx.h"
#include "TrendLine.h"
#include "AlgorithmHelper.h"
#include "SystemMethods.h"

using namespace cv;
using namespace std;

TrendLine::TrendLine()
{
}

TrendLine::TrendLine(double slope, double offset)
{
	setupMemberVariables(slope, offset);
}

TrendLine::TrendLine(const vector<Point>& points) {

	if (points.empty()) {

		setupMemberVariables(0, 0);
	}
	else if (points.size() == 1) {

		Point2d origo(0, 0);
		vector<Point> newVector{ origo, points[0] };
		TrendLine line(newVector);

		setupMemberVariables(line.Slope, line.Offset);
	}
	else {

		vector<double> x, y;

		for (Point2d point : points) {

			x.push_back(point.x);
			y.push_back(point.y);
		}

		double slope = AlgorithmHelper::Slope(x, y);
		Point2d point((float)AlgorithmHelper::Average(x), (float)AlgorithmHelper::Average(y));
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

vector<Point2d> TrendLine::GetEndPoints(float leftLimitX, float rightLimitX) {

	Point2d linePoint1(leftLimitX, (float)(Offset + Slope * leftLimitX));
	Point2d linePoint2(rightLimitX, (float)(Offset + Slope * rightLimitX));

	vector<Point2d> linePoints;
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

	wstring offset = SystemMethods::ToWString(abs(Offset), 3);
	wstring slope = SystemMethods::ToWString(Slope, 3);
	wstring sign = (Offset < 0) ? L"- " : L"+ ";
	wstring equation = L"y = " + slope + L"x " + sign + offset;

	return equation;
}

wstring TrendLine::GetZeroEquation() {

	wstring offset = SystemMethods::ToWString(abs(Offset), 3);
	wstring slope = SystemMethods::ToWString(abs(Slope), 3);
	wstring offsetSign = (Offset > 0) ? L"- " : L"+ ";
	wstring slopeSign = (Slope > 0) ? L"- " : L"+ ";
	wstring equation = L"y " + slopeSign + slope + L"x " + offsetSign + offset + L" = 0";

	return equation;
}

double TrendLine::GetPerpendicularDistance(Point point) {

	//Check if the line is parallel with the X axis.
	//It's a special case that could be calculated quicker,
	//but the regular algorithm works for these cases as well.
	if (Slope == 0) {

		return abs(Offset - point.y);
	}

	//Find distance using intersection;
	TrendLine perpendicularLine = GetPerpendicularLine(point);
	Point2d intersection = GetIntersectionPoint(*this, perpendicularLine);
	double controlDistance = AlgorithmHelper::FindDistance(point, intersection);

	//Implemented according to:
	//https://www.slideshare.net/nsimmons/11-x1-t05-05-perpendicular-distance
	//Also check "Line defined by an equation" at Wikipedia:
	//https://en.wikipedia.org/w/index.php?title=Distance_from_a_point_to_a_line&oldid=837108058#Line_defined_by_an_equation

	double a = (-1) * Slope;
	double b = (1);
	double c = (-1) * Offset;

	double numerator = a * point.x + b * point.y + c;
	double denominator = sqrt(pow(a, 2) + pow(b, 2)); //Will never be negative or zero.

	double distance = numerator / denominator;

	//The two methods should render the same result but considering the result being doubles
	//after valculations with doubles, they most likely have small differences.
	
	//The first method can't determine which side the point is relative the line, hense absolute value method.
	double difference = abs(controlDistance - abs(distance));

	//Although, too much of a difference indicates error in the algorithms.
	if (difference >= 0.01) {
		throw OperationException("The methods calculating the perpendicular distance renders different results!");
	}

	//Use the average value.
	double sign = (distance < 0) ? -1 : 1;
	double value = AlgorithmHelper::Average(vector<double> { controlDistance, abs(distance) });
	distance = sign * value;

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

TrendLine TrendLine::GetParallelLine(Point pointOnParallelLine) {

	double y = GetY(pointOnParallelLine.x);
	double extraOffset = pointOnParallelLine.y - y;

	TrendLine parallel(Slope, Offset + extraOffset);

	return parallel;
}

Point2d TrendLine::GetIntersectionPoint(TrendLine lineA, TrendLine lineB) {

	//Implemented according to:
	//https://en.wikipedia.org/w/index.php?title=Line%E2%80%93line_intersection&oldid=815048738#Given_the_equations_of_the_lines

	if (lineA.Slope == lineB.Slope) {
		throw OperationException("The slopes are identical, meaning the lines are parallel and has no intersection point!");
	}

	double x = (lineB.Offset - lineA.Offset) / (lineA.Slope - lineB.Slope); //Will never be division by zero due to the exception above.
	double y = lineA.GetY(x);

	Point2d intersection(x, y);

	return intersection;
}

vector<TrendLine> TrendLine::GetBoundLines(vector<Point> points) {

	if (points.size() < 3) {
		return vector<TrendLine> { this, this };
	}

	Point lowestPoint = points[0], highestPoint = points[0];
	double lowestPointDistance = GetPerpendicularDistance(lowestPoint);
	double highestPointDistance = lowestPointDistance;

	for (size_t i = 1; i < points.size(); i++) {

		Point point = points[i];
		double distance = GetPerpendicularDistance(point);

		if (distance < lowestPointDistance) {
			lowestPointDistance = distance;
			lowestPoint = point;
		}
		if (distance > highestPointDistance) {
			highestPointDistance = distance;
			highestPoint = point;
		}
	}

	TrendLine lowestBound = GetParallelLine(lowestPoint);
	TrendLine highestBound = GetParallelLine(highestPoint);

	vector<TrendLine> bounds{ lowestBound, highestBound };
	return bounds;
}
