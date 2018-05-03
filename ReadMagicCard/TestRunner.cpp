#include "stdafx.h"
#include "TestRunner.h"
#include "CardTestRunner.h"
#include "TrendLine.h"
#include <iostream>

using namespace std;

TestRunner::TestRunner(SystemMethods* systemMethods)
{
	this->systemMethods = systemMethods;
}

TestRunner::~TestRunner()
{
}

void TestRunner::RunTestCases(vector<CardNameInfo> result) {

	CardTestRunner cardTester(systemMethods);

	//Perform tests.
	bool cardTestsSucceded = cardTester.RunTestCases(result);
	bool sortTestsSucceded = runSortTest();
	bool lineAngleSucceded = testLineAngleCalculation();

	//Check the test results.
	if (cardTestsSucceded && sortTestsSucceded && lineAngleSucceded) {

		systemMethods->SetCommandLineTextColour(Colour::Green);
		wcout << L"All test cases still works!" << endl << endl;
	}
	else {

		systemMethods->SetCommandLineTextColour(Colour::Red);

		//Inform about trouble in the sorting.
		if (!sortTestsSucceded) {
			wcout << L"The algorithm for sorting card is broken!" << endl;
		}

		//Inform about trouble in the algorithm calculating angles between lines.
		if (!lineAngleSucceded) {
			wcout << L"The algorithm calculating the angle between two lines is broken!" << endl;
		}

		wcout << endl;
	}

	systemMethods->ResetCommandLineTextColour();
}

bool TestRunner::runSortTest() {

	vector<CardNameInfo> cardList = getCardListToSort();
	sort(cardList.begin(), cardList.end(), CardNameInfo::CompareByCardName);

	wstring expectedResult = getExpectedSortResult();
	wstring actualResult;
	for (CardNameInfo card : cardList) {

		actualResult += card.CardName + L"\n";
	}

	bool success = (actualResult == expectedResult);
	return success;
}

vector<CardNameInfo> TestRunner::getCardListToSort() {

	vector<CardNameInfo> cardListToSort {

		CardNameInfo(L"", L"Buccaneer's Armada"),
		CardNameInfo(L"", L"Buccaneer's Crest"),
		CardNameInfo(L"", L"Buccaneers Bravado"),
		CardNameInfo(L"", L"Dark Martyr"),
		CardNameInfo(L"", L"Dark"),
		CardNameInfo(L"", L"Darker Offering"),
		CardNameInfo(L"", L"J�tun Grunt"),
		CardNameInfo(L"", L"Jotun Grunt"),
		CardNameInfo(L"", L"J�tun Owl Keeper"),
		CardNameInfo(L"", L"Sun-Crested Raptor"),
		CardNameInfo(L"", L"Suncrap Gather"),
		CardNameInfo(L"", L"Vear"),
		CardNameInfo(L"", L"Very Fast Raptor"),
		CardNameInfo(L"", L"Wear // Tear"),
		CardNameInfo(L"", L"Wear Away"),
		CardNameInfo(L"", L"Zombie"),
		CardNameInfo(L"", L"ZONDER")
	};

	return cardListToSort;
}

wstring TestRunner::getExpectedSortResult() {

	wstring result =
		L"Buccaneer's Armada\n"
		L"Buccaneers Bravado\n"
		L"Buccaneer's Crest\n"
		L"Dark\n"
		L"Dark Martyr\n"
		L"Darker Offering\n"
		L"Jotun Grunt\n"
		L"J�tun Grunt\n"
		L"J�tun Owl Keeper\n"
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

bool TestRunner::testLineAngleCalculation() {

	TrendLine lineA(2, -3);
	TrendLine lineB(5, 1);
	TrendLine lineC(4, 1);
	TrendLine lineD((1 / (double)2), -(1 / (double)3));
	TrendLine lineE(-1, 0);
	TrendLine lineF(1, 2);
	TrendLine lineG(0.32, 2);
	TrendLine lineH(-3.125, 3);
	long double result1 = TrendLine::GetAngleBetweenLines(lineA, lineB);
	long double result2 = TrendLine::GetAngleBetweenLines(lineC, lineD);
	long double result3 = TrendLine::GetAngleBetweenLines(lineE, lineF);
	long double result4 = TrendLine::GetAngleBetweenLines(lineG, lineH);

	long double expectedResult1 = -15.255118703057775;
	long double expectedResult2 = 49.398705354995542;
	long double expectedResult3 = -90.000000000000000;
	long double expectedResult4 = 90.000000000000000;

	bool isWorking =
		result1 == expectedResult1 &&
		result2 == expectedResult2 &&
		result3 == expectedResult3 &&
		result4 == expectedResult4;
	return isWorking;
}
