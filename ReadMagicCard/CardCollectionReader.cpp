#include "stdafx.h"
#include "CardCollectionReader.h"
#include <iomanip>
#include <iostream>

using namespace std;
using namespace cv;

CardCollectionReader::CardCollectionReader(SystemMethods* systemMethods, bool runParallelized, bool doDebugging)
{
	m_systemMethods = systemMethods;
	m_doDebugging = doDebugging;
	m_runParallelized = runParallelized;
	m_currentSize = 0;
	m_readers = vector<CardReader>();
	m_longestFilename = L"";
}

CardCollectionReader::~CardCollectionReader()
{
}

void CardCollectionReader::AddCard(wstring imageFileName) {

	if (m_currentSize < MaxSize()) {

		m_readers.push_back(CardReader(imageFileName, m_systemMethods, m_doDebugging));
		m_currentSize++;

		//Check if the new card has the longest filename.
		if (m_longestFilename.length() < imageFileName.length()) {
			m_longestFilename = imageFileName;
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

	return m_currentSize;
}

int CardCollectionReader::LengthOfLongestFilename() {

	return (int)(m_longestFilename.length()); //Cast should be fine. Hard to thinkt that any OS would allow filenames longer than INT_MAX!
}

int CardCollectionReader::AmountOfErrors() {

	return m_amountOfErrors;
}

vector<CardNameInfo> CardCollectionReader::ExtractCardNames() {

	vector<CardNameInfo> result = vector<CardNameInfo>();
	Range range = getRange();
	m_counterForCardExtractionMessage = 0;
	m_amountOfErrors = 0;
	int lengthOfLongestFilename = LengthOfLongestFilename();

	wcout << L"Reading cards . . ." << endl;

	if (m_runParallelized) {
		parallel_for_(range, [&](const Range& internalRange) {
			cardNameExtraction(internalRange, result, range.end, lengthOfLongestFilename);
		});
	}
	else {
		cardNameExtraction(range, result, range.end, lengthOfLongestFilename);
	}

	return result;
}

void CardCollectionReader::cardNameExtraction(const Range& range, vector<CardNameInfo>& result, const int amountOfCardsRead, const int lengthOfLongestFilename)
{
	for (int i = range.start; i < range.end; i++)
	{
		CardNameInfo info;

		try {

			//Extract the card name.
			info.FileName = m_readers[i].GetImageFileName();
			info.CardName = m_readers[i].ExtractCardName();
			info.Confidence = m_readers[i].GetConfidence();
			info.Success = m_readers[i].GetSuccess();
		}
		catch (exception& ex) {

			info.CardName = m_systemMethods->ToWString((string)ex.what());
		}

		//Print that we have read the image.
		printProgressMessage(info.FileName, info.CardName, amountOfCardsRead, lengthOfLongestFilename);

		if (!info.Success) {
			m_amountOfErrors++;
		}

		result.push_back(info);
	}
}

Range CardCollectionReader::getRange() {

	int readerSize = (int)m_readers.size(); //The AddCard method should throw exception if more than INT_MAX cards are added!
	Range range = Range(0, readerSize);

	return range;
}

void CardCollectionReader::printProgressMessage(const wstring fileName, const wstring cardName, const int amountOfCardsRead, const int lengthOfLongestFilename)
{
	m_consoleLock.lock();

	size_t spaceForCardAmount = to_string(amountOfCardsRead).length();
	wcout << L"Done reading image "
		<< setw(spaceForCardAmount) << to_wstring(m_counterForCardExtractionMessage + 1) << L" of "
		<< setw(spaceForCardAmount + 2) << to_wstring(amountOfCardsRead) + L": "
		<< left << setw(lengthOfLongestFilename + 3) << L"'" + fileName + L"'"
		<< L"'" + cardName + L"'"
		<< right << endl;
	m_counterForCardExtractionMessage++;

	m_consoleLock.unlock();
}
