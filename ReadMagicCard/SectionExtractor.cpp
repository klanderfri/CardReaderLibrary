#include "stdafx.h"
#include "SectionExtractor.h"

using namespace cv;
using namespace std;

SectionExtractor::SectionExtractor(wstring imageFileName, Mat originalImageData, SystemMethods* systemMethods, bool runDebugging)
	: BasicReaderData(imageFileName, originalImageData, systemMethods, runDebugging)
{
}

SectionExtractor::~SectionExtractor()
{
}

Contour SectionExtractor::ContoursConvexHull(const Contours contours)
{
	//Implemented as suggested at:
	//http://answers.opencv.org/question/74777/how-to-use-approxpolydp-to-close-contours/

	Contour result;
	Contour pts;

	for (size_t i = 0; i < contours.size(); i++) {

		for (size_t j = 0; j < contours[i].size(); j++) {

			pts.push_back(contours[i][j]);
		}
	}

	convexHull(pts, result);

	return result;
}
