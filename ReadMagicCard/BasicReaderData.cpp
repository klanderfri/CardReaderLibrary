#include "stdafx.h"
#include "BasicReaderData.h"

using namespace cv;
using namespace std;

BasicReaderData::BasicReaderData(Session* session, wstring imageFileName, Mat originalImageData)
	: SessionBound(session)
{
	this->imageFileName = imageFileName;
	this->originalImageData = originalImageData;
}

BasicReaderData::~BasicReaderData()
{
}
