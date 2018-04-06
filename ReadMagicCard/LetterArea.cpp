#include "stdafx.h"
#include "LetterArea.h"

LetterArea::LetterArea()
{
}

LetterArea::~LetterArea()
{
}

bool LetterArea::CompareLetterAreaByBorderXAscending(LetterArea area1, LetterArea area2)
{
	float borderX1 = area1.box.center.x - ImageHelper::SmallestDistanceCenterToLimit(area1.box);
	float borderX2 = area2.box.center.x - ImageHelper::SmallestDistanceCenterToLimit(area2.box);

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
	bool parentContainsChildCenter = pointPolygonTest(parent.contour, child.box.center, false) > 0;
	if (!parentContainsChildCenter) { return false; }

	//The child will have an area less than its' parents' area.
	double parentArea = contourArea(parent.contour);
	double childArea = contourArea(child.contour);
	if (childArea > parentArea) { return false; }

	//Close enough to be a child.
	return true;
}