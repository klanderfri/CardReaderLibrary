#pragma once
#include "SessionBound.h"
#include "TimePoint.h"
//Class holding methods printing the messages of the program.
class ApplicationMessages : 
	public SessionBound
{
public:
	ApplicationMessages(Session* session);
	~ApplicationMessages();

	//Prints a welcome message for the user.
	void printWelcomeMessage();
	//Prints a message telling the user where the program is looking for card image files.
	void printWorkingFolderMessage(std::wstring mtgFolderPath);
	//Prints a message telling the user that the result of the read is being saved to disk.
	void printSavingResultsToDiskMessage();
	//Prints a message telling the user that the result of the read has been saved to disk.
	void printResultsHasBeenSavedToDiskMessage(std::wstring pathToResultFolder);
	//Prints a message telling the user how the execution went.
	void printResultMessage(int numberOfErrors);
	//Prints a message telling the user that there are no files to read.
	void printNoImagesMessage();
	//Prints a message telling the user that there are to many files to successfully read them all.
	void printToManyFilesMessage();
	//Prints a message telling the user the length of the execution time.
	void printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds);
};

