#include "stdafx.h"
#include "BasicReaderData.h"

using namespace cv;
using namespace std;

BasicReaderData::BasicReaderData(Session* session, wstring imageFilePath, Mat originalImageData)
	: SessionBound(session)
{
	this->imageFilePath = imageFilePath;
	this->originalImageData = originalImageData;
	this->imageFileName = session->systemMethods->GetFileNameFromFilePath(imageFilePath);
}

BasicReaderData::~BasicReaderData()
{
}
