#pragma once
#include "OcvSystemDependencyClass.h"
#include "OcvCardCollectionReader.h"
#include <chrono>
class OcvCardReaderApplication
{
public:
	//A chrono time point for steady clock.
	typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

	OcvCardReaderApplication();
	~OcvCardReaderApplication();

	//Runs the card reader application.
	int Run(const bool runParallelized, const bool doDebugging);

private:

	//Removes lingering old data from previous runs.
	void removeOldData(OcvSystemDependencyClass* systemMethods);
	//Fetches the file names of the MtG image files.
	std::vector<std::wstring> getMtgImageFileNames(OcvSystemDependencyClass* systemMethods);
	//Creates an collection holding a reader for evere card image.
	OcvCardCollectionReader* createCardReaderCollection(OcvSystemDependencyClass* systemMethods, const std::vector<std::wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging);
	//Resizes the command window to occupy as little space as possible without creating line breakes in its' output texts. 
	void reziseCommandWindow(OcvSystemDependencyClass* systemMethods, size_t numberOfFiles, int lengthOfLongestFilename);
	//Prints a message telling the user how the execution went.
	void printResultMessage(int numberOfErrors);
	//Reads the title of the card in the provided image files.
	void readAllCards(OcvSystemDependencyClass* systemMethods, const std::vector<std::wstring> filenamesOfImages, const bool runParallelized, const bool doDebugging);
	//Prints a message telling the user that there are to many files to successfully read them all.
	void printToManyFilesMessage();
	//Extracts the time (in milliseconds) that passed between two time points.
	long long getexecutionTime(TimePoint startTime, TimePoint endTime);
	//Prints a message telling the user the length of the execution time.
	void printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds);
};

