#pragma once
#include "FileHandling.h"
//Class for loading images from disk.
class LoadOcvImage
	: public FileHandling
{
public:
	LoadOcvImage();
	~LoadOcvImage();

	//Loads the image data from the specified image file.
	static cv::Mat LoadImageData(Session* session, std::wstring imageFilePath);
};
