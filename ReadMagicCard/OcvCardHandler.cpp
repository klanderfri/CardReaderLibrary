#include "stdafx.h"
#include "OcvCardHandler.h"

using namespace cv;
using namespace std;

OcvCardHandler::OcvCardHandler(wstring imageFileName, Mat originalImageData, OcvSystemDependencyClass* systemMethods, bool doDebugging)
	: WORKING_CARD_HEIGHT(680)
{
	this->imageFileName = imageFileName;
	this->originalImageData = originalImageData;
	this->systemMethods = systemMethods;
	this->doDebugging = doDebugging;
}

OcvCardHandler::~OcvCardHandler()
{
}
