#pragma once
#include "CardReader.h"
#include "CardNameInfo.h"
#include <mutex>
//A collection of card readers.
class CardCollectionReader
{
public:
	CardCollectionReader(SystemMethods* systemMethods, const bool runSilent, bool runParallelized, bool doDebugging);
	~CardCollectionReader();

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
	SystemMethods* m_systemMethods;
	//Telling if the program should run without any prints to the command line.
	const bool m_runSilent;
	//Telling if the extraction should be done parallelized.
	const bool m_runParallelized;
	//Telling if the program should do debugging such as exporting the images to files.
	const bool m_doDebugging;
	//The container holding all the card readers.
	std::vector<CardReader> m_readers;
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
