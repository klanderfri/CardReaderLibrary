#include "stdafx.h"
#include "OcvCardReaderApplication.h"
#include "OcvFileHandling.h"
#include "OcvWindows.h"
#include <iostream>
#include "boost\filesystem.hpp"

using namespace std;

OcvCardReaderApplication::OcvCardReaderApplication()
{
}

OcvCardReaderApplication::~OcvCardReaderApplication()
{
}

int OcvCardReaderApplication::Run(const bool runParallelized, const bool doDebugging) {

	//Store the start time for later check of efficiency.
	auto startTime = chrono::high_resolution_clock::now();

	//Print a welcome message!
	wcout << L"Welcome to the eminent MtG card reader! Let's read some cards! :-D" << endl << endl;

	//Create an object handling the system dependent methods.
	OcvSystemDependencyClass* systemMethods = new OcvWindows();

	//Remove old data.
	removeOldData(systemMethods);

	//Fetch all the file names of the card images.
	vector<wstring> filenamesOfImages = getMtgImageFileNames(systemMethods);

	//Check if there are to many files to handle.
	size_t numberOfFiles = filenamesOfImages.size();
	if (numberOfFiles <= (size_t)OcvCardCollectionReader::MaxSize()) {

		//Read the cards.
		readAllCards(systemMethods, filenamesOfImages, runParallelized, doDebugging);
	}
	else {

		//Tell the user that there was to many files for the reader to handle.
		printToManyFilesMessage();
	}

	//Go memory! Be FREEEE!!
	delete systemMethods;
	
	//Print out how long the program took to execute.
	TimePoint endTime = chrono::high_resolution_clock::now();
	printExecutionTimeMessage(startTime, endTime, numberOfFiles, !doDebugging);

	//Wait for a keystroke in the window.
	system("pause");
	return 0;
}

void OcvCardReaderApplication::removeOldData(OcvSystemDependencyClass* systemMethods) {

	wstring folderPath = OcvFileHandling::GetSubFolderPath(systemMethods, L"Image Data");
	boost::filesystem::remove_all(folderPath);
}

vector<wstring> OcvCardReaderApplication::getMtgImageFileNames(OcvSystemDependencyClass* systemMethods) {

	wstring mtgFolderPath = OcvFileHandling::GetMtgImageFileFolderPath(systemMethods);
	wcout << L"Looking for card images in folders . . ." << endl;
	wcout << mtgFolderPath << endl << endl;
	vector<wstring> filenamesOfImages = OcvFileHandling::GetMtgImageFileNames(mtgFolderPath);

	return filenamesOfImages;
}

OcvCardCollectionReader* OcvCardReaderApplication::createCardReaderCollection(OcvSystemDependencyClass* systemMethods, const vector<wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging) {

	OcvCardCollectionReader *readers = new OcvCardCollectionReader(systemMethods, runParallelized, doDebugging);

	//Add the cards to the reader collection
	for (size_t i = 0; i < filenamesOfImages.size(); i++) {

		wstring filename = filenamesOfImages[i];
		readers->AddCard(filename);
	}

	return readers;
}

void OcvCardReaderApplication::reziseCommandWindow(OcvSystemDependencyClass* systemMethods, size_t numberOfFiles, int lengthOfLongestFilename) {

	int lettersToAccommodate = to_string(numberOfFiles).length() * 2 + lengthOfLongestFilename + 75;
	systemMethods->SetConsoleWidthInCharacters(lettersToAccommodate);
}

void OcvCardReaderApplication::printResultMessage(int numberOfErrors) {

	wstring message;
	if (numberOfErrors == 0) {
		message = L"All images was successfully read! Yeay! :-)";
	}
	else {
		wstring amountOfErrorsStr = to_wstring(numberOfErrors);
		wstring errorWord = (numberOfErrors == 1) ? L"error" : L"errors";
		message = L"Oh no! There was " + amountOfErrorsStr + L" " + errorWord + L" when reading the cards! :-(";
	}

	wcout << endl << message << endl;
}

void OcvCardReaderApplication::readAllCards(OcvSystemDependencyClass* systemMethods, const vector<wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging) {

	//Create a reader for every card.
	OcvCardCollectionReader* readers = createCardReaderCollection(systemMethods, filenamesOfImages, runParallelized, doDebugging);

	//Resize console window to avoid line breaks.
	reziseCommandWindow(systemMethods, filenamesOfImages.size(), readers->LengthOfLongestFilename());

	//Fetch the card names.
	vector<CardNameInfo> result = readers->ExtractCardNames();

	//Give a reassuring message... or not.
	printResultMessage(readers->AmountOfErrors());

	//Since we don't need the readers anymore...
	delete readers;
}

void OcvCardReaderApplication::printToManyFilesMessage() {

	wcout
		<< endl
		<< L"Glunck! There were to many cards for me to handle! Don't feed me more than "
		+ to_wstring(OcvCardCollectionReader::MaxSize())
		+ L" card image files!"
		<< endl;
}

long long OcvCardReaderApplication::getexecutionTime(TimePoint startTime, TimePoint endTime) {
	
	return chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
}

void OcvCardReaderApplication::printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds) {

	auto executionDurationTime = getexecutionTime(startTime, endTime);
	wstring exeTime = showTimeInSeconds ? to_wstring(executionDurationTime / (float)1000000) : to_wstring(executionDurationTime);
	wstring timeUnit = showTimeInSeconds ? L"seconds" : L"microseconds";
	wcout << endl << L"The reading took " + exeTime + L" " + timeUnit + L" to execute." << endl;
	wcout << L"That's " + to_wstring(executionDurationTime / (numberOfFilesExecuted * (float)1000000)) + L" seconds per card on average!" << endl << endl;
}
