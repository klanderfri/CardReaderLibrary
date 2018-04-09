#pragma once
#include "SystemMethods.h"
#include "CardNameInfo.h"
#include <vector>
class TestRunner
{
public:
	TestRunner(SystemMethods* systemMethods);
	~TestRunner();
	
	//Runs tests checking if the result of the card read is as expected, indicating any broken code.
	bool RunTests(std::vector<CardNameInfo> actualResults, std::vector<CardNameInfo>& incorrectResults);

private:
	//Checks if two result pairs corresponds, i.e if the actual result corresponds with the expected result.
	bool resultsCorresponds(CardNameInfo expectedResult, CardNameInfo actualResult);
	//Compares two result pairs by the card name.
	static bool compareByCardName(CardNameInfo info1, CardNameInfo info2);
	
	static bool compareByFileName(CardNameInfo info1, CardNameInfo info2);
	//Gets a vector containing the expected result.
	std::vector<CardNameInfo> getExpectedResult();

	//An object holding OS-specific methods.
	SystemMethods* systemMethods;
};

