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
