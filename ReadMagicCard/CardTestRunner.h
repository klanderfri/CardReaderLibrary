#pragma once
#include "SessionBound.h"
#include "CardNameInfo.h"
//Class running tests of the card reading results.
class CardTestRunner :
	public SessionBound
{
public:
	CardTestRunner(Session* session);
	~CardTestRunner();

	//Runs tests to see if there are any broken code.
	bool RunTestCases(std::vector<CardNameInfo> result);

private:

	//Gets a vector containing the expected result.
	std::vector<CardNameInfo> getExpectedCardResult();
	//Runs tests checking if the card names are correct.
	bool runCardNameTests(std::vector<CardNameInfo> actualResults, std::vector<CardNameInfo>& incorrectResults);
	//Checks if the card name in the result is correct.
	static bool cardNameIsOK(CardNameInfo expectedResult, CardNameInfo actualResult);
	//Runs tests checking if the card types are correct.
	bool runCardTypeTests(std::vector<CardNameInfo> actualResults, std::vector<CardNameInfo>& incorrectResults);
	//Checks if the card type in the result is correct.
	static bool cardTypeIsOK(CardNameInfo expectedResult, CardNameInfo actualResult);
	//Runs tests checking if the result of the card read is as expected, indicating any broken code.
	bool runCardTests(std::vector<CardNameInfo> actualResults, std::vector<CardNameInfo>& incorrectResults, bool(*resultSanityCheckMethod)(CardNameInfo, CardNameInfo));
	//Tests that the confidence has not degraded.
	bool runConfidenceTest(std::vector<CardNameInfo> actualResults, double expectedAverageConfidence, double& actualAverageConfidence, int expectedLowestConfidence, int& actualLowestConfidence);
	//Checks if two result pairs corresponds, i.e if the actual result corresponds with the expected result.
	static bool resultsCorresponds(CardNameInfo expectedResult, CardNameInfo actualResult);
};

