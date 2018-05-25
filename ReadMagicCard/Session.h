#pragma once
#include "SystemMethods.h"
#include "ImageHelper.h"
//Class for the session of card reading.
class Session
{
public:
	Session(SystemMethods* systemMethods, char* runArguments[], int numberOfRunArguments);
	~Session();

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_CARD_HEIGHT;

	//Pointer to object handling the system dependent methods.
	SystemMethods* systemMethods;
	//Pointer to object containing image methods.
	ImageHelper* imageHelper;
	//Tells if the program should do debugging such as exporting the images to files.
	bool runDebugging;
	//Tells if the program should run without printing any messages.
	bool runSilent;
	//Tells if the program should read each card in a separate thread.
	bool runParallelized;
	//The file path of a specific image to decode.
	std::wstring filePathToImageToDecode;

private:

	//Converts the input argument to a bool value.
	bool getInputArgumentValue(char* runArguments[], int numberOfRunArguments, int index, bool defaultValue);
	//Gets the input argument as a string.
	std::string getInputArgumentString(char* runArguments[], int index);
	//Checks if an input argument index is within the range of provided arguments.
	bool indexIsWithinRange(int numberOfRunArguments, int index);
};

