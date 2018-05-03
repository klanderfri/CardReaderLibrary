#pragma once
#include "CardNameInfo.h"
class CardTestRunner
{
public:
	CardTestRunner(SystemMethods* systemMethods);
	~CardTestRunner();

	//Runs tests to see if there are any broken code.
	bool RunTestCases(std::vector<CardNameInfo> result);

private:

	//Gets a vector containing the expected result.
	std::vector<CardNameInfo> getExpectedCardResult();
	//Runs tests checking if the result of the card read is as expected, indicating any broken code.
	bool runCardTests(std::vector<CardNameInfo> actualResults, std::vector<CardNameInfo>& incorrectResults);
	//Tests that the confidence has not degraded.
	bool runConfidenceTest(std::vector<CardNameInfo> actualResults, double expectedAverageConfidence, double& actualAverageConfidence, int expectedLowestConfidence, int& actualLowestConfidence);
	//Checks if two result pairs corresponds, i.e if the actual result corresponds with the expected result.
	bool resultsCorresponds(CardNameInfo expectedResult, CardNameInfo actualResult);

	//An object holding OS-specific methods.
	SystemMethods* systemMethods;
};

