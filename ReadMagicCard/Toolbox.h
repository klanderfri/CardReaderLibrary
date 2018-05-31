#pragma once
#include "Converter.h"
#include "StoreCardProcessingData.h"
#include "CRLibraryHelper.h"
class Toolbox
{
public:
	Toolbox(char* runArguments[], int numberOfRunArguments);
	~Toolbox();
	//Sets the command line text colour to the specified colour.
	void SetCommandLineTextColour(int colour);
	//Resets the command line text colour to the OS default one.
	void ResetCommandLineTextColour();
	//Sets the width of the console window to accommodate a specific amount of characters.
	void SetConsoleWidthInCharacters(int amountOfCharacters);

	//Tells if the program should do debugging such as exporting the images to files.
	bool runDebugging;
	//Tells if the program should run without printing any messages.
	bool runSilent;
	//Tells if the program should read each card in a separate thread.
	bool runParallelized;
	//The file path of a specific image to decode.
	std::wstring filePathToImageToDecode;

	Converter* converter;

	StoreCardProcessingData* resultStorer;

	CRLibraryHelper* cardReader;

private:

	//Converts the input argument to a bool value.
	bool getInputArgumentValue(char* runArguments[], int numberOfRunArguments, int index, bool defaultValue);
	//Gets the input argument as a string.
	std::string getInputArgumentString(char* runArguments[], int index);
	//Checks if an input argument index is within the range of provided arguments.
	bool indexIsWithinRange(int numberOfRunArguments, int index);
};

