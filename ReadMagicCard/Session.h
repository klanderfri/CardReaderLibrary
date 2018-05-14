#pragma once
#include "SystemMethods.h"
class Session
{
public:
	Session(SystemMethods* systemMethods, char* runArguments[], int numberOfRunArguments);
	~Session();

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_CARD_HEIGHT;

	//Pointer to object handling the system dependent methods.
	SystemMethods* systemMethods;
	//Tells if the program should do debugging such as exporting the images to files.
	bool runDebugging;
	//Tells if the program should run without printing any messages.
	bool runSilent;
	//Tells if the program should read each card in a separate thread.
	bool runParallelized;

private:

	//Converts the input argument to a bool value.
	bool getInputArgumentValue(char* runArguments[], int numberOfRunArguments, int index, bool defaultValue);
};

