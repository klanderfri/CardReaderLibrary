#pragma once
#include "CardReader.h"
#include "SessionBound.h"
#include "CardNameInfo.h"
#include <mutex>
//A collection of card readers.
class CardCollectionReader :
	public SessionBound
{
public:
	CardCollectionReader(Session* session);
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

	//The container holding all the card readers.
	std::vector<CardReader> readers;
	//The current amount of readers in the collection.
	int currentAmountOfReaders;
	//This counter shows the user how many cards of which card name extraction has been initialized.
	int counterForCardExtractionMessage;
	//Lock to prevent several threads print to the console line simultaneously.
	std::mutex consoleLock;
	//The longest filename among the readers.
	std::wstring longestFilename;
	//The amount of card that couldn't be read.
	int amountOfErrors;
};
