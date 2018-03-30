#pragma once
#include "OcvSystemDependencyClass.h"
#include <mutex>
//Class for handling files.
class OcvFileHandling
{
public:
	OcvFileHandling();
	~OcvFileHandling();

	//Gets the path to the folder containing the images of the MtG-cards that are to be read.
	static std::wstring GetMtgImageFileFolderPath(OcvSystemDependencyClass* systemMethods);
	//Gets the names of the image files with the MtG-cards that are to be read.
	static std::vector<std::wstring> GetMtgImageFileNames(std::wstring fullFolderPath);
	//Adds a row to the specified file.
	static void AddRowToFile(OcvSystemDependencyClass* systemMethods, std::wstring textToWrite, std::wstring fileName, std::wstring subFolder);
	//Gets the path to the specified subfolder in the folder containing the MtG-card images.
	static std::wstring GetSubFolderPath(OcvSystemDependencyClass* systemMethods, std::wstring subFolder);
	//Creates a file directory.
	static bool CreateFileDirectory(std::wstring fullFolderPath);

private:

	//The name of the folder containing the MtG-card images.
	static const std::wstring MTG_IMAGES_WORKING_FOLDER;
	//A lock preventing multiple threads accessing the same file at the same time.
	static std::mutex m_fileLock;
};
