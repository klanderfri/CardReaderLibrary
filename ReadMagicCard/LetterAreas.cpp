#include "stdafx.h"
#include "LetterAreas.h"
#include "ContourHelper.h"

using namespace cv;
using namespace std;

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
	: LetterAreasBase(first, last)
{
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