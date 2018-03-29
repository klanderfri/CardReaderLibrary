#include "stdafx.h"
#include "OcvLoadImage.h"
#include "OcvFileHandling.h"
#include <opencv2\imgcodecs.hpp>

using namespace std;
using namespace cv;

OcvLoadImage::OcvLoadImage()
{
}

OcvLoadImage::~OcvLoadImage()
{
}

Mat OcvLoadImage::LoadImageData(OcvSystemDependencyClass* systemMethods, std::wstring imageFilePath) {

	//Read the file.
	wstring fullImageFilePath = OcvFileHandling::GetMtgImageFileFolderPath(systemMethods) + imageFilePath;
	string pathParameter = systemMethods->ToString(fullImageFilePath);
	Mat image = imread(pathParameter, IMREAD_COLOR);

	//Check for invalid input.
	if (image.empty())
	{
		throw OcvLoadException("Could not load the image: " + systemMethods->ToString(fullImageFilePath));
	}

	return image;
}
