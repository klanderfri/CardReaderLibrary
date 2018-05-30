#pragma once
#include "Toolbox.h"
#include "Card.h"
//Class running tests of the card reading results.
class CardTestRunner
{
public:
	CardTestRunner(Toolbox* toolbox);
	~CardTestRunner();

	//Runs tests to see if there are any broken code.
	bool RunTestCases(std::vector<Card> result);

private:

	//Gets a vector containing the expected result.
	std::vector<Card> getExpectedCardResult();
	//Runs tests checking if the card names are correct.
	bool runCardNameTests(std::vector<Card> actualResults, std::vector<Card>& incorrectResults);
	//Checks if the card name in the result is correct.
	static bool cardNameIsOK(Card expectedResult, Card actualResult);
	//Runs tests checking if the card types are correct.
	bool runCardTypeTests(std::vector<Card> actualResults, std::vector<Card>& incorrectResults);
	//Checks if the card type in the result is correct.
	static bool cardTypeIsOK(Card expectedResult, Card actualResult);
	//Runs tests checking if the result of the card read is as expected, indicating any broken code.
	bool runCardTests(std::vector<Card> actualResults, std::vector<Card>& incorrectResults, bool(*resultSanityCheckMethod)(Card, Card));
	//Tests that the confidence has not degraded.
	bool runConfidenceTest(std::vector<Card> actualResults, double expectedAverageConfidence, double& actualAverageConfidence, int expectedLowestConfidence, int& actualLowestConfidence);
	//Checks if two result pairs corresponds, i.e if the actual result corresponds with the expected result.
	static bool resultsCorresponds(Card expectedResult, Card actualResult);
	//Compares two card info set by the file name. Returns true if they are in the right order.
	static bool compareByFileName(Card card1, Card card2);

	//Pointer to object holding useful data/methods.
	Toolbox* toolbox;
};

