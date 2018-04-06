#pragma once
#include "SystemMethods.h"
//Class for loading images from disk.
class LoadOcvImage
{
public:
	LoadOcvImage();
	~LoadOcvImage();

	//Loads the image data from the specified image file.
	static cv::Mat LoadImageData(SystemMethods* systemMethods, std::wstring imageFilePath);
};
