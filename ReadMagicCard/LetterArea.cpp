#include "stdafx.h"
#include "LetterArea.h"
#include "ContourHelper.h"
#include "RectangleHelper.h"

using namespace cv;
using namespace std;

LetterArea::LetterArea()
{
}

LetterArea::~LetterArea()
{
}

LetterAreas::LetterAreas()
{
}

LetterAreas::~LetterAreas()
{
}

LetterAreas::LetterAreas(Contours contours) {

	for (size_t i = 0; i < contours.size(); i++) {

		Contour letterContour = ContourHelper::GetConvexHullContours({ contours[i] });
		RotatedRect letterBox = minAreaRect(letterContour);

		LetterArea area;
		area.TightContour = contours[i];
		area.OuterContour = letterContour;
		area.Box = letterBox;

		this->push_back(area);
	}

	sort(this->begin(), this->end(), LetterArea::CompareLetterAreaByLeftBorderXAscending);
}

LetterAreas::LetterAreas(const_iterator first, const_iterator last)
	: vector<LetterArea>(first, last)
{
}

bool LetterArea::CompareLetterAreaByLeftBorderXAscending(LetterArea area1, LetterArea area2) {

	float borderX1 = area1.Box.center.x - RectangleHelper::SmallestDistanceCenterToLimit(area1.Box);
	float borderX2 = area2.Box.center.x - RectangleHelper::SmallestDistanceCenterToLimit(area2.Box);

	return (borderX1 < borderX2);
}

bool LetterArea::CompareLetterAreaByHeightAscending(LetterArea area1, LetterArea area2) {

	return (area1.Box.size.height < area2.Box.size.height);
}

bool LetterAreas::HasParentLetter(LetterArea letterToCheck, LetterAreas possibleParents) {

	for (size_t i = 0; i < possibleParents.size(); i++) {

		if (hasChildParentRelation(letterToCheck, possibleParents[i])) { return true; }
	}

	return false;
}

bool LetterAreas::hasChildParentRelation(LetterArea child, LetterArea parent) {

	//A child will have its' center inside the parent.
	bool parentContainsChildCenter = pointPolygonTest(parent.OuterContour, child.Box.center, false) > 0;
	if (!parentContainsChildCenter) { return false; }

	//The child will have an area less than its' parents' area.
	double parentArea = contourArea(parent.OuterContour);
	double childArea = contourArea(child.OuterContour);
	if (childArea > parentArea) { return false; }

	//Close enough to be a child.
	return true;
}

Point2d LetterArea::GetMiddleBottomPoint() {

	//Implemented according to section "But which one?" at:
	//http://www.mathsisfun.com/algebra/trig-inverse-sin-cos-tan.html

	double height = Box.size.height;
	double angle = Box.angle;
	if (angle < -45) {

		height = Box.size.width;
		angle += 90.0;
	}

	double hypotenuse = height / 2;
	double angleFactor = (-1) * sin(angle * CV_PI / 180);
	double xdistance = angleFactor * hypotenuse;
	double yDistance = sqrt(pow(hypotenuse, 2) - pow(xdistance, 2));

	double x = Box.center.x + xdistance;
	double y = Box.center.y + yDistance;
	Point2d bottomMiddle(x, y);

	return bottomMiddle;
}
