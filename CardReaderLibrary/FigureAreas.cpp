#include "stdafx.h"
#include "FigureAreas.h"
#include "ContourHelper.h"

using namespace cv;
using namespace std;

FigureAreas::FigureAreas()
{
}

FigureAreas::~FigureAreas()
{
}

FigureAreas::FigureAreas(Contours contours) {

	for (size_t i = 0; i < contours.size(); i++) {

		Contour letterContour = ContourHelper::GetConvexHullContours({ contours[i] });
		RotatedRect letterBox = minAreaRect(letterContour);

		FigureArea area;
		area.TightContour = contours[i];
		area.OuterContour = letterContour;
		area.Box = letterBox;

		this->push_back(area);
	}

	sort(this->begin(), this->end(), FigureArea::CompareFigureAreaByLeftBorderXAscending);
}

FigureAreas::FigureAreas(const_iterator first, const_iterator last)
	: FigureAreasBase(first, last)
{
}

bool FigureAreas::HasParentFigure(FigureArea figureToCheck, FigureAreas possibleParents) {

	for (size_t i = 0; i < possibleParents.size(); i++) {

		if (hasChildParentRelation(figureToCheck, possibleParents[i])) { return true; }
	}

	return false;
}

bool FigureAreas::hasChildParentRelation(FigureArea child, FigureArea parent) {

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