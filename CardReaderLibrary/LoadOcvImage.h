#pragma once
#include "FileSystem.h"
//Class for loading images from disk.
class LoadOcvImage :
	public FileSystem
{
public:
	LoadOcvImage(SystemMethods* systemMethods, CardReadInput* inputData);
	~LoadOcvImage();

	//Loads the image data from the specified image file.
	cv::Mat LoadImageData(std::wstring imageFilePath);
};
