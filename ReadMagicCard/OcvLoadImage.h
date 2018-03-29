#pragma once
#include "OcvSystemDependencyClass.h"
//Class for loading images from disk.
class OcvLoadImage
{
public:
	OcvLoadImage();
	~OcvLoadImage();

	//Loads the image data from the specified image file.
	static cv::Mat LoadImageData(OcvSystemDependencyClass* systemMethods, std::wstring imageFilePath);
};
