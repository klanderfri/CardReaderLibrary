#include "stdafx.h"
#include "FileSystemHelper.h"

using namespace std;

FileSystemHelper::FileSystemHelper(SystemMethods* systemMethods, CardReadInput* inputData)
	: FileSystem(systemMethods, inputData)
{
	this->imageLoader = new LoadOcvImage(systemMethods, inputData);
	this->imageSaver = new SaveOcvImage(systemMethods, inputData);
	this->dataStorer = new ProcessingDataStorer(systemMethods, inputData);
}

FileSystemHelper::~FileSystemHelper()
{
	delete imageLoader;
	delete imageSaver;
	delete dataStorer;
}
