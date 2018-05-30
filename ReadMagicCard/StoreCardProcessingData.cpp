#include "stdafx.h"
#include "StoreCardProcessingData.h"
#include "FileHandling.h"

using namespace std;

StoreCardProcessingData::StoreCardProcessingData()
{
}

StoreCardProcessingData::~StoreCardProcessingData()
{
}

wstring StoreCardProcessingData::StoreFinalResult(vector<Card> result) {

	wstring textToAdd = L"Image file name\tImage file path\tCard name\tCard type\tOCR confidence\tSuccess\n";
	for (Card info : result) {

		textToAdd +=
			info.FileName +
			L"\t" + info.FilePath +
			L"\t" + info.CardName +
			L"\t" + to_wstring(info.CardType) +
			L"\t" + to_wstring(info.Confidence) +
			L"\t" + to_wstring(info.IsSuccessful) +
			L"\n";
	}
	textToAdd = textToAdd.substr(0, textToAdd.size() - 1);
	FileHandling::AddRowToFile(textToAdd, L"CardTitles.txt", FileHandling::MTG_OUTPUT_FOLDER);

	wstring resultFolder = FileHandling::GetSubFolderPath(FileHandling::MTG_OUTPUT_FOLDER);
	return resultFolder;
}
