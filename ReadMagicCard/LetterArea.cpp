#include "stdafx.h"
#include "LetterArea.h"

LetterArea::LetterArea()
{
}

LetterArea::~LetterArea()
{
}

bool LetterArea::CompareLetterAreaByLeftBorderXAscending(LetterArea area1, LetterArea area2)
{
	float borderX1 = area1.Box.center.x - ImageHelper::SmallestDistanceCenterToLimit(area1.Box);
	float borderX2 = area2.Box.center.x - ImageHelper::SmallestDistanceCenterToLimit(area2.Box);

	return (borderX1 < borderX2);
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