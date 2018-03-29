#pragma once
#include "OcvCardHandler.h"
#include "Contour.h"
//Class for extracting a section of an image or a card.
class OcvSectionExtractor :
	protected OcvCardHandler
{
public:
	OcvSectionExtractor(std::wstring imageFileName, cv::Mat imageData, OcvSystemDependencyClass* systemMethods, bool debuggingOn);
	~OcvSectionExtractor();

protected:

	//Finds the convex hull of multiple contours.
	Contour ContoursConvexHull(const Contours contours);
};
