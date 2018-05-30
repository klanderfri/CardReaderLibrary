#pragma once
#include "SystemMethods.h"
#include "CardReadInput.h"
#include <mutex>
class FileSystem
{
public:
	FileSystem(SystemMethods* systemMethods, CardReadInput* inputData);
	~FileSystem();

	//Extracts the filename from a file path.
	std::wstring GetFileNameFromFilePath(const std::wstring filePath);
	//Adds a row to the specified file.
	std::wstring AddRowToFile(std::wstring textToWrite, std::wstring fileName, std::wstring fullFolderPath);
	//Adds a row to the specified file.
	std::wstring AddRowToFile(std::wstring textToWrite, std::wstring fileName, std::wstring fullFolderPath, std::mutex& fileLock);
	//Creates a file directory.
	bool CreateFileDirectory(std::wstring fullFolderPath);
	//Creates a string that could be used to postfix files with the same filename.
	static std::wstring CreateFileNumberPostfix(int currentFileNumber, int amountOfFiles = 0);

protected:


	SystemMethods* systemMethods;
	
	CardReadInput* inputData;

private:

	//A lock preventing multiple threads accessing the same file at the same time.
	static std::mutex m_fileLock;
	//A lock preventing multiple threads from creating the same folder.
	static std::mutex m_createFolderLock;
};

