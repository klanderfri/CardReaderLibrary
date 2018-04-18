#pragma once
#include "SystemMethods.h"
#include "CardCollectionReader.h"
#include "TimePoint.h"
#include "ApplicationMessages.h"
//Class holding the card reader application.
class CardReaderApplication
{
public:
	CardReaderApplication(const bool runSilent, const bool runParallelized, const bool runDebugging);
	~CardReaderApplication();

	//Runs the card reader application.
	int Run();

private:

	//Removes lingering old data from previous runs.
	void removeOldData();
	//Fetches the file names of the MtG image files.
	std::vector<std::wstring> getMtgImageFileNames();
	//Creates an collection holding a reader for evere card image.
	CardCollectionReader* createCardReaderCollection(SystemMethods* systemMethods, const std::vector<std::wstring> filenamesOfImages);
	//Resizes the command window to occupy as little space as possible without creating line breakes in its' output texts. 
	void reziseCommandWindow(size_t numberOfFiles, int lengthOfLongestFilename);
	//Reads the title of the card in the provided image files.
	std::vector<CardNameInfo> readAllCards(SystemMethods* systemMethods, const std::vector<std::wstring> filenamesOfImages);

	//Pointer to object handling the system dependent methods.
	SystemMethods* systemMethods;
	//Object holding methods printing messages for the application.
	ApplicationMessages messages;
	//Telling if the program should run without any prints to the command line.
	const bool runSilent;
	//Telling if the extraction should be done parallelized.
	const bool runParallelized;
	//Telling if the program should do debugging such as exporting the images to files.
	const bool runDebugging;
};

