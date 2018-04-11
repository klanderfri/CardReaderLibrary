#include "stdafx.h"
#include "BasicReaderData.h"

using namespace cv;
using namespace std;

BasicReaderData::BasicReaderData(wstring imageFileName, Mat originalImageData, SystemMethods* systemMethods, bool runDebugging)
	: WORKING_CARD_HEIGHT(680)
{
	this->imageFileName = imageFileName;
	this->originalImageData = originalImageData;
	this->systemMethods = systemMethods;
	this->runDebugging = runDebugging;
}

BasicReaderData::~BasicReaderData()
{
}
