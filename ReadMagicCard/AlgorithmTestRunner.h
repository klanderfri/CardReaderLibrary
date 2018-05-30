#pragma once
#include "Toolbox.h"
#include "Card.h"
//Class running tests of the generic algorithms in the program.
class AlgorithmTestRunner
{
public:
	AlgorithmTestRunner(Toolbox* toolbox);
	~AlgorithmTestRunner();

	//Runs tests to see if there are any broken code.
	bool RunTestCases();

private:

	//Runs tests checking if the code sorting cards are working as intended.
	bool runSortTest();

	static bool compareByCardName(Card card1, Card card2);
	//Creates a list of cards the sort algorithm must successfully sort.
	std::vector<Card> getCardListToSort();
	//Gets a wstring containing the expected sort result.
	std::wstring getExpectedSortResult();

	//Pointer to object holding useful data/methods.
	Toolbox* toolbox;
};

