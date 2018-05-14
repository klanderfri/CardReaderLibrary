#pragma once
#include "FileHandling.h"
//Class for saving images to disk.
class SaveOcvImage
	: public FileHandling
{
public:
	SaveOcvImage();
	~SaveOcvImage();

	//Saves the image data to the specified image file.
	static void SaveImageData(Session* session, cv::Mat imageToSave, std::wstring fileNameOfSavedFile, std::wstring subFolder = L"", int currentFileNumber = 0, int amountOfFiles = 0);
};
