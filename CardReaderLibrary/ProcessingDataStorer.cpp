#include "stdafx.h"
#include "ProcessingDataStorer.h"

using namespace std;

mutex ProcessingDataStorer::fl_OcrConfidence;
mutex ProcessingDataStorer::fl_SideRelations;
bool ProcessingDataStorer::hwfh_OcrConfidence = false;
bool ProcessingDataStorer::hwfh_SideRelations = false;

ProcessingDataStorer::ProcessingDataStorer(SystemMethods* systemMethods, CardReadInput* inputData)
{
	this->systemMethods = systemMethods;
	this->inputData = inputData;
	this->fileSystem = new FileSystem(systemMethods, inputData);
}


ProcessingDataStorer::~ProcessingDataStorer()
{
	delete fileSystem;
}

wstring ProcessingDataStorer::StoreOcrConfidence(wstring imageFilePath, int numberOfCardReadTries, wstring ocrResult, int ocrConfidence) {

	vector<wstring> headers{ L"Image file name", L"Image file path", L"Number of tries", L"OCR result", L"OCR confidence" };
	vector<wstring> rowData{ fileSystem->GetFileNameFromFilePath(imageFilePath), imageFilePath, to_wstring(numberOfCardReadTries), ocrResult, to_wstring(ocrConfidence) };

	return writeToFile(L"TitleDecodeConfidence.txt", fl_OcrConfidence, hwfh_OcrConfidence, headers, rowData);
}

wstring ProcessingDataStorer::StoreSideRelations(wstring imageFilePath, float sideFactor) {

	vector<wstring> headers{ L"Image file name", L"Image file path", L"Long side to short side" };
	vector<wstring> rowData{ fileSystem->GetFileNameFromFilePath(imageFilePath), imageFilePath, systemMethods->ToWString(sideFactor) };

	return writeToFile(L"CardSidesRelations.txt", fl_SideRelations, hwfh_SideRelations, headers, rowData);
}

wstring ProcessingDataStorer::writeToFile(wstring textfileName, mutex& fileLock, bool& hasWrittenFileHeader, vector<wstring> headers, vector<wstring> rowData) {

	//Make sure the header can't be written by two different threads.
	fileLock.lock();
	if (!hasWrittenFileHeader) {

		wstring rowToAdd = headers[0];
		for (size_t i = 1; i < headers.size(); i++) {
			rowToAdd += L"\t" + headers[i];
		}

		fileSystem->AddRowToFile(rowToAdd, textfileName, inputData->fullOutputFolderPath);

		hasWrittenFileHeader = true;
	}
	fileLock.unlock();

	wstring rowToAdd = rowData[0];
	for (size_t i = 1; i < rowData.size(); i++) {
		rowToAdd += L"\t" + rowData[i];
	}

	return fileSystem->AddRowToFile(rowToAdd, textfileName, inputData->fullOutputFolderPath, fileLock);
}
