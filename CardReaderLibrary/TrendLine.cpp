#include "stdafx.h"
#include "TrendLine.h"
#include "AlgorithmHelper.h"
#include "SystemMethods.h"
#include "math.h"

using namespace cv;
using namespace std;

TrendLine TrendLine::XAxis = TrendLine(0, 0);

TrendLine::TrendLine()
{
}

TrendLine::TrendLine(long double slope, long double offset)
{
	setupMemberVariables(slope, offset);
}

TrendLine::TrendLine(const vector<Point2d>& points) {

	if (points.empty()) {

		setupMemberVariables(0, 0);
	}
	else if (points.size() == 1) {

		Point2d origo(0, 0);
		vector<Point2d> newVector{ origo, points[0] };
		TrendLine line(newVector);

		setupMemberVariables(line.Slope, line.Offset);
	}
	else {

		vector<long double> x, y;

		for (Point2d point : points) {

			x.push_back(point.x);
			y.push_back(point.y);
		}

		long double slope = AlgorithmHelper::Slope(x, y);
		Point2d point(AlgorithmHelper::Average(x), AlgorithmHelper::Average(y));
		long double offset = point.y - (slope * point.x);

		setupMemberVariables(slope, offset);
	}
}

TrendLine::~TrendLine()
{
}

void TrendLine::setupMemberVariables(long double slope, long double offset) {

	this->Slope = slope;
	this->Offset = offset;
}

vector<Point2d> TrendLine::GetEndPoints(long double leftLimitX, long double rightLimitX) {

	Point2d linePoint1(leftLimitX, Offset + Slope * leftLimitX);
	Point2d linePoint2(rightLimitX, Offset + Slope * rightLimitX);

	vector<Point2d> linePoints;
	linePoints.push_back(linePoint1);
	linePoints.push_back(linePoint2);

	return linePoints;
}

long double TrendLine::GetY(long double x) {

	return Slope * x + Offset;
}

long double TrendLine::GetX(long double y) {

	return (y - Offset) / Slope;
}

bool TrendLine::IsZeroLine() {

	return (Offset == XAxis.Offset && Slope == XAxis.Slope);
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

long double TrendLine::GetPerpendicularDistance(TrendLine lineA, TrendLine lineB) {

	//Implemented according to:
	//https://en.wikipedia.org/w/index.php?title=Distance_between_two_straight_lines&oldid=816643023

	if (lineA.Slope != lineB.Slope) {
		throw OperationException("The lines are not parallel and therefore the perpendicular distance cannot be calculated.");
	}

	long double numerator = abs(lineB.Offset - lineA.Offset);
	long double denominator = sqrt(pow(lineA.Slope, 2) + 1);
	long double distance = numerator / denominator;

	return distance;
}

long double TrendLine::GetPerpendicularDistance(Point2d point) {

	//Check if the line is parallel with the X axis.
	//It's a special case that could be calculated quicker,
	//but the regular algorithm works for these cases as well.
	if (Slope == 0) {

		return abs(Offset - point.y);
	}

	//Find distance using intersection;
	TrendLine perpendicularLine = GetPerpendicularLine(point);
	Point2d intersection = GetIntersectionPoint(*this, perpendicularLine);
	long double controlDistance = AlgorithmHelper::FindDistance(point, intersection);

	//Implemented according to:
	//https://www.slideshare.net/nsimmons/11-x1-t05-05-perpendicular-distance
	//Also check "Line defined by an equation" at Wikipedia:
	//https://en.wikipedia.org/w/index.php?title=Distance_from_a_point_to_a_line&oldid=837108058#Line_defined_by_an_equation

	long double a = (1) * Slope;
	long double b = (-1);
	long double c = (1) * Offset;

	long double numerator = a * point.x + b * point.y + c;
	long double denominator = sqrt(pow(a, 2) + pow(b, 2)); //Will never be negative or zero.

	long double distance = numerator / denominator;

	//The two methods should render the same result but considering the result being doubles
	//after calculations with doubles, they most likely have small differences.
	
	//The first method can't determine which side the point is relative the line, hense absolute value method.
	long double difference = abs(controlDistance - abs(distance));

	//Although, too much of a difference indicates error in the algorithms.
	assert(difference < 0.1);

	//Use the average value.
	long double sign = (distance < 0) ? -1 : 1;
	long double value = AlgorithmHelper::Average(vector<long double> { controlDistance, abs(distance) });
	distance = sign * value;

	return distance;
}

long double TrendLine::GetVerticalDistance(Point2d point) {

	long double lineY = GetY(point.x);
	long double verticalDistance = point.y - lineY;

	return verticalDistance;
}

TrendLine TrendLine::GetPerpendicularLine(Point2d pointOnPerpendicularLine) {

	//Implemented according to section "Perpendicular Lines" at
	//http://www.purplemath.com/modules/strtlneq3.htm
	//https://www.mathsisfun.com/algebra/line-parallel-perpendicular.html

	if (Slope == 0) {
		throw OperationException("Perpendicular lines cannot be calculated when the slope is zero!");
	}

	//Calculate perpendicular line.
	long double slope = (-1 / Slope);
	long double offset = slope * (-1) * pointOnPerpendicularLine.x + pointOnPerpendicularLine.y;
	TrendLine perpendicularLine(slope, offset);

	//Verify that the line is perpendicular.
	long double lineAngles = TrendLine::GetAngleBetweenLines(*this, perpendicularLine);
	//The perpendicular line is not perpendicular!
	assert(abs(lineAngles) == 90);

	return perpendicularLine;
}

TrendLine TrendLine::GetParallelLine(Point2d pointOnParallelLine) {

	//Calculate parallel line.
	long double xOnThisLine = pointOnParallelLine.x;
	long double yOnThisLine = GetY(xOnThisLine);
	long double extraOffset = pointOnParallelLine.y - yOnThisLine;
	TrendLine parallel(Slope, Offset + extraOffset);

	//Verify that the line is parallel.
	long double yOnParallelLine = parallel.GetY(pointOnParallelLine.x);
	long double difference = abs(yOnParallelLine - pointOnParallelLine.y);
	//The methods calculating the parallel line got an inaccurate result!
	assert(difference < FLT_EPSILON);
	//The methods calculating the parallel line got a line that is not parallel!
	assert(this->Slope == parallel.Slope);

	return parallel;
}

Point2d TrendLine::GetIntersectionPoint(TrendLine lineA, TrendLine lineB) {

	//Implemented according to:
	//https://en.wikipedia.org/w/index.php?title=Line%E2%80%93line_intersection&oldid=815048738#Given_the_equations_of_the_lines

	if (lineA.Slope == lineB.Slope) {
		throw OperationException("The slopes are identical, meaning the lines are parallel and has no intersection point!");
	}

	long double x = (lineB.Offset - lineA.Offset) / (lineA.Slope - lineB.Slope); //Will never be division by zero due to the exception above.
	long double y = lineA.GetY(x);

	Point2d intersection(x, y);

	return intersection;
}

long double TrendLine::GetAngleBetweenLines(TrendLine normLine, TrendLine relationLine) {

	//Handle infinity.
	long double inf = numeric_limits<double>::infinity();
	if (abs(normLine.Slope) == inf && abs(relationLine.Slope) == inf) {
		return 0;
	}
	else if (abs(normLine.Slope) == inf || abs(relationLine.Slope) == inf) {

		TrendLine nonInfiniteLine = (abs(normLine.Slope) != inf) ? normLine : relationLine;
		long double correctedAngle = GetAngleBetweenLines(TrendLine::XAxis, nonInfiniteLine) - 90;
		correctedAngle = (correctedAngle < -90) ? correctedAngle + 180 : correctedAngle;
		return correctedAngle;
	}

	//Implemented according to:
	//https://www.youtube.com/watch?v=JSEPDJfl8m8

	long double numerator = normLine.Slope - relationLine.Slope;
	long double denominator = 1 + normLine.Slope * relationLine.Slope;
	if (denominator == 0) { return -90; }
	long double value = numerator / denominator;
	long double angle = degreesFromRadians(atan(value));

	return angle;
}

long double TrendLine::degreesFromRadians(long double radians) {

	return radians * 180 / CV_PI;
}

long double TrendLine::GetAngleToAxisX() {

	long double angle = GetAngleBetweenLines(XAxis, *this);
	return angle;
}

vector<TrendLine> TrendLine::GetBoundLines(vector<Point2d> points) {

	if (points.size() == 0) {
		throw ParameterException("There was no points to bound!", "points");
	}

	vector<TrendLine> bounds;

	if (points.size() == 1) {
		
		TrendLine line = GetParallelLine(points[0]);
		bounds.push_back(line);
		bounds.push_back(line);
	}
	else {

		Point2d lowestPoint = points[0], highestPoint = points[0];
		long double lowestPointDistance = GetPerpendicularDistance(lowestPoint);
		long double highestPointDistance = lowestPointDistance;

		for (size_t i = 1; i < points.size(); i++) {

			Point2d point = points[i];
			long double distance = GetPerpendicularDistance(point);

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

		bounds.push_back(lowestBound);
		bounds.push_back(highestBound);
	}

	//The bounding lines must be parallel!
	assert(this->Slope == bounds[0].Slope);
	assert(bounds[0].Slope == bounds[1].Slope);

	return bounds;
}

bool TrendLine::IsParallel(TrendLine lineA, TrendLine lineB) {

	return (lineA.Slope == lineB.Slope);
}
