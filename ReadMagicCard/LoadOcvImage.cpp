#include "stdafx.h"
#include "LoadOcvImage.h"
#include "FileHandling.h"
#include <opencv2\imgcodecs.hpp>

using namespace std;
using namespace cv;

LoadOcvImage::LoadOcvImage()
{
}

LoadOcvImage::~LoadOcvImage()
{
}

Mat LoadOcvImage::LoadImageData(Session* session, wstring imageFilePath) {

	//Read the file.
	wstring fullImageFilePath = FileHandling::GetMtgImageFileFolderPath(session) + imageFilePath;
	string pathParameter = session->systemMethods->ToString(fullImageFilePath);
	Mat image = imread(pathParameter, IMREAD_COLOR);

	//Check for invalid input.
	if (image.empty())
	{
		throw LoadException("Could not load the image: " + session->systemMethods->ToString(fullImageFilePath));
	}

	return image;
}
