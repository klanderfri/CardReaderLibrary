#pragma once
#include "TimePoint.h"
#include "SystemMethods.h"
class ApplicationMessages
{
public:
	ApplicationMessages(SystemMethods * systemMethods, const bool runSilent);
	~ApplicationMessages();

	//Prints a welcome message for the user.
	void printWelcomeMessage();
	//Prints a message telling the user where the program is looking for card image files.
	void printWorkingFolderMessage(std::wstring mtgFolderPath);
	//Prints a message telling the user how the execution went.
	void printResultMessage(int numberOfErrors, std::wstring pathToResultFile);
	//Prints a message telling the user that there are no files to read.
	void printNoImagesMessage();
	//Prints a message telling the user that there are to many files to successfully read them all.
	void printToManyFilesMessage();
	//Prints a message telling the user the length of the execution time.
	void printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds);

private:

	//Pointer to object handling the system dependent methods.
	SystemMethods * systemMethods;
	//Telling if the program should run without any prints to the command line.
	const bool runSilent;
};

