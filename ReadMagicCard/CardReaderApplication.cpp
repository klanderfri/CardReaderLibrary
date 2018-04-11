#include "stdafx.h"
#include "CardReaderApplication.h"
#include "FileHandling.h"
#include "StoreCardProcessingData.h"
#include "WindowsMethods.h"
#include "TestRunner.h"
#include <iostream>
#include "boost\filesystem.hpp"

using namespace std;

CardReaderApplication::CardReaderApplication()
{
}

CardReaderApplication::~CardReaderApplication()
{
}

int CardReaderApplication::Run(const bool runSilent, const bool runParallelized, const bool doDebugging) {

	//Store the start time for later check of efficiency.
	auto startTime = chrono::high_resolution_clock::now();

	//Print a welcome message!
	wcout << L"Welcome to the eminent MtG card reader! Let's read some cards! :-D" << endl << endl;

	//Create an object handling the system dependent methods.
	SystemMethods* systemMethods = new WindowsMethods();

	//Remove old data.
	removeOldData(systemMethods);

	//Fetch all the file names of the card images.
	vector<wstring> filenamesOfImages = getMtgImageFileNames(systemMethods);

	//Check if there are to many files to handle.
	vector<CardNameInfo> result;
	size_t numberOfFiles = filenamesOfImages.size();
	if (numberOfFiles == 0) {
		//Tell the user that no files was found.
		printNoImagesMessage(systemMethods);
	}
	else if (numberOfFiles <= (size_t)CardCollectionReader::MaxSize()) {

		//Read the cards.
		result = readAllCards(systemMethods, filenamesOfImages, runParallelized, doDebugging);
	}
	else {

		//Tell the user that there was to many files for the reader to handle.
		printToManyFilesMessage(systemMethods);
	}
	
	//Print out how long the program took to execute.
	TimePoint endTime = chrono::high_resolution_clock::now();
	printExecutionTimeMessage(systemMethods, startTime, endTime, numberOfFiles, !doDebugging);

	//Run tests to see if any code has been broken.
	if (doDebugging) {
		runTestCases(systemMethods, result);
	}

	//Go memory! Be FREEEE!!
	delete systemMethods;

	//Wait for a keystroke in the window.
	system("pause");
	return 0;
}

void CardReaderApplication::removeOldData(SystemMethods* systemMethods) {

	wstring folderPath = FileHandling::GetSubFolderPath(systemMethods, StoreCardProcessingData::SubfolderName);
	boost::filesystem::remove_all(folderPath);
}

vector<wstring> CardReaderApplication::getMtgImageFileNames(SystemMethods* systemMethods) {

	wstring mtgFolderPath = FileHandling::GetMtgImageFileFolderPath(systemMethods);
	wcout << L"Looking for card images in folders . . ." << endl;
	wcout << mtgFolderPath << endl << endl;
	vector<wstring> filenamesOfImages = FileHandling::GetMtgImageFileNames(mtgFolderPath);

	return filenamesOfImages;
}

CardCollectionReader* CardReaderApplication::createCardReaderCollection(SystemMethods* systemMethods, const vector<wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging) {

	CardCollectionReader *readers = new CardCollectionReader(systemMethods, runParallelized, doDebugging);

	//Add the cards to the reader collection
	for (size_t i = 0; i < filenamesOfImages.size(); i++) {

		wstring filename = filenamesOfImages[i];
		readers->AddCard(filename);
	}

	return readers;
}

void CardReaderApplication::reziseCommandWindow(SystemMethods* systemMethods, size_t numberOfFiles, int lengthOfLongestFilename) {

	int lettersToAccommodate = to_string(numberOfFiles).length() * 2 + lengthOfLongestFilename + 75;
	systemMethods->SetConsoleWidthInCharacters(lettersToAccommodate);
}

void CardReaderApplication::printResultMessage(SystemMethods* systemMethods, int numberOfErrors) {

	wstring message;
	if (numberOfErrors == 0) {
		systemMethods->SetCommandLineTextColour(Colour::Green);
		message = L"All images was successfully read! Yay! :-)";
	}
	else {
		systemMethods->SetCommandLineTextColour(Colour::Red);
		wstring amountOfErrorsStr = to_wstring(numberOfErrors);
		wstring errorWord = (numberOfErrors == 1) ? L"error" : L"errors";
		message = L"Oh no! There was " + amountOfErrorsStr + L" " + errorWord + L" when reading the cards! :-(";
	}

	wcout << endl << message << endl;
	systemMethods->ResetCommandLineTextColour();
}

vector<CardNameInfo> CardReaderApplication::readAllCards(SystemMethods* systemMethods, const vector<wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging) {

	//Create a reader for every card.
	CardCollectionReader* readers = createCardReaderCollection(systemMethods, filenamesOfImages, runParallelized, doDebugging);

	//Resize console window to avoid line breaks.
	reziseCommandWindow(systemMethods, filenamesOfImages.size(), readers->LengthOfLongestFilename());

	//Fetch the card names.
	vector<CardNameInfo> result = readers->ExtractCardNames();

	//Write the card names to a separate file.
	StoreCardProcessingData storer = StoreCardProcessingData(systemMethods);
	storer.StoreFinalResult(result);

	//Give a reassuring message... or not.
	printResultMessage(systemMethods, readers->AmountOfErrors());

	//Since we don't need the readers anymore...
	delete readers;

	return result;
}

void CardReaderApplication::printNoImagesMessage(SystemMethods* systemMethods) {

	systemMethods->SetCommandLineTextColour(Colour::Yellow);
	wcout
		<< L"Hmm, it semms like there are no cards... :-|"
		<< endl;
	systemMethods->ResetCommandLineTextColour();
}

void CardReaderApplication::printToManyFilesMessage(SystemMethods* systemMethods) {

	systemMethods->SetCommandLineTextColour(Colour::Red);
	wcout
		<< L"Glunck! There were to many cards for me to handle! Don't feed me more than "
		+ to_wstring(CardCollectionReader::MaxSize())
		+ L" card image files!"
		<< endl;
	systemMethods->ResetCommandLineTextColour();
}

long long CardReaderApplication::getexecutionTime(TimePoint startTime, TimePoint endTime) {
	
	return chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
}

void CardReaderApplication::printExecutionTimeMessage(SystemMethods* systemMethods, TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds) {

	auto executionDurationTime = getexecutionTime(startTime, endTime);

	//Print total execution time.
	float totalExecutionTime = executionDurationTime / (float)(showTimeInSeconds ? 1000000 : 1);
	wstring exeTime = systemMethods->ToWString(totalExecutionTime, showTimeInSeconds ? 1 : 0);
	wstring timeUnit = showTimeInSeconds ? L"seconds" : L"microseconds";
	wcout << endl << L"The reading took " + exeTime + L" " + timeUnit + L" to execute." << endl;

	//Print card avarage execution time.
	if (numberOfFilesExecuted > 0) {
		float executionTimePerCard = executionDurationTime / numberOfFilesExecuted / (float)1000000;
		wstring avarageExecutionTimePerCard = systemMethods->ToWString(executionTimePerCard, showTimeInSeconds ? 2 : 8);
		wcout << L"That's " + avarageExecutionTimePerCard + L" seconds per card on average!" << endl;
	}

	wcout << endl;
}

void CardReaderApplication::runTestCases(SystemMethods* systemMethods, vector<CardNameInfo> result) {

	vector<CardNameInfo> incorrectResults;
	TestRunner tester(systemMethods);
	bool testsSucceded = tester.RunTests(result, incorrectResults);

	if (testsSucceded) {
		systemMethods->SetCommandLineTextColour(Colour::Green);
		wcout << L"All test cases still works!" << endl << endl;
	}
	else if (incorrectResults.size() == 0) {
		systemMethods->SetCommandLineTextColour(Colour::Red);
		wcout << L"There are missing test cases!" << endl << endl;
	}
	else {
		systemMethods->SetCommandLineTextColour(Colour::Red);
		wcout << L"There are " + to_wstring(incorrectResults.size()) + L" broken test cases!" << endl;
		for (CardNameInfo subResult : incorrectResults) {
			wcout << subResult.FileName + L" got '" + subResult.CardName + L"'" << endl;
		}
		wcout << endl;
	}

	systemMethods->ResetCommandLineTextColour();
}
