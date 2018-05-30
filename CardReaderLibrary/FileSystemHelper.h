#pragma once
#include "LoadOcvImage.h"
#include "SaveOcvImage.h"
#include "ProcessingDataStorer.h"
class FileSystemHelper :
	public FileSystem
{
public:
	FileSystemHelper(SystemMethods* systemMethods, CardReadInput* inputData);
	~FileSystemHelper();

	LoadOcvImage* imageLoader;

	SaveOcvImage* imageSaver;
	//Pointer to object providing methods for storing processing data.
	ProcessingDataStorer* dataStorer;
};
