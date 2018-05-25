#include "stdafx.h"
#include "Session.h"

using namespace std;

Session::Session(SystemMethods* systemMethods, char* runArguments[], int numberOfRunArguments)
	: WORKING_CARD_HEIGHT(680)
{
	this->systemMethods = systemMethods;
	this->imageHelper = new ImageHelper();
	this->cardInfo = new MtgCardInfoHelper();

	//Running the program silent will prevent it from doing
	//outputs in the command line window.
	this->runSilent = getInputArgumentValue(runArguments, numberOfRunArguments, 1, false);
	//Running the program parallelized will read the cards
	//quicker but makes it harder to debug.
	this->runParallelized = getInputArgumentValue(runArguments, numberOfRunArguments, 2, true);
	//Enable debug if you want the program to output images and
	//text showing the different steps in the card identification.
	this->runDebugging = getInputArgumentValue(runArguments, numberOfRunArguments, 3, false);

	//Extract the file path of a specific image to decode.
	int fileArgumentIndex = 4;
	if (indexIsWithinRange(numberOfRunArguments, fileArgumentIndex)) {

		string fileArg = getInputArgumentString(runArguments, fileArgumentIndex);
		this->filePathToImageToDecode = systemMethods->ToWString(fileArg);
	}
}

Session::~Session()
{
	//Go memory! Be FREEEE!!
	delete systemMethods;
	delete imageHelper;
	delete cardInfo;
}

bool Session::getInputArgumentValue(char* runArguments[], int numberOfRunArguments, int index, bool defaultValue) {

	if (!indexIsWithinRange(numberOfRunArguments, index)) { return defaultValue; }

	string argument = getInputArgumentString(runArguments, index);
	bool value = systemMethods->ToBool(argument);

	return value;
}

string Session::getInputArgumentString(char* runArguments[], int index) {

	char* argumentPtr = runArguments[index];
	string argumentStr = string(argumentPtr);

	return argumentStr;
}

bool Session::indexIsWithinRange(int numberOfRunArguments, int index) {

	return index < numberOfRunArguments;
}