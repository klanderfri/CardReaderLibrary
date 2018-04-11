#include "stdafx.h"
#include "CardCollectionReader.h"
#include <iomanip>
#include <iostream>

using namespace std;
using namespace cv;

CardCollectionReader::CardCollectionReader(SystemMethods* systemMethods, const bool runSilent, bool runParallelized, bool doDebugging)
	: runSilent(runSilent), runParallelized(runParallelized), doDebugging(doDebugging)
{
	systemMethods = systemMethods;
	currentAmountOfReaders = 0;
	readers = vector<CardReader>();
	longestFilename = L"";
}

CardCollectionReader::~CardCollectionReader()
{
}

void CardCollectionReader::AddCard(wstring imageFileName) {

	if (currentAmountOfReaders < MaxSize()) {

		readers.push_back(CardReader(imageFileName, systemMethods, doDebugging));
		currentAmountOfReaders++;

		//Check if the new card has the longest filename.
		if (longestFilename.length() < imageFileName.length()) {
			longestFilename = imageFileName;
		}
	}
	else {

		throw overflow_error("Could not add card to collection! The collection is full!");
	}
}

int CardCollectionReader::MaxSize() {

	return INT_MAX;
}

int CardCollectionReader::Size() {

	return currentAmountOfReaders;
}

int CardCollectionReader::LengthOfLongestFilename() {

	return (int)(longestFilename.length()); //Cast should be fine. Hard to thinkt that any OS would allow filenames longer than INT_MAX!
}

int CardCollectionReader::AmountOfErrors() {

	return amountOfErrors;
}

vector<CardNameInfo> CardCollectionReader::ExtractCardNames() {

	vector<CardNameInfo> result = vector<CardNameInfo>();
	Range range = getRange();
	counterForCardExtractionMessage = 0;
	amountOfErrors = 0;
	int lengthOfLongestFilename = LengthOfLongestFilename();

	if (!runSilent) {
		wcout << L"Reading cards . . ." << endl;
	}

	if (runParallelized) {
		parallel_for_(range, [&](const Range& internalRange) {
			cardNameExtraction(internalRange, result, range.end, lengthOfLongestFilename);
		});
	}
	else {
		cardNameExtraction(range, result, range.end, lengthOfLongestFilename);
	}

	return result;
}

void CardCollectionReader::cardNameExtraction(const Range& range, vector<CardNameInfo>& result, const int amountOfCardsRead, const int lengthOfLongestFilename) {
	
	for (int i = range.start; i < range.end; i++) {

		CardNameInfo info;

		try {

			//Extract the card name.
			info.FileName = readers[i].GetImageFileName();
			info.CardName = readers[i].ExtractCardName();
			info.Confidence = readers[i].GetConfidence();
			info.Success = readers[i].GetSuccess();
		}
		catch (exception& ex) {

			info.CardName = systemMethods->ToWString((string)ex.what());
		}

		//Print that we have read the image.
		if (!runSilent) {
			printProgressMessage(info.FileName, info.CardName, amountOfCardsRead, lengthOfLongestFilename);
		}

		if (!info.Success) {
			amountOfErrors++;
		}

		result.push_back(info);
	}
}

Range CardCollectionReader::getRange() {

	int readerSize = (int)readers.size(); //The AddCard method should throw exception if more than INT_MAX cards are added!
	Range range = Range(0, readerSize);

	return range;
}

void CardCollectionReader::printProgressMessage(const wstring fileName, const wstring cardName, const int amountOfCardsRead, const int lengthOfLongestFilename) {

	consoleLock.lock();

	size_t spaceForCardAmount = to_string(amountOfCardsRead).length();
	wcout << L"Done reading image "
		<< setw(spaceForCardAmount) << to_wstring(counterForCardExtractionMessage + 1) << L" of "
		<< setw(spaceForCardAmount + 2) << to_wstring(amountOfCardsRead) + L": "
		<< left << setw(lengthOfLongestFilename + 3) << L"'" + fileName + L"'"
		<< L"'" + cardName + L"'"
		<< right << endl;
	counterForCardExtractionMessage++;

	consoleLock.unlock();
}
