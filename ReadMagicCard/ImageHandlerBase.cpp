#include "stdafx.h"
#include "ImageHandlerBase.h"

using namespace cv;
using namespace std;

ImageHandlerBase::ImageHandlerBase(Session* session, wstring imageFilePath)
	: SessionBound(session)
{
	if (imageFilePath.empty()) {
		throw ParameterException("The provided image file name was empty!", "imageFilePath");
	}

	this->imageFilePath = imageFilePath;
	this->imageFileName = session->systemMethods->GetFileNameFromFilePath(imageFilePath);
}

ImageHandlerBase::ImageHandlerBase(Session* session, wstring imageFilePath, Mat originalImageData)
	: ImageHandlerBase(session, imageFilePath)
{
	this->originalImageData = originalImageData;
}

ImageHandlerBase::~ImageHandlerBase()
{
}

bool ImageHandlerBase::HasOriginalImageData() {

	return !originalImageData.empty();
}
