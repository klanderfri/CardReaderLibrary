#pragma once
#include "OcvCardReader.h"
#include "CardNameInfo.h"
#include <mutex>
//A collection of card readers.
class OcvCardCollectionReader
{
public:
	OcvCardCollectionReader(OcvSystemDependencyClass* systemMethods, bool runParallelized, bool doDebugging);
	~OcvCardCollectionReader();

	//Adds a card to the collection.
	void AddCard(std::wstring imageFileName);
	//Extracts the card names.
	std::vector<CardNameInfo> ExtractCardNames();
	//Gets the maximal amount of cards the reader can hold.
	static int MaxSize();
	//Gets the amount of cards the reader holds.
	int Size();
	//Gets the lenght of the longest filename.
	int LengthOfLongestFilename();
	//Gets the amount of card that couldn't be read.
	int AmountOfErrors();

private:

	//Extracts the card names from the card images.
	void cardNameExtraction(const cv::Range& range, std::vector<CardNameInfo>& result, const int amountOfCardsRead, const int lengthOfLongestFilename);
	//Prints a message about the progress.
	void printProgressMessage(const std::wstring fileName, const std::wstring cardName, const int amountOfCardsRead, const int lengthOfLongestFilename);
	//Gets the range of the internal readers.
	cv::Range getRange();

	//Pointer to object handling the system dependent methods.
	OcvSystemDependencyClass * m_systemMethods;
	//Telling if the program should do debugging such as exporting the images to files.
	bool m_doDebugging;
	//Telling if the extraction should be done parallelized.
	bool m_runParallelized;
	//The container holding all the card readers.
	std::vector<OcvCardReader> m_readers;
	//The current amount of readers in the collection.
	int m_currentSize;
	//This counter shows the user how many cards of which card name extraction has been initialized.
	int m_counterForCardExtractionMessage;
	//Lock to prevent several threads print to the console line simultaneously.
	std::mutex m_consoleLock;
	//The longest filename among the readers.
	std::wstring m_longestFilename;
	//The amount of card that couldn't be read.
	int m_amountOfErrors;
};
