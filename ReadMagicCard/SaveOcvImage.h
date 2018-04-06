#pragma once
#include "SystemMethods.h"
//Class for saving images to disk.
class SaveOcvImage
{
public:
	SaveOcvImage();
	~SaveOcvImage();

	//Saves the image data to the specified image file.
	static void SaveImageData(SystemMethods* systemMethods, cv::Mat imageToSave, std::wstring fileNameOfSavedFile, std::wstring subFolder = L"");
};
