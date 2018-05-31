#include "stdafx.h"
#include "CardReaderApplication.h"
#include "FileHandling.h"
#include "StoreCardProcessingData.h"
#include "Card.h"
#include "TestRunner.h"
#include <iostream>
#include "boost\filesystem.hpp"

using namespace std;

CardReaderApplication::CardReaderApplication(char* runArguments[], int numberOfRunArguments)
{
	toolbox = new Toolbox(runArguments, numberOfRunArguments);
	messages = new ApplicationMessages(toolbox);
}

CardReaderApplication::~CardReaderApplication()
{
	delete messages;
	delete toolbox;
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
	vector<Card> cards;
	size_t numberOfFiles = filepathsOfImages.size();
	if (numberOfFiles == 0) {
		//Tell the user that no files was found.
		messages->printNoImagesMessage();
	}
	else if (numberOfFiles <= (size_t)toolbox->cardReader->GetMaxCardAmount()) {

		//Read the cards.
		cards = readAllCards(filepathsOfImages);
	}
	else {

		//Tell the user that there was to many files for the reader to handle.
		messages->printToManyFilesMessage();
	}
	
	//Print out how long the program took to execute.
	TimePoint endTime = chrono::high_resolution_clock::now();
	messages->printExecutionTimeMessage(startTime, endTime, numberOfFiles, !toolbox->runDebugging);

	//Run tests to see if any code has been broken.
	if (toolbox->runDebugging && !toolbox->runSilent) {

		TestRunner tester(toolbox);
		tester.RunTestCases(cards);
	}

	//Wait for a keystroke in the window.
	if (!toolbox->runSilent) {
		system("pause");
	}

	return 0;
}

vector<Card> CardReaderApplication::readAllCards(vector<wstring> filepathsOfImages)
{
	//Read the cards.
	string readingParameters = createReadingParameter(filepathsOfImages);
	string result = toolbox->cardReader->ReadCardTitles(readingParameters.c_str());
	vector<Card> cards = extractCardData(result);

	//Store the result.
	toolbox->resultStorer->StoreFinalResult(cards);

	//Give a reassuring message... or not.
	int amountOfErrors = 0;
	for (Card card : cards) {
		if (!card.IsSuccessful) { amountOfErrors++; }
	}
	messages->printResultMessage(amountOfErrors);

	return cards;
}

vector<Card> CardReaderApplication::extractCardData(string output) {

	const char DELIMITER = ';';
	const int VALUES_PER_CARD = 5;

	vector<wstring> values;
	int index = 0;
	while ((index = output.find(DELIMITER)) >= 0) {

		wstring value = toolbox->converter->ToWString(output.substr(0, index));
		values.push_back(value);

		output = output.substr(index + 1);
	}

	if (values.size() % VALUES_PER_CARD != 0) {

		throw OperationException("The reading output data has incorrect amount of values!");
	}

	vector<Card> cards;
	for (size_t i = 0; i < values.size(); i += VALUES_PER_CARD) {

		Card card;
		card.FilePath = values[i];
		card.FileName = FileHandling::GetFileNameFromFilePath(card.FilePath);
		card.CardName = values[i + 1];
		card.CardType = stoi(values[i + 2]);
		card.Confidence = stoi(values[i + 3]);
		card.IsSuccessful = toolbox->converter->ToBool(values[i + 4]);

		cards.push_back(card);
	}

	return cards;
}

string CardReaderApplication::createReadingParameter(vector<wstring> filepathsOfImages) {

	string params =
		to_string(toolbox->runSilent) + ";" +
		to_string(toolbox->runParallelized) + ";" +
		"1;" + //Store extracted cards.
		to_string(toolbox->runDebugging) + ";" +
		toolbox->converter->ToString(FileHandling::GetOutputFolderPath()) + ";";

	for (wstring path : filepathsOfImages) {

		params += toolbox->converter->ToString(path) + ";";
	}

	return params;
}

void CardReaderApplication::removeOldData() {

	boost::filesystem::remove_all(FileHandling::GetOutputFolderPath());
}

vector<wstring> CardReaderApplication::getMtgImageFilePaths() {

	vector<wstring> filepaths;

	if (toolbox->filePathToImageToDecode.empty())
	{
		wstring mtgFolderPath = FileHandling::GetMtgImageFileFolderPath();
		messages->printWorkingFolderMessage(mtgFolderPath);
		filepaths = FileHandling::GetMtgImageFilePaths(mtgFolderPath);
	}
	else {

		filepaths.push_back(toolbox->filePathToImageToDecode);
	}

	return filepaths;
}

void CardReaderApplication::reziseCommandWindow(size_t numberOfFiles, int lengthOfLongestFilename) {

	int lettersToAccommodate = to_string(numberOfFiles).length() * 2 + lengthOfLongestFilename + 75;
	toolbox->SetConsoleWidthInCharacters(lettersToAccommodate);
}
