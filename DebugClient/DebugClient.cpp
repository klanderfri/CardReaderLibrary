// DebugClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "CardReaderLibrary.h"

using namespace CRLibrary;
using namespace std;

int main(int argc, char* argv[])
{
	const int MAX_RESULT_LENGTH = 5000;

	//Don't try to handle errors. We're just a debug program.
	if (argc != 3) { return 1; }

	//Get the output folder path.
	char* outputFolderPathPtr = argv[1];
	string outputFolderPathStr = string(outputFolderPathPtr);

	//Get the file path.
	char* fileToReadPtr = argv[2];
	string fileToReadStr = string(fileToReadPtr);

	//Create the run argument.
	string argument = "0;1;1;1;" + outputFolderPathStr + ";" + fileToReadStr + ";";

	//Read the card.
	char* output = new char[MAX_RESULT_LENGTH];
	CRLibrary::ReadCardTitles(argument.c_str(), output, MAX_RESULT_LENGTH);
	string result(output);

    return 0;
}

