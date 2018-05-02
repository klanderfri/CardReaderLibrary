#pragma once
#include "SystemMethods.h"
#include "CardNameInfo.h"
#include <vector>
//Class running tests of the program.
class TestRunner
{
public:
	TestRunner(SystemMethods* systemMethods);
	~TestRunner();

	//Runs tests to see if there are any broken code.
	void RunTestCases(std::vector<CardNameInfo> result);

private:
	//Runs tests checking if the result of the card read is as expected, indicating any broken code.
	bool runCardTests(std::vector<CardNameInfo> actualResults, std::vector<CardNameInfo>& incorrectResults);
	//Runs tests checking if the code sorting cards are working as intended.
	bool runSortTest();
	//Tests that the confidence has not degraded.
	bool runConfidenceTest(std::vector<CardNameInfo> actualResults, double expectedAverageConfidence, double& actualAverageConfidence, int expectedLowestConfidence, int& actualLowestConfidence);
	//Checks if two result pairs corresponds, i.e if the actual result corresponds with the expected result.
	bool resultsCorresponds(CardNameInfo expectedResult, CardNameInfo actualResult);
	//Compares two subresults by the card name.
	static bool compareByCardName(CardNameInfo info1, CardNameInfo info2);
	//Compares two subresults by the file name.
	static bool compareByFileName(CardNameInfo info1, CardNameInfo info2);
	//Gets a vector containing the expected result.
	std::vector<CardNameInfo> getExpectedCardResult();
	//Creates a list of cards the sort algorithm must successfully sort.
	std::vector<CardNameInfo> getCardListToSort();
	//Gets a wstring containing the expected sort result.
	std::wstring getExpectedSortResult();
	//Tests the method calculating the angle between two lines.
	bool testLineAngleCalculation();

	//An object holding OS-specific methods.
	SystemMethods* systemMethods;
};

