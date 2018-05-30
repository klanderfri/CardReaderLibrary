#pragma once
#include "Toolbox.h"
#include "TimePoint.h"
//Class holding methods printing the messages of the program.
class ApplicationMessages
{
public:
	ApplicationMessages(Toolbox* toolbox);
	~ApplicationMessages();

	//Prints a welcome message for the user.
	void printWelcomeMessage();
	//Prints a message telling the user where the program is looking for card image files.
	void printWorkingFolderMessage(std::wstring mtgFolderPath);
	//Prints a message telling the user how the execution went.
	void printResultMessage(int numberOfErrors);
	//Prints a message telling the user that there are no files to read.
	void printNoImagesMessage();
	//Prints a message telling the user that there are to many files to successfully read them all.
	void printToManyFilesMessage();
	//Prints a message telling the user the length of the execution time.
	void printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds);

private:

	//Pointer to object holding useful data/methods.
	Toolbox* toolbox;
};

