#include "stdafx.h"
#include "CardReaderApplication.h"
#include "FileHandling.h"
#include "StoreCardProcessingData.h"
#include "WindowsMethods.h"
#include "TestRunner.h"
#include <iostream>
#include "boost\filesystem.hpp"

using namespace std;

CardReaderApplication::CardReaderApplication(char* runArguments[], int numberOfRunArguments)
{
	WindowsMethods* systemMethods = new WindowsMethods();
	session = new Session(systemMethods, runArguments, numberOfRunArguments);
	messages = new ApplicationMessages(session);
}

CardReaderApplication::~CardReaderApplication()
{
	delete messages;
	delete session;
}

int CardReaderApplication::Run() {

	//Store the start time for later check of efficiency.
	TimePoint startTime = chrono::high_resolution_clock::now();

	//Print a welcome message!
	messages->printWelcomeMessage();

	//Remove old data.
	removeOldData();

	//Fetch all the file paths of the card images.
	vector<wstring> filepathsOfImages = getMtgImageFilePaths();

	//Check if there are to many files to handle.
	vector<CardNameInfo> result;
	size_t numberOfFiles = filepathsOfImages.size();
	if (numberOfFiles == 0) {
		//Tell the user that no files was found.
		messages->printNoImagesMessage();
	}
	else if (numberOfFiles <= (size_t)CardCollectionReader::MaxSize()) {

		//Read the cards.
		result = readAllCards(filepathsOfImages);
	}
	else {

		//Tell the user that there was to many files for the reader to handle.
		messages->printToManyFilesMessage();
	}
	
	//Print out how long the program took to execute.
	TimePoint endTime = chrono::high_resolution_clock::now();
	messages->printExecutionTimeMessage(startTime, endTime, numberOfFiles, !session->runDebugging);

	//Run tests to see if any code has been broken.
	if (session->runDebugging && !session->runSilent) {

		TestRunner tester(session);
		tester.RunTestCases(result);
	}

	//Wait for a keystroke in the window.
	if (!session->runSilent) {
		system("pause");
	}

	return 0;
}

void CardReaderApplication::removeOldData() {

	wstring folderPath = FileHandling::GetSubFolderPath(session, StoreCardProcessingData::SubfolderName);
	boost::filesystem::remove_all(folderPath);
}

vector<wstring> CardReaderApplication::getMtgImageFilePaths() {

	vector<wstring> filepaths;

	if (session->filePathToImageToDecode.empty())
	{
		wstring mtgFolderPath = FileHandling::GetMtgImageFileFolderPath(session);
		messages->printWorkingFolderMessage(mtgFolderPath);
		filepaths = FileHandling::GetMtgImageFilePaths(session, mtgFolderPath);
	}
	else {

		filepaths.push_back(session->filePathToImageToDecode);
	}

	return filepaths;
}

CardCollectionReader* CardReaderApplication::createCardReaderCollection(const vector<wstring> filepathsOfImages) {

	CardCollectionReader* readers = new CardCollectionReader(session);

	//Add the cards to the reader collection
	for (size_t i = 0; i < filepathsOfImages.size(); i++) {

		readers->AddCard(filepathsOfImages[i]);
	}

	return readers;
}

void CardReaderApplication::reziseCommandWindow(size_t numberOfFiles, int lengthOfLongestFilename) {

	int lettersToAccommodate = to_string(numberOfFiles).length() * 2 + lengthOfLongestFilename + 75;
	session->systemMethods->SetConsoleWidthInCharacters(lettersToAccommodate);
}

vector<CardNameInfo> CardReaderApplication::readAllCards(const vector<wstring> filepathsOfImages) {

	//Create a reader for every card.
	CardCollectionReader* readers = createCardReaderCollection(filepathsOfImages);

	//Resize console window to avoid line breaks.
	reziseCommandWindow(filepathsOfImages.size(), readers->LengthOfLongestFilename());

	//Fetch the card names.
	vector<CardNameInfo> result = readers->ExtractCardNames();

	//Write the card names to a separate file.
	messages->printSavingResultsToDiskMessage();
	StoreCardProcessingData storer = StoreCardProcessingData(session);
	wstring pathToResultFolder = storer.StoreFinalResult(result);
	messages->printResultsHasBeenSavedToDiskMessage(pathToResultFolder);

	//Give a reassuring message... or not.
	messages->printResultMessage(readers->AmountOfErrors());

	//Since we don't need the readers anymore...
	delete readers;

	return result;
}
