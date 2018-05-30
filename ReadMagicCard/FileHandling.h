#pragma once
#include <mutex>
//Class for handling files.
class FileHandling
{
public:
	FileHandling();
	~FileHandling();

	//Gets the path to the folder containing the output files.
	static std::wstring GetOutputFolderPath();
	//Gets the path to the folder containing the images of the MtG-cards that are to be read.
	static std::wstring GetMtgImageFileFolderPath();
	//Gets the paths of the image files with the MtG-cards that are to be read.
	static std::vector<std::wstring> GetMtgImageFilePaths(std::wstring fullFolderPath);
	//Adds a row to the specified file.
	static std::wstring AddRowToFile(std::wstring textToWrite, std::wstring fileName, std::wstring subFolder);
	//Adds a row to the specified file.
	static std::wstring AddRowToFile(std::wstring textToWrite, std::wstring fileName, std::wstring subFolder, std::mutex& fileLock);
	//Gets the path to the specified subfolder in the folder containing the MtG-card images.
	static std::wstring GetSubFolderPath(std::wstring subFolder);
	//Creates a file directory.
	static bool CreateFileDirectory(std::wstring fullFolderPath);
	//Compares two file names.
	static double CompareFilenames(std::wstring filename1, std::wstring filename2);

	static std::wstring GetUserPicturesFileDirectory();

	static std::wstring GetCurrentUserName();
	//Extracts the filename from a file path.
	static std::wstring GetFileNameFromFilePath(const std::wstring filePath);

	//The name of the folder containing the MtG-card images.
	static const std::wstring MTG_IMAGES_WORKING_FOLDER;
	//The name of the folder where the output files are to be stored.
	static const std::wstring MTG_OUTPUT_FOLDER;

private:

	//A lock preventing multiple threads accessing the same file at the same time.
	static std::mutex m_fileLock;
	//A lock preventing multiple threads from creating the same folder.
	static std::mutex m_createFolderLock;

	static std::wstring cache_currentUserName;
};
