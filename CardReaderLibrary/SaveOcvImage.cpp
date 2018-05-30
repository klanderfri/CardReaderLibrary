#include "stdafx.h"
#include "SaveOcvImage.h"
#include <opencv2\imgcodecs.hpp>

using namespace std;
using namespace cv;

SaveOcvImage::SaveOcvImage(SystemMethods* systemMethods, CardReadInput* inputData)
	: FileSystem(systemMethods, inputData)
{
}

SaveOcvImage::~SaveOcvImage()
{
}

//Saves the image and gives the specified name to the file.
void SaveOcvImage::SaveImageData(Mat imageToSave, wstring fileNameOfSavedFile, wstring subFolderName, int currentFileNumber, int amountOfFiles)
{
	if (subFolderName.empty()) {
		throw ParameterException("The subfolder path cannot be empty!", "subFolder");
	}
	if (imageToSave.empty()) {
		throw ParameterException("The image to save cannot be empty!", "imageToSave");
	}
	
	//Add postfix.
	wstring imageNumberPostfix = L"";
	if (currentFileNumber > 0 && amountOfFiles > 0) {
		imageNumberPostfix = CreateFileNumberPostfix(currentFileNumber, amountOfFiles);
	}
	else if (currentFileNumber > 0) {
		imageNumberPostfix = CreateFileNumberPostfix(currentFileNumber);
	}
	fileNameOfSavedFile = systemMethods->AddToEndOfFilename(fileNameOfSavedFile, imageNumberPostfix);

	//Get the path the image should be saved to.
	wstring folderPath = inputData->fullOutputFolderPath + subFolderName + L"\\";
	wstring savePath = folderPath + fileNameOfSavedFile;

	if (CreateFileDirectory(folderPath))
	{
		//Create grey scale image.
		string pathParameter = systemMethods->ToString(savePath);
		bool couldWriteToFile = imwrite(pathParameter, imageToSave);

		if (!couldWriteToFile) {
			throw SaveException("Could not write image to file: " + systemMethods->ToString(savePath));
		}
	}
	else
	{
		//Failed to create directory.
		throw SaveException("Could not create file directory: " + systemMethods->ToString(folderPath));
	}
}
