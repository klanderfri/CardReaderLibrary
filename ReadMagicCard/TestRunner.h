#pragma once
#include "SystemMethods.h"
#include "CardNameInfo.h"
//Class running tests of the program.
class TestRunner
{
public:
	TestRunner(SystemMethods* systemMethods);
	~TestRunner();

	//Runs tests to see if there are any broken code.
	void RunTestCases(std::vector<CardNameInfo> result);

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

