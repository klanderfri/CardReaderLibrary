#include "stdafx.h"
#include "OcvCardCollectionReader.h"
#include <iomanip>
#include <iostream>

using namespace std;
using namespace cv;

OcvCardCollectionReader::OcvCardCollectionReader(OcvSystemDependencyClass* systemMethods, bool doDebugging, bool runParallelized)
{
	m_systemMethods = systemMethods;
	m_doDebugging = doDebugging;
	m_runParallelized = runParallelized;
	m_currentSize = 0;
	m_readers = vector<OcvCardReader>();
	m_longestFilename = L"";
}

OcvCardCollectionReader::~OcvCardCollectionReader()
{
}

void OcvCardCollectionReader::AddCard(wstring imageFileName) {

	if (m_currentSize < MaxSize()) {

		m_readers.push_back(OcvCardReader(imageFileName, m_systemMethods, m_doDebugging));
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

int OcvCardCollectionReader::MaxSize() {

	return INT_MAX;
}

int OcvCardCollectionReader::Size() {

	return m_currentSize;
}

int OcvCardCollectionReader::LongestFilenameLength() {

	return (int)(m_longestFilename.length()); //Cast should we find. Hard to thinkt that any OS would allow filenames longer than INT_MAX!
}

int OcvCardCollectionReader::AmountOfErrors() {

	return m_amountOfErrors;
}

vector<OcvCardCollectionReader::CardNameInfo> OcvCardCollectionReader::ExtractCardNames() {

	vector<CardNameInfo> result = vector<CardNameInfo>();
	Range range = getRange();
	m_counterForCardExtractionMessage = 0;
	m_amountOfErrors = 0;
	int longestFilenameLength = LongestFilenameLength();

	wcout << L"Reading cards . . ." << endl;

	if (m_runParallelized) {
		parallel_for_(range, [&](const Range& internalRange) {
			cardNameExtraction(internalRange, result, range.end, longestFilenameLength);
		});
	}
	else {
		cardNameExtraction(range, result, range.end, longestFilenameLength);
	}

	return result;
}

void OcvCardCollectionReader::cardNameExtraction(const Range& range, vector<OcvCardCollectionReader::CardNameInfo>& result, const int amountOfCardsRead, const int longestFilenameLength)
{
	for (int i = range.start; i < range.end; i++)
	{
		wstring fileName, cardName;
		bool success = true;

		try {

			//Extract the card name.
			fileName = m_readers[i].GetImageFileName();
			cardName = m_readers[i].ExtractCardName();

			if (!m_readers[i].GetSuccess()) {
				success = false;
			}
		}
		catch (exception& ex) {

			success = false;
			cardName = m_systemMethods->ToWString((string)ex.what());
		}

		//Print that we have read the image.
		printProgressMessage(fileName, cardName, amountOfCardsRead, longestFilenameLength);

		if (!success) {
			m_amountOfErrors++;
		}

		CardNameInfo info;
		info.CardName = cardName;
		info.FileName = fileName;
		info.Success = success;

		result.push_back(info);
	}
}

Range OcvCardCollectionReader::getRange() {

	int readerSize = (int)m_readers.size(); //The AddCard method should throw exception if more than INT_MAX cards are added!
	Range range = Range(0, readerSize);

	return range;
}

void OcvCardCollectionReader::printProgressMessage(const wstring fileName, const wstring cardName, const int amountOfCardsRead, const int longestFilenameLength)
{
	m_consoleLock.lock();

	size_t spaceForCardAmount = to_string(amountOfCardsRead).length();
	wcout << L"Done reading image "
		<< setw(spaceForCardAmount) << to_wstring(m_counterForCardExtractionMessage + 1) << L" of "
		<< setw(spaceForCardAmount + 2) << to_wstring(amountOfCardsRead) + L": "
		<< left << setw(longestFilenameLength + 3) << L"'" + fileName + L"'"
		<< L"'" + cardName + L"'"
		<< right << endl;
	m_counterForCardExtractionMessage++;

	m_consoleLock.unlock();
}
