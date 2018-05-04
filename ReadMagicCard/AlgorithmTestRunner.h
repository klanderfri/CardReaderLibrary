#pragma once
#include "CardNameInfo.h"
class AlgorithmTestRunner
{
public:
	AlgorithmTestRunner(SystemMethods* systemMethods);
	~AlgorithmTestRunner();

	//Runs tests to see if there are any broken code.
	bool RunTestCases();

private:

	//Runs tests checking if the code sorting cards are working as intended.
	bool runSortTest();
	//Creates a list of cards the sort algorithm must successfully sort.
	std::vector<CardNameInfo> getCardListToSort();
	//Gets a wstring containing the expected sort result.
	std::wstring getExpectedSortResult();
	//Tests the method calculating the angle between two lines.
	bool testLineAngleCalculation();

	//An object holding OS-specific methods.
	SystemMethods* systemMethods;
};

