#pragma once
#include "SystemMethods.h"
#include "CardCollectionReader.h"
#include <chrono>
//Class holding the card reader application.
class CardReaderApplication
{
public:
	CardReaderApplication();
	~CardReaderApplication();

	//Runs the card reader application.
	int Run(const bool runSilent, const bool runParallelized, const bool doDebugging);

private:

	//A chrono time point for steady clock.
	typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

	//Prints a welcome message for the user.
	void printWelcomeMessage();
	//Prints a message telling the user where the program is looking for card image files.
	void printWorkingFolderMessage(std::wstring mtgFolderPath);
	//Removes lingering old data from previous runs.
	void removeOldData();
	//Fetches the file names of the MtG image files.
	std::vector<std::wstring> getMtgImageFileNames();
	//Creates an collection holding a reader for evere card image.
	CardCollectionReader* createCardReaderCollection(SystemMethods* systemMethods, const std::vector<std::wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging);
	//Resizes the command window to occupy as little space as possible without creating line breakes in its' output texts. 
	void reziseCommandWindow(SystemMethods* systemMethods, size_t numberOfFiles, int lengthOfLongestFilename);
	//Prints a message telling the user how the execution went.
	void printResultMessage(SystemMethods* systemMethods, int numberOfErrors, std::wstring pathToResultFile);
	//Reads the title of the card in the provided image files.
	std::vector<CardNameInfo> readAllCards(SystemMethods* systemMethods, const std::vector<std::wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging);
	//Prints a message telling the user that there are no files to read.
	void printNoImagesMessage();
	//Prints a message telling the user that there are to many files to successfully read them all.
	void printToManyFilesMessage();
	//Extracts the time (in milliseconds) that passed between two time points.
	long long getexecutionTime(TimePoint startTime, TimePoint endTime);
	//Prints a message telling the user the length of the execution time.
	void printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds);
	//Checks if any code has been broken.
	void runTestCases(std::vector<CardNameInfo> result);

	SystemMethods* systemMethods;
};

