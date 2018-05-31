#include "stdafx.h"
#include "Toolbox.h"
#include <windows.h>

using namespace std;

Toolbox::Toolbox(char* runArguments[], int numberOfRunArguments)
{
	this->converter = new Converter();
	this->resultStorer = new StoreCardProcessingData();
	this->cardReader = new CRLibraryHelper();

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
		this->filePathToImageToDecode = converter->ToWString(fileArg);
	}
}

Toolbox::~Toolbox()
{
	delete converter;
}

bool Toolbox::getInputArgumentValue(char* runArguments[], int numberOfRunArguments, int index, bool defaultValue) {

	if (!indexIsWithinRange(numberOfRunArguments, index)) { return defaultValue; }

	string argument = getInputArgumentString(runArguments, index);
	bool value = converter->ToBool(argument);

	return value;
}

string Toolbox::getInputArgumentString(char* runArguments[], int index) {

	char* argumentPtr = runArguments[index];
	string argumentStr = string(argumentPtr);

	return argumentStr;
}

bool Toolbox::indexIsWithinRange(int numberOfRunArguments, int index) {

	return index < numberOfRunArguments;
}

void Toolbox::SetCommandLineTextColour(int colour) {

	//Implemented as suggested at:
	//https://stackoverflow.com/a/4053879/1997617

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, colour);
}

void Toolbox::ResetCommandLineTextColour() {

	SetCommandLineTextColour(7);
}

void Toolbox::SetConsoleWidthInCharacters(int amountOfCharacters) {

	//Implemented as suggested at
	//https://stackoverflow.com/a/29197930/1997617
	HANDLE Hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX Font = { sizeof(Font) };
	GetCurrentConsoleFontEx(Hout, FALSE, &Font);

	//Implemented as suggested at
	//https://stackoverflow.com/a/21238849/1997617
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //Stores the console's current dimensions.
	double phi = (1 + sqrt(5)) / 2;
	int width = amountOfCharacters * Font.dwFontSize.X + 33; //Lets hope the user doesn't change the font size!
	int height = (int)round(width / phi); //Use the golden ratio! :-D
	MoveWindow(console, r.left, r.top, width, height, TRUE);
}
