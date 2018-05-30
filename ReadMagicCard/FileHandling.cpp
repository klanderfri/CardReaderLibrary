#include "stdafx.h"
#include "FileHandling.h"
#include "Converter.h"
#include <experimental\filesystem>
#include <chrono>
#include "boost\algorithm\string.hpp"
#include "boost\filesystem.hpp"
#include <windows.h>
#include <Lmcons.h>

namespace fs = std::experimental::filesystem;
using namespace std;

const wstring FileHandling::MTG_IMAGES_WORKING_FOLDER = L"MtG-cards";
const wstring FileHandling::MTG_OUTPUT_FOLDER = L"Image Data";
mutex FileHandling::m_fileLock;
mutex FileHandling::m_createFolderLock;
wstring FileHandling::cache_currentUserName = L"";

FileHandling::FileHandling()
{
}

FileHandling::~FileHandling()
{
}

wstring FileHandling::GetOutputFolderPath() {

	//Create the file path.
	return GetMtgImageFileFolderPath() + MTG_OUTPUT_FOLDER + L"\\";
}

wstring FileHandling::GetMtgImageFileFolderPath() {

	//Create the file path.
	return GetUserPicturesFileDirectory() + MTG_IMAGES_WORKING_FOLDER + L"\\";
}

vector<wstring> FileHandling::GetMtgImageFilePaths(wstring fullFolderPath) {

	//Create a list holding the filenames.
	vector<wstring> filepaths;

	//Run the reader for every card in the working folder.
	for (auto & p : fs::directory_iterator(fullFolderPath)) {

		//Fetch the file name.
		wstring filePath = p.path().wstring();

		//Skip all folders.
		if (boost::filesystem::is_directory(filePath)) {
			continue;
		}

		//Add the file path to the vector.
		filepaths.push_back(filePath);
	}

	return filepaths;
}

wstring FileHandling::AddRowToFile(wstring textToWrite, wstring fileName, wstring subFolder) {

	return AddRowToFile(textToWrite, fileName, subFolder, m_fileLock);
}

wstring FileHandling::AddRowToFile(wstring textToWrite, wstring fileName, wstring subFolder, std::mutex& fileLock) {

	wstring folderPath = GetSubFolderPath(subFolder);
	wstring fullFilePath = folderPath + fileName;

	if (CreateFileDirectory(folderPath))
	{
		fileLock.lock();

		//Implemented as suggested at
		//http://www.publicstaticfinal.com/2015/12/18/converting-stdwstring-to-utf-8-in-c-x11-and-writing-utf-8-files-with-fstream/
		ofstream file(fullFilePath, fstream::in | fstream::out | fstream::app);
		wbuffer_convert<codecvt_utf8<wchar_t>> converter(file.rdbuf());
		wostream out(&converter);
		out << textToWrite << endl;
		file.close();

		fileLock.unlock();
	}
	else
	{
		//Failed to create directory.
		Converter converter = Converter();
		throw SaveException("Could not create file directory: " + converter.ToString(folderPath));
	}

	return fullFilePath;
}

wstring FileHandling::GetSubFolderPath(wstring subFolder)
{
	//Check that we actually got a subfolder.
	if (subFolder.empty()) {
		throw OperationException("The subFolder parameter cannot be empty!");
	}

	//Create the file path.
	return FileHandling::GetMtgImageFileFolderPath() + subFolder + L"\\";
}

bool FileHandling::CreateFileDirectory(wstring fullFolderPath) {

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

double FileHandling::CompareFilenames(std::wstring filename1, std::wstring filename2) {

	wstring temp_name1 = filename1;
	wstring temp_name2 = filename2;
	boost::to_lower(temp_name1);
	boost::to_lower(temp_name2);

	return temp_name1.compare(temp_name2);
}

wstring FileHandling::GetUserPicturesFileDirectory() {

	return L"C:\\Users\\" + GetCurrentUserName() + L"\\Pictures\\";
}

wstring FileHandling::GetCurrentUserName() {

	if (cache_currentUserName.empty())
	{
		//Get username.
		wchar_t username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;
		GetUserNameW(username, &username_len);

		cache_currentUserName = wstring(username);
	}

	return cache_currentUserName;
}

wstring FileHandling::GetFileNameFromFilePath(const wstring filePath) {

	return fs::path(filePath).filename().wstring();
}
