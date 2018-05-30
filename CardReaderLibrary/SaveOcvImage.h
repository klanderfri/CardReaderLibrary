#pragma once
#include "FileSystem.h"
//Class for saving images to disk.
class SaveOcvImage :
	public FileSystem
{
public:
	SaveOcvImage(SystemMethods* systemMethods, CardReadInput* inputData);
	~SaveOcvImage();

	//Saves the image data to the specified image file.
	void SaveImageData(cv::Mat imageToSave, std::wstring fileNameOfSavedFile, std::wstring fullFolderPath, int currentFileNumber = 0, int amountOfFiles = 0);
};
