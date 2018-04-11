#include "stdafx.h"
#include "CardReaderApplication.h"
#include "FileHandling.h"
#include "StoreCardProcessingData.h"
#include "WindowsMethods.h"
#include "TestRunner.h"
#include <iostream>
#include "boost\filesystem.hpp"

using namespace std;

CardReaderApplication::CardReaderApplication(const bool runSilent, const bool runParallelized, const bool doDebugging)
	: runSilent(runSilent),
	  runParallelized(runParallelized),
	  doDebugging(doDebugging),
	  systemMethods(new WindowsMethods()),
	  messages(ApplicationMessages(systemMethods, runSilent))
{
}

CardReaderApplication::~CardReaderApplication()
{
	//Go memory! Be FREEEE!!
	delete systemMethods;
}

int CardReaderApplication::Run() {

	//Store the start time for later check of efficiency.
	TimePoint startTime = chrono::high_resolution_clock::now();

	//Print a welcome message!
	messages.printWelcomeMessage();

	//Remove old data.
	removeOldData();

	//Fetch all the file names of the card images.
	vector<wstring> filenamesOfImages = getMtgImageFileNames();

	//Check if there are to many files to handle.
	vector<CardNameInfo> result;
	size_t numberOfFiles = filenamesOfImages.size();
	if (numberOfFiles == 0) {
		//Tell the user that no files was found.
		messages.printNoImagesMessage();
	}
	else if (numberOfFiles <= (size_t)CardCollectionReader::MaxSize()) {

		//Read the cards.
		result = readAllCards(systemMethods, filenamesOfImages);
	}
	else {

		//Tell the user that there was to many files for the reader to handle.
		messages.printToManyFilesMessage();
	}
	
	//Print out how long the program took to execute.
	TimePoint endTime = chrono::high_resolution_clock::now();
	messages.printExecutionTimeMessage(startTime, endTime, numberOfFiles, !doDebugging);

	//Run tests to see if any code has been broken.
	if (doDebugging) {
		runTestCases(result);
	}

	//Wait for a keystroke in the window.
	if (!runSilent) {
		system("pause");
	}

	return 0;
}

void CardReaderApplication::removeOldData() {

	wstring folderPath = FileHandling::GetSubFolderPath(systemMethods, StoreCardProcessingData::SubfolderName);
	boost::filesystem::remove_all(folderPath);
}

vector<wstring> CardReaderApplication::getMtgImageFileNames() {

	wstring mtgFolderPath = FileHandling::GetMtgImageFileFolderPath(systemMethods);
	messages.printWorkingFolderMessage(mtgFolderPath);
	vector<wstring> filenamesOfImages = FileHandling::GetMtgImageFileNames(mtgFolderPath);

	return filenamesOfImages;
}

CardCollectionReader* CardReaderApplication::createCardReaderCollection(SystemMethods* systemMethods, const vector<wstring> filenamesOfImages) {

	CardCollectionReader* readers = new CardCollectionReader(systemMethods, runSilent, runParallelized, doDebugging);

	//Add the cards to the reader collection
	for (size_t i = 0; i < filenamesOfImages.size(); i++) {

		wstring filename = filenamesOfImages[i];
		readers->AddCard(filename);
	}

	return readers;
}

void CardReaderApplication::reziseCommandWindow(size_t numberOfFiles, int lengthOfLongestFilename) {

	int lettersToAccommodate = to_string(numberOfFiles).length() * 2 + lengthOfLongestFilename + 75;
	systemMethods->SetConsoleWidthInCharacters(lettersToAccommodate);
}

vector<CardNameInfo> CardReaderApplication::readAllCards(SystemMethods* systemMethods, const vector<wstring> filenamesOfImages) {

	//Create a reader for every card.
	CardCollectionReader* readers = createCardReaderCollection(systemMethods, filenamesOfImages);

	//Resize console window to avoid line breaks.
	reziseCommandWindow(filenamesOfImages.size(), readers->LengthOfLongestFilename());

	//Fetch the card names.
	vector<CardNameInfo> result = readers->ExtractCardNames();

	//Write the card names to a separate file.
	StoreCardProcessingData storer = StoreCardProcessingData(systemMethods);
	wstring pathToResultFile = storer.StoreFinalResult(result);

	//Give a reassuring message... or not.
	messages.printResultMessage(readers->AmountOfErrors(), pathToResultFile);

	//Since we don't need the readers anymore...
	delete readers;

	return result;
}

void CardReaderApplication::runTestCases(vector<CardNameInfo> result) {

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
