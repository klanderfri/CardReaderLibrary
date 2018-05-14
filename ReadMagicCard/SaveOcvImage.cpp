#include "stdafx.h"
#include "SaveOcvImage.h"
#include "FileHandling.h"
#include <opencv2\imgcodecs.hpp>

using namespace std;
using namespace cv;

SaveOcvImage::SaveOcvImage()
{
}

SaveOcvImage::~SaveOcvImage()
{
}

//Saves the image and gives the specified name to the file.
void SaveOcvImage::SaveImageData(Session* session, Mat imageToSave, wstring fileNameOfSavedFile, wstring subFolder, int currentFileNumber, int amountOfFiles)
{
	if (subFolder.empty()) {
		throw ParameterException("The subfolder cannot be empty!", "subFolder");
	}
	if (imageToSave.empty()) {
		throw ParameterException("The image to save cannot be empty!", "imageToSave");
	}
	
	//Add postfix.
	wstring imageNumberPostfix = L"";
	if (currentFileNumber > 0 && amountOfFiles > 0) {
		imageNumberPostfix = FileHandling::CreateFileNumberPostfix(currentFileNumber, amountOfFiles);
	}
	else if (currentFileNumber > 0) {
		imageNumberPostfix = FileHandling::CreateFileNumberPostfix(currentFileNumber);
	}
	fileNameOfSavedFile = session->systemMethods->AddToEndOfFilename(fileNameOfSavedFile, imageNumberPostfix);

	//Get the path the image should be saved to.
	wstring saveInFolderPath = FileHandling::GetSubFolderPath(session, subFolder);
	wstring savePath = saveInFolderPath + fileNameOfSavedFile;

	if (FileHandling::CreateFileDirectory(saveInFolderPath))
	{
		//Create grey scale image.
		string pathParameter = session->systemMethods->ToString(savePath);
		bool couldWriteToFile = imwrite(pathParameter, imageToSave);

		if (!couldWriteToFile) {
			throw SaveException("Could not write image to file: " + session->systemMethods->ToString(savePath));
		}
	}
	else
	{
		//Failed to create directory.
		throw SaveException("Could not create file directory: " + session->systemMethods->ToString(saveInFolderPath));
	}
}
