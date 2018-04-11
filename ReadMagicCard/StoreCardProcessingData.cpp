#include "stdafx.h"
#include "StoreCardProcessingData.h"
#include "FileHandling.h"

using namespace std;

const wstring StoreCardProcessingData::SubfolderName = L"Image Data";
bool StoreCardProcessingData::hasSetSystemMethods = false;
SystemMethods* StoreCardProcessingData::systemMethods;
mutex StoreCardProcessingData::fl_OcrConfidence;
mutex StoreCardProcessingData::fl_SideRelations;
bool StoreCardProcessingData::hwfh_OcrConfidence = false;
bool StoreCardProcessingData::hwfh_SideRelations = false;

StoreCardProcessingData::StoreCardProcessingData(SystemMethods* systemMethods)
{
	if (!hasSetSystemMethods) {
		this->systemMethods = systemMethods;
		this->hasSetSystemMethods = true;
	}
}

StoreCardProcessingData::~StoreCardProcessingData()
{
}

void StoreCardProcessingData::StoreFinalResult(vector<CardNameInfo> result) {

	wstring textToAdd = L"Image file name\tCard name\tOCR confidence\tSuccess\n";
	for (CardNameInfo info : result) {

		textToAdd += info.FileName + L"\t" + info.CardName + L"\t" + to_wstring(info.Confidence) + L"\t" + to_wstring(info.Success) + L"\n";
	}
	textToAdd = textToAdd.substr(0, textToAdd.size() - 1);

	FileHandling::AddRowToFile(systemMethods, textToAdd, L"CardTitles.txt", SubfolderName);
}

void StoreCardProcessingData::StoreOcrConfidence(wstring imageFileName, int numberOfTries, wstring ocrResult, int ocrConfidence) {

	vector<wstring> headers{ L"Image file name", L"Number of tries", L"OCR result", L"OCR confidence" };
	vector<wstring> rowData{ imageFileName, to_wstring(numberOfTries), ocrResult, to_wstring(ocrConfidence) };

	writeToFile(L"TitleDecodeConfidence.txt", fl_OcrConfidence, hwfh_OcrConfidence, headers, rowData);
}

void StoreCardProcessingData::StoreSideRelations(wstring imageFileName, float sideFactor) {

	vector<wstring> headers{ L"Image file name", L"Long side to short side" };
	vector<wstring> rowData{ imageFileName, systemMethods->ToWString(sideFactor) };

	writeToFile(L"CardSidesRelations.txt", fl_SideRelations, hwfh_SideRelations, headers, rowData);
}

void StoreCardProcessingData::writeToFile(wstring textfileName, mutex& fileLock, bool& hasWrittenFileHeader, vector<wstring> headers, vector<wstring> rowData) {

	//Make sure the header can't be written by two different threads.
	fileLock.lock();
	if (!hasWrittenFileHeader) {

		wstring rowToAdd = headers[0];
		for (size_t i = 1; i < headers.size(); i++) {
			rowToAdd += L"\t" + headers[i];
		}

		FileHandling::AddRowToFile(systemMethods, rowToAdd, textfileName, SubfolderName);

		hasWrittenFileHeader = true;
	}
	fileLock.unlock();

	wstring rowToAdd = rowData[0];
	for (size_t i = 1; i < rowData.size(); i++) {
		rowToAdd += L"\t" + rowData[i];
	}

	FileHandling::AddRowToFile(systemMethods, rowToAdd, textfileName, SubfolderName, fileLock);
}
