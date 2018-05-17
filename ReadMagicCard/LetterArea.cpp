#include "stdafx.h"
#include "LetterArea.h"
#include "ImageHelper.h"

using namespace cv;
using namespace std;

LetterArea::LetterArea()
{
}

LetterArea::~LetterArea()
{
}

bool LetterArea::CompareLetterAreaByLeftBorderXAscending(LetterArea area1, LetterArea area2) {

	float borderX1 = area1.Box.center.x - ImageHelper::SmallestDistanceCenterToLimit(area1.Box);
	float borderX2 = area2.Box.center.x - ImageHelper::SmallestDistanceCenterToLimit(area2.Box);

	return (borderX1 < borderX2);
}

bool LetterArea::CompareLetterAreaByHeightAscending(LetterArea area1, LetterArea area2) {

	return (area1.Box.size.height < area2.Box.size.height);
}

bool LetterArea::HasParentLetter(LetterArea letterToCheck, LetterAreas possibleParents) {

	for (size_t i = 0; i < possibleParents.size(); i++) {

		if (hasChildParentRelation(letterToCheck, possibleParents[i])) { return true; }
	}

	return false;
}

bool LetterArea::hasChildParentRelation(LetterArea child, LetterArea parent) {

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
