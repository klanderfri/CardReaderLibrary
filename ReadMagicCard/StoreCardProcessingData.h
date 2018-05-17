#pragma once
#include "SessionBound.h"
#include "CardNameInfo.h"
#include <mutex>
class StoreCardProcessingData : 
	public SessionBound
{
public:
	StoreCardProcessingData(Session* session);
	~StoreCardProcessingData();

	//Stores the final result.
	std::wstring StoreFinalResult(std::vector<CardNameInfo> result);
	//Store the confidence of the OCR read in an external file.
	std::wstring StoreOcrConfidence(std::wstring imageFilePath, int numberOfCardReadTries, std::wstring ocrResult, int ocrConfidence);
	//Store the side relations of the card in an external file.
	std::wstring StoreSideRelations(std::wstring imageFilePath, float sideFactor);

	//The name of the subfolder the external files should be stored in.
	static const std::wstring SubfolderName;

private:

	/* Abbreviations:
	* 
	* hwfh - has written file header.
	* fl - file lock.
	*/

	//Writes the card information data to an external file.
	std::wstring writeToFile(std::wstring textfileName, std::mutex& fileLock, bool& hasWrittenFileHeader, std::vector<std::wstring> headers, std::vector<std::wstring> rowData);
	
	//Lock preventing multiple threads from writing the header in the file storing the title OCR confidence.
	static std::mutex fl_OcrConfidence;
	//Lock preventing multiple threads from writing the header in the file storing the card side relations.
	static std::mutex fl_SideRelations;
	//Tells if the header in the file storing the title OCR confidence has been written.
	static bool hwfh_OcrConfidence;
	//Tells if the header in the file storing the card side relations has been written.
	static bool hwfh_SideRelations;
};

