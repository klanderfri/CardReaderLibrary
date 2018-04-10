#pragma once
#include "SystemMethods.h"
#include "CardCollectionReader.h"
#include <chrono>
class CardReaderApplication
{
public:
	//A chrono time point for steady clock.
	typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

	CardReaderApplication();
	~CardReaderApplication();

	//Runs the card reader application.
	int Run(const bool runParallelized, const bool doDebugging);

private:

	//Removes lingering old data from previous runs.
	void removeOldData(SystemMethods* systemMethods);
	//Fetches the file names of the MtG image files.
	std::vector<std::wstring> getMtgImageFileNames(SystemMethods* systemMethods);
	//Creates an collection holding a reader for evere card image.
	CardCollectionReader* createCardReaderCollection(SystemMethods* systemMethods, const std::vector<std::wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging);
	//Resizes the command window to occupy as little space as possible without creating line breakes in its' output texts. 
	void reziseCommandWindow(SystemMethods* systemMethods, size_t numberOfFiles, int lengthOfLongestFilename);
	//Prints a message telling the user how the execution went.
	void printResultMessage(SystemMethods* systemMethods, int numberOfErrors);
	//Reads the title of the card in the provided image files.
	std::vector<CardNameInfo> readAllCards(SystemMethods* systemMethods, const std::vector<std::wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging);
	//Prints a message telling the user that there are to many files to successfully read them all.
	void printToManyFilesMessage();
	//Extracts the time (in milliseconds) that passed between two time points.
	long long getexecutionTime(TimePoint startTime, TimePoint endTime);
	//Prints a message telling the user the length of the execution time.
	void printExecutionTimeMessage(SystemMethods* systemMethods, TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds);
	//Checks if any code has been broken.
	void runTestCases(SystemMethods* systemMethods, std::vector<CardNameInfo> result);
};
