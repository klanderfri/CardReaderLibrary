#include "stdafx.h"
#include "OcvFileHandling.h"
#include <experimental\filesystem>
#include "boost\filesystem.hpp"

namespace fs = std::experimental::filesystem;
using namespace std;

const wstring OcvFileHandling::MTG_IMAGES_WORKING_FOLDER = L"MtG-cards";

mutex OcvFileHandling::m_fileLock;

OcvFileHandling::OcvFileHandling()
{
}

OcvFileHandling::~OcvFileHandling()
{
}

wstring OcvFileHandling::GetMtgImageFileFolderPath(OcvSystemDependencyClass* systemMethods) {

	//Create the file path.
	return systemMethods->GetUserPicturesFileDirectory() + MTG_IMAGES_WORKING_FOLDER + L"\\";
}

vector<wstring> OcvFileHandling::GetMtgImageFileNames(OcvSystemDependencyClass* systemMethods) {

	//Create a list holding the filenames.
	vector<wstring> filenames;

	//Get all the file names.
	wstring folderPath = GetMtgImageFileFolderPath(systemMethods);

	//Run the reader for every card in the working folder.
	for (auto & p : fs::directory_iterator(folderPath)) {

		//Fetch the file name.
		wstring filePath = p.path().wstring();
		wstring fileName = fs::path(filePath).filename().wstring();

		//Skip all folders.
		if (boost::filesystem::is_directory(filePath)) {
			continue;
		}

		//Add the filename to the vector.
		filenames.push_back(fileName);
	}

	return filenames;
}

void OcvFileHandling::AddRowToFile(OcvSystemDependencyClass* systemMethods, wstring textToWrite, wstring fileName, wstring subFolder) {

	wstring folderPath = GetSubFolderPath(systemMethods, subFolder);
	wstring fullFilePath = folderPath + fileName;

	if (CreateFileDirectory(folderPath))
	{
		m_fileLock.lock();

		//Implemented as suggested at
		//http://www.publicstaticfinal.com/2015/12/18/converting-stdwstring-to-utf-8-in-c-x11-and-writing-utf-8-files-with-fstream/
		ofstream file(fullFilePath, fstream::in | fstream::out | fstream::app);
		wbuffer_convert<codecvt_utf8<wchar_t>> converter(file.rdbuf());
		wostream out(&converter);
		out << textToWrite << endl;
		file.close();

		m_fileLock.unlock();
	}
	else
	{
		//Failed to create directory.
		throw OcvSaveException("Could not create file directory: " + systemMethods->ToString(folderPath));
	}
}

wstring OcvFileHandling::GetSubFolderPath(OcvSystemDependencyClass* systemMethods, wstring subFolder)
{
	//Check that we actually got a subfolder.
	if (subFolder.empty()) {
		throw OcvOperationException("The subFolder parameter cannot be empty!");
	}

	//Create the file path.
	return OcvFileHandling::GetMtgImageFileFolderPath(systemMethods) + subFolder + L"\\";
}

bool OcvFileHandling::CreateFileDirectory(wstring fullFolderPath) {

	if (boost::filesystem::exists(fullFolderPath)) { return true; }
	return boost::filesystem::create_directory(fullFolderPath);
}
