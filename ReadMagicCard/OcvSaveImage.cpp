#include "stdafx.h"
#include "OcvSaveImage.h"
#include "OcvFileHandling.h"
#include <opencv2\imgcodecs.hpp>

using namespace std;
using namespace cv;

OcvSaveImage::OcvSaveImage()
{
}

OcvSaveImage::~OcvSaveImage()
{
}

//Saves the image and gives the specified name to the file.
void OcvSaveImage::SaveImageData(OcvSystemDependencyClass* systemMethods, Mat imageToSave, wstring fileNameOfSavedFile, wstring subFolder)
{
	if (subFolder.empty()) {
		throw OcvParameterException("The subfolder cannot be empty!", "subFolder");
	}

	//Get the path the image should be saved to.
	wstring saveInFolderPath = OcvFileHandling::GetSubFolderPath(systemMethods, subFolder);
	wstring savePath = saveInFolderPath + fileNameOfSavedFile;

	if (OcvFileHandling::CreateFileDirectory(saveInFolderPath))
	{
		//Create grey scale image.
		string pathParameter = systemMethods->ToString(savePath);
		bool couldWriteToFile = imwrite(pathParameter, imageToSave);

		if (!couldWriteToFile) {
			throw OcvSaveException("Could not write image to file: " + systemMethods->ToString(savePath));
		}
	}
	else
	{
		//Failed to create directory.
		throw OcvSaveException("Could not create file directory: " + systemMethods->ToString(saveInFolderPath));
	}
}
