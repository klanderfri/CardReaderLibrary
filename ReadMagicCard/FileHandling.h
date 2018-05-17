#pragma once
#include "Session.h"
#include <mutex>
//Class for handling files.
class FileHandling
{
public:
	FileHandling();
	~FileHandling();

	//Gets the path to the folder containing the images of the MtG-cards that are to be read.
	static std::wstring GetMtgImageFileFolderPath(Session* session);
	//Gets the paths of the image files with the MtG-cards that are to be read.
	static std::vector<std::wstring> GetMtgImageFilePaths(Session* session, std::wstring fullFolderPath);
	//Adds a row to the specified file.
	static std::wstring AddRowToFile(Session* session, std::wstring textToWrite, std::wstring fileName, std::wstring subFolder);
	//Adds a row to the specified file.
	static std::wstring AddRowToFile(Session* session, std::wstring textToWrite, std::wstring fileName, std::wstring subFolder, std::mutex& fileLock);
	//Gets the path to the specified subfolder in the folder containing the MtG-card images.
	static std::wstring GetSubFolderPath(Session* session, std::wstring subFolder);
	//Creates a file directory.
	static bool CreateFileDirectory(std::wstring fullFolderPath);
	//Creates a string that could be used to postfix files with the same filename.
	static std::wstring CreateFileNumberPostfix(int currentFileNumber, int amountOfFiles = 0);
	//Compares two file names.
	static double CompareFilenames(std::wstring filename1, std::wstring filename2);

private:

	//The name of the folder containing the MtG-card images.
	static const std::wstring MTG_IMAGES_WORKING_FOLDER;
	//A lock preventing multiple threads accessing the same file at the same time.
	static std::mutex m_fileLock;
	//A lock preventing multiple threads from creating the same folder.
	static std::mutex m_createFolderLock;
};
