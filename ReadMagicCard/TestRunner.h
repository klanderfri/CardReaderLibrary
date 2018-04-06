#pragma once
#include "SystemMethods.h"
#include "CardNameInfo.h"
#include <vector>
#include <string>
class TestRunner
{
public:
	TestRunner(SystemMethods* systemMethods);
	~TestRunner();
	
	//Runs tests checking if the result of the card read is as expected, indicating any broken code.
	bool RunTests(std::vector<CardNameInfo> result);

private:
	//A vector containing an image filename and the name of the card in that image.
	typedef std::vector<std::wstring> FileCardPair;

	//Converts an vector containing the actual results to the same format as the vector contianing the expected result.
	std::vector<FileCardPair> convertActualResultToPair(std::vector<CardNameInfo> result);
	//Checks if two result pairs corresponds, i.e if the actual result corresponds with the expected result.
	bool resultsCorresponds(FileCardPair expectedResult, FileCardPair actualResult);
	//Compares two result pairs by the card name.
	static bool compareByCardName(FileCardPair vector1, FileCardPair vector2);
	//Gets a vector containing the expected result.
	std::vector<FileCardPair> getExpectedResult();

	//An object holding OS-specific methods.
	SystemMethods* systemMethods;
};

