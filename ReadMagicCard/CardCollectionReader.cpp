#include "stdafx.h"
#include "CardCollectionReader.h"
#include "CardExtractor.h"
#include <iomanip>
#include <iostream>

using namespace std;
using namespace cv;

CardCollectionReader::CardCollectionReader(Session* session)
	: SessionBound(session)
{
	currentAmountOfReaders = 0;
	readers = vector<CardReader>();
	longestFilename = L"";
}

CardCollectionReader::~CardCollectionReader()
{
}

void CardCollectionReader::AddCard(wstring imageFilePath) {

	if (currentAmountOfReaders < MaxSize()) {

		readers.push_back(CardReader(session, imageFilePath));
		currentAmountOfReaders++;

		//Check if the new card has the longest filename.
		wstring imageFileName = session->systemMethods->GetFileNameFromFilePath(imageFilePath);
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

	if (!session->runSilent) {
		wcout << L"Reading cards . . ." << endl;
	}

	if (session->runParallelized) {
		parallel_for_(range, [&](const Range& internalRange) {
			cardNameExtraction(internalRange, result, range.end, lengthOfLongestFilename);
		});
	}
	else {
		cardNameExtraction(range, result, range.end, lengthOfLongestFilename);
	}

	if (!session->runSilent) {
		wcout << endl;
	}

	return result;
}

void CardCollectionReader::cardNameExtraction(const Range& range, vector<CardNameInfo>& result, const int amountOfCardsRead, const int lengthOfLongestFilename) {
	
	for (int i = range.start; i < range.end; i++) {

		CardNameInfo info;
		Mat cardImage;
		wstring imageFilePath = readers[i].GetimageFilePath();
		bool gotCardImage = false;

		try {

			//Extract the card from the image.
			cardImage = CardExtractor::ExtractCard(session, imageFilePath);
			gotCardImage = true;
			
			//Extract the card name.
			readers[i].ReadCardName(cardImage);
			info = readers[i].GetResult();
		}
		catch (exception& ex) {

			info.CardName = session->systemMethods->ToWString((string)ex.what());
			info.FileName = readers[i].GetimageFilePath();
			info.ExtractedCardImage = cardImage;
		}

		assert(!gotCardImage || !info.ExtractedCardImage.empty());

		//Print that we have read the image.
		if (!session->runSilent) {
			printProgressMessage(info.FileName, info.CardName, amountOfCardsRead, lengthOfLongestFilename);
		}

		if (!info.HasGotResult()) {
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
