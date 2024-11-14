#include "stdafx.h"
#include "FileSystem.h"
#include <filesystem>
#include "boost\filesystem.hpp"
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

mutex FileSystem::m_fileLock;
mutex FileSystem::m_createFolderLock;

FileSystem::FileSystem(SystemMethods* systemMethods, CardReadInput* inputData)
{
	this->systemMethods = systemMethods;
	this->inputData = inputData;
}

FileSystem::~FileSystem()
{
}

wstring FileSystem::GetFileNameFromFilePath(const wstring filePath) {

	return fs::path(filePath).filename().wstring();
}

wstring FileSystem::AddRowToFile(wstring textToWrite, wstring fileName, wstring fullFolderPath) {

	return AddRowToFile(textToWrite, fileName, fullFolderPath, m_fileLock);
}

wstring FileSystem::AddRowToFile(wstring textToWrite, wstring fileName, wstring fullFolderPath, std::mutex& fileLock) {

	wstring fullFilePath = fullFolderPath + fileName;

	if (CreateFileDirectory(fullFolderPath))
	{
		fileLock.lock();

		//Implemented as suggested at
		//https://stackoverflow.com/a/71887365/1997617
		//https://stackoverflow.com/a/18226387/1997617
		wofstream file;
		file.open(fullFilePath);
		file << textToWrite << endl;
		file.close();

		fileLock.unlock();
	}
	else
	{
		//Failed to create directory.
		throw SaveException("Could not create file directory: " + systemMethods->ToString(fullFolderPath));
	}

	return fullFilePath;
}

bool FileSystem::CreateFileDirectory(wstring fullFolderPath) {

	while (!m_createFolderLock.try_lock()) {
		this_thread::sleep_for(chrono::milliseconds(1));
	}

	bool folderExists = true;
	if (!boost::filesystem::exists(fullFolderPath)) {
		folderExists = boost::filesystem::create_directories(fullFolderPath);
	}

	m_createFolderLock.unlock();

	return folderExists;
}

wstring FileSystem::CreateFileNumberPostfix(int currentFileNumber, int amountOfFiles) {

	if (currentFileNumber < 1) {
		throw ParameterException("The first file to get a postfix must have a number bigger than zero!", "currentFileNumber");
	}

	wstring numberStr = to_wstring(currentFileNumber);

	if (amountOfFiles > 0) {
		return L" (" + numberStr + L" of " + to_wstring(amountOfFiles) + L")";
	}
	else {
		return L" (" + numberStr + L")";
	}
}
