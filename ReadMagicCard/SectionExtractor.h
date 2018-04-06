#pragma once
#include "BasicReaderData.h"
#include "Contour.h"
//Class for extracting a section of an image or a card.
class SectionExtractor :
	protected BasicReaderData
{
public:
	SectionExtractor(std::wstring imageFileName, cv::Mat imageData, SystemMethods* systemMethods, bool doDebugging);
	~SectionExtractor();

protected:

	//Finds the convex hull of multiple contours.
	Contour ContoursConvexHull(const Contours contours);
};
