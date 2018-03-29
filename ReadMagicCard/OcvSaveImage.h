#pragma once
#include "OcvSystemDependencyClass.h"
//Class for saving images to disk.
class OcvSaveImage
{
public:
	OcvSaveImage();
	~OcvSaveImage();

	//Saves the image data to the specified image file.
	static void SaveImageData(OcvSystemDependencyClass* systemMethods, cv::Mat imageToSave, std::wstring fileNameOfSavedFile, std::wstring subFolder = L"");
};
