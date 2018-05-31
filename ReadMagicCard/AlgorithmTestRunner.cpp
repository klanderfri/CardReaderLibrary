#include "stdafx.h"
#include "AlgorithmTestRunner.h"
#include "CRLibraryHelper.h"
#include <iostream>
#include <algorithm>

using namespace std;

AlgorithmTestRunner::AlgorithmTestRunner(Toolbox* toolbox)
{
	this->toolbox = toolbox;
}

AlgorithmTestRunner::~AlgorithmTestRunner()
{
}

bool AlgorithmTestRunner::RunTestCases() {

	//Run tests
	bool sortTestsSucceded = runSortTest();

	//Check the test results.
	if (!sortTestsSucceded) {

		toolbox->SetCommandLineTextColour(12);

		//Inform about trouble in the sorting.
		if (!sortTestsSucceded) {
			wcout << L"The algorithm for sorting card is broken!" << endl;
		}

		wcout << endl;
		toolbox->ResetCommandLineTextColour();

		return false;
	}
	
	return true;
}

bool AlgorithmTestRunner::runSortTest() {

	vector<Card> cardList = getCardListToSort();
	sort(cardList.begin(), cardList.end(), compareByCardName);

	wstring expectedResult = getExpectedSortResult();
	wstring actualResult;
	for (Card card : cardList) {

		actualResult += card.CardName + L"\n";
	}

	bool success = (actualResult == expectedResult);
	return success;
}

bool AlgorithmTestRunner::compareByCardName(Card card1, Card card2) {

	Converter converter = Converter();

	string name1 = converter.ToString(card1.CardName);
	string name2 = converter.ToString(card2.CardName);

	bool isInCorrectOrder = CRLibraryHelper().CompareCardNames(name1.c_str(), name2.c_str());
	return isInCorrectOrder;
}

vector<Card> AlgorithmTestRunner::getCardListToSort() {

	vector<Card> cardListToSort{

		Card(L"", L"Buccaneer's Armada"),
		Card(L"", L"Buccaneer's Crest"),
		Card(L"", L"Buccaneers Bravado"),
		Card(L"", L"Dark Martyr"),
		Card(L"", L"Dark"),
		Card(L"", L"Darker Offering"),
		Card(L"", L"Jötun Grunt"),
		Card(L"", L"Jotun Grunt"),
		Card(L"", L"Jötun Owl Keeper"),
		Card(L"", L"Sun-Crested Raptor"),
		Card(L"", L"Suncrap Gather"),
		Card(L"", L"Vear"),
		Card(L"", L"Very Fast Raptor"),
		Card(L"", L"Wear // Tear"),
		Card(L"", L"Wear Away"),
		Card(L"", L"Zombie"),
		Card(L"", L"ZONDER")
	};

	return cardListToSort;
}

wstring AlgorithmTestRunner::getExpectedSortResult() {

	wstring result =
		L"Buccaneer's Armada\n"
		L"Buccaneers Bravado\n"
		L"Buccaneer's Crest\n"
		L"Dark\n"
		L"Dark Martyr\n"
		L"Darker Offering\n"
		L"Jotun Grunt\n"
		L"Jötun Grunt\n"
		L"Jötun Owl Keeper\n"
		L"Suncrap Gather\n"
		L"Sun-Crested Raptor\n"
		L"Vear\n"
		L"Very Fast Raptor\n"
		L"Wear Away\n"
		L"Wear // Tear\n"
		L"Zombie\n"
		L"ZONDER\n";

	return result;
}
