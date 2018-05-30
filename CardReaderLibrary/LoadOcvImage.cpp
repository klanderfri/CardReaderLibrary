#include "stdafx.h"
#include "LoadOcvImage.h"
#include <opencv2\imgcodecs.hpp>

using namespace std;
using namespace cv;

LoadOcvImage::LoadOcvImage(SystemMethods* systemMethods, CardReadInput* inputData)
	: FileSystem(systemMethods, inputData)
{
}

LoadOcvImage::~LoadOcvImage()
{
}

Mat LoadOcvImage::LoadImageData(wstring imageFilePath) {

	//Read the file.
	string pathParameter = systemMethods->ToString(imageFilePath);
	Mat image = imread(pathParameter, IMREAD_COLOR);

	//Check for invalid input.
	if (image.empty())
	{
		throw LoadException("Could not load the image: " + systemMethods->ToString(imageFilePath));
	}

	return image;
}
