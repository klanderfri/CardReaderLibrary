#pragma once
#include "SystemMethods.h"
#include "CardNameInfo.h"
#include <mutex>
class StoreCardProcessingData
{
public:
	StoreCardProcessingData(SystemMethods* systemMethods);
	~StoreCardProcessingData();

	//Stores the final result.
	void StoreFinalResult(std::vector<CardNameInfo> result);
	//Store the confidence of the OCR read in an external file.
	void StoreOcrConfidence(std::wstring imageFileName, int numberOfTries, std::wstring ocrResult, int ocrConfidence);
	//Store the side relations of the card in an external file.
	void StoreSideRelations(std::wstring imageFileName, float sideFactor);

private:

	/* Abbreviations:
	* 
	* hwfh - has written file header.
	* fl - file lock.
	*/

	void writeToFile(std::wstring textfileName, std::mutex& fileLock, bool& hasWrittenFileHeader, std::vector<std::wstring> headers, std::vector<std::wstring> rowData);

	static bool hasSetSystemMethods;
	//Pointer to object handling the system dependent methods.
	static SystemMethods* systemMethods;
	//The name of the subfolder the externa files should be stored in.
	static std::wstring subfolderName;
	//Lock preventing multiple threads from writing the header in the file storing the title OCR confidence.
	static std::mutex fl_OcrConfidence;
	//Lock preventing multiple threads from writing the header in the file storing the card side relations.
	static std::mutex fl_SideRelations;
	//Tells if the header in the file storing the title OCR confidence has been written.
	static bool hwfh_OcrConfidence;
	//Tells if the header in the file storing the card side relations has been written.
	static bool hwfh_SideRelations;
};

