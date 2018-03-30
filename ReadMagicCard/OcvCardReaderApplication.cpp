#include "stdafx.h"
#include "OcvCardReaderApplication.h"
#include "OcvCardCollectionReader.h"
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

int OcvCardReaderApplication::Run(const bool doDebugging, const bool runParallelized) {

	//Store the start time for later check of efficiency.
	auto start_time = chrono::high_resolution_clock::now();

	//Print a welcome message!
	wcout << L"Welcome to the eminent MtG card reader! Let's read some cards! :-D" << endl << endl;

	//Create variable telling if the run was successful
	bool successful = true;

	//Create an object handling the system dependent methods.
	OcvSystemDependencyClass *systemMethodsPointer = new OcvWindows();

	//Remove old data.
	wstring folderPath = OcvFileHandling::GetSubFolderPath(systemMethodsPointer, L"Image Data");
	boost::filesystem::remove_all(folderPath);

	//Fetch all the file names of the card images.
	wstring mtgFolderPath = OcvFileHandling::GetMtgImageFileFolderPath(systemMethodsPointer);
	wcout << L"Looking for card images in folders . . ." << endl;
	wcout << mtgFolderPath << endl << endl;
	vector<wstring> filenamesOfImages = OcvFileHandling::GetMtgImageFileNames(mtgFolderPath);

	//Check if there are to many files to handle.
	size_t numberOfFiles = filenamesOfImages.size();
	if (numberOfFiles <= (size_t)OcvCardCollectionReader::MaxSize()) {

		//Create a card collection reader.
		OcvCardCollectionReader readers(systemMethodsPointer, (int)numberOfFiles, doDebugging, runParallelized);

		//Run the reader for every card in the working folder.
		for (size_t i = 0; i < numberOfFiles; i++) {

			//Get the current element.
			wstring filename = filenamesOfImages[i];

			//Add the cards to the reader collection
			readers.AddCard(filename);
		}

		//Resize console window to avoid line breaks.
		int lettersToAccommodate = to_string(numberOfFiles).length() * 2 + readers.LongestFilenameLength() + 75;
		systemMethodsPointer->SetConsoleWidthInCharacters(lettersToAccommodate);

		//Fetch the card names.
		auto result = readers.ExtractCardNames();

		//Check if there was any errors.
		for (auto info : result) {
			if (!info.Success) {
				successful = false;
				break;
			}
		}

		//Give a reassuring message... or not.
		wstring message;
		if (successful) {
			message = L"All images was successfully read! Yeay! :-)";
		}
		else {
			int amountOfErrorsInt = readers.AmountOfErrors();
			wstring amountOfErrorsStr = to_wstring(amountOfErrorsInt);
			wstring errorWord = (amountOfErrorsInt == 1) ? L"error" : L"errors";
			message = L"Oh no! There was " + amountOfErrorsStr + L" " + errorWord + L" when reading the cards! :-(";
		}
		wcout << endl << message << endl;
	}
	else {

		//Tell the user that there was to many files for the reader to handle.
		wcout << endl << L"Glunck! There were to many cards for me to handle! Don't feed me more than " + to_wstring(OcvCardCollectionReader::MaxSize()) + L" card image files!" << endl;
	}

	//Print out how long the program took to execute.
	auto end_time = chrono::high_resolution_clock::now();
	auto executionDurationTime = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
	wstring exeTime = doDebugging ? to_wstring(executionDurationTime) : to_wstring(executionDurationTime / (float)1000000);
	wstring timeUnit = doDebugging ? L"microseconds" : L"seconds";
	wcout << endl << L"The reading took " + exeTime + L" " + timeUnit + L" to execute." << endl;
	wcout << L"That's " + to_wstring(executionDurationTime / (numberOfFiles * (float)1000000)) + L" seconds per card on average!" << endl << endl;

	//Destroy the pointer to the object handling the system dependent methods.
	delete systemMethodsPointer;

	//Wait for a keystroke in the window.
	system("pause");
	return 0;
}
