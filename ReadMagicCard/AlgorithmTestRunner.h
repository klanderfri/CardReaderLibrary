#pragma once
#include "SessionBound.h"
#include "CardNameInfo.h"
//Class running tests of the generic algorithms in the program.
class AlgorithmTestRunner :
	public SessionBound
{
public:
	AlgorithmTestRunner(Session* session);
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
};

