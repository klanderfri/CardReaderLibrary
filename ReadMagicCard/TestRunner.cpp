#include "stdafx.h"
#include "TestRunner.h"
#include "TrendLine.h"
#include "MtgCardInfoHelper.h"
#include "boost\algorithm\string.hpp"
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

	vector<CardNameInfo> incorrectResults;
	bool cardTestsSucceded = runCardTests(result, incorrectResults);
	bool sortTestsSucceded = runSortTest();
	bool lineAngleSucceded = testLineAngleCalculation();

	if (cardTestsSucceded && sortTestsSucceded && lineAngleSucceded) {

		systemMethods->SetCommandLineTextColour(Colour::Green);
		wcout << L"All test cases still works!" << endl << endl;
	}
	else {

		systemMethods->SetCommandLineTextColour(Colour::Red);

		//Inform about card identifying trouble.
		if (!cardTestsSucceded) {

			if (incorrectResults.size() == 0) {

				wcout << L"There are missing card test cases!" << endl;
			}
			else {

				wcout << L"There are " + to_wstring(incorrectResults.size()) + L" broken card test cases!" << endl;
				for (CardNameInfo subResult : incorrectResults) {
					wcout << subResult.FileName + L" got '" + subResult.CardName + L"'" << endl;
				}
			}
		}

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

bool TestRunner::runCardTests(vector<CardNameInfo> actualResults, vector<CardNameInfo>& incorrectResults) {

	vector<CardNameInfo> expectedResults = getExpectedCardResult();

	//Check that all test cases are acounted for.
	if (expectedResults.size() != actualResults.size()) { return false; }

	//Edit the result format so it will be comparable.
	sort(expectedResults.begin(), expectedResults.end(), TestRunner::compareByFileName);
	sort(actualResults.begin(), actualResults.end(), TestRunner::compareByFileName);

	//Check that the result corresponds with the expected result.
	bool success = true;
	for (size_t i = 0; i < expectedResults.size(); i++) {

		if (!actualResults[i].IsSuccessful() ||
			!resultsCorresponds(expectedResults[i], actualResults[i])) {

			incorrectResults.push_back(actualResults[i]);
			success = false;
		}
	}

	return success;
}

bool TestRunner::runSortTest() {

	vector<CardNameInfo> cardList = getCardListToSort();
	sort(cardList.begin(), cardList.end(), TestRunner::compareByCardName);

	wstring expectedResult = getExpectedSortResult();
	wstring actualResult;
	for (CardNameInfo card : cardList) {

		actualResult += card.CardName + L"\n";
	}

	bool success = (actualResult == expectedResult);
	return success;
}

bool TestRunner::resultsCorresponds(CardNameInfo expectedResult, CardNameInfo actualResult) {

	wstring exp_Filename = expectedResult.FileName;
	wstring exp_Cardname = expectedResult.CardName;
	wstring act_Filename = actualResult.FileName;
	wstring act_Cardname = actualResult.CardName;

	boost::algorithm::to_lower(exp_Filename);
	boost::algorithm::to_lower(exp_Cardname);
	boost::algorithm::to_lower(act_Filename);
	boost::algorithm::to_lower(act_Cardname);

	bool filenamesAreIdentical = exp_Filename == act_Filename;
	bool cardnamesAreIdentical = exp_Cardname == act_Cardname;

	if (!filenamesAreIdentical) { return false; }
	if (!cardnamesAreIdentical) { return false; }

	return true;
}

bool TestRunner::compareByCardName(CardNameInfo info1, CardNameInfo info2)
{
	return MtgCardInfoHelper::CompareCardNames(info1.CardName, info2.CardName) < 0;
}

bool TestRunner::compareByFileName(CardNameInfo info1, CardNameInfo info2)
{
	//Yeah, I know. We are sorting the filename with the algorithm for comparing card names.
	//But it should be fine as long as the filenames are sorted in the same way as eachother.
	return MtgCardInfoHelper::CompareCardNames(info1.FileName, info2.FileName) < 0;
}

vector<CardNameInfo> TestRunner::getExpectedCardResult() {

	//The test data files, i.e the image files, can be downloaded from a third-party server.
	//Check the CONTRIBUTING.md for information about the adress.

	vector<CardNameInfo> expectedResults {

		CardNameInfo(L"Raptor Companion - Rotated.jpg", L"Raptor Companion"),
		CardNameInfo(L"Enter the Unknown.jpg", L"Enter the Unknown"),
		CardNameInfo(L"Hardy Veteran.jpg", L"Hardy Veteran"),
		CardNameInfo(L"Arterial Flow.jpg", L"Arterial flow"),
		CardNameInfo(L"Ravenous Chupacabra.jpg", L"Ravenous Chupacabra"),
		CardNameInfo(L"Mountain - Foiled.jpg", L"Mountain"),
		CardNameInfo(L"Snubhorn Sentry.jpg", L"Snubhorn Sentry"),
		CardNameInfo(L"Frilled Deathspitter - Foiled.jpg", L"Frilled Deathspitter"),
		CardNameInfo(L"Raptor Companion - Shifted.jpg", L"Raptor Companion"),
		CardNameInfo(L"Buccaneer's Bravado.jpg", L"Buccaneer's Bravado"),
		CardNameInfo(L"Hornswoggle.jpg", L"Hornswoggle"),
		CardNameInfo(L"Evolving Wilds.jpg", L"Evolving Wilds"),
		CardNameInfo(L"Nicol Bolas, Planeswalker.jpg", L"Nicol Bolas, Planeswalker"),
		CardNameInfo(L"Zombie - Token.jpg", L"ZOMBIE"),
		CardNameInfo(L"Saproling - Token.jpg", L"SAPROLING"),
		CardNameInfo(L"Gatstaf Shepard.jpg", L"Gatstaf Shepherd"),
		CardNameInfo(L"Swaggering Corsair.jpg", L"Swaggering Corsair"),
		CardNameInfo(L"Progenitus.jpg", L"Progenitus"),
		CardNameInfo(L"Raptor Companion - Straight.jpg", L"Raptor Companion"),
		CardNameInfo(L"Daring Buccaneer.jpg", L"Daring Buccaneer"),
		CardNameInfo(L"Martyr of Dusk.jpg", L"Martyr of Dusk"),
		CardNameInfo(L"Fathom Fleet Boarder.jpg", L"Fathom Fleet Boarder"),
		CardNameInfo(L"Giltgrove Stalker.jpg", L"Giltgrove Stalker"),
		CardNameInfo(L"Secrets of the Golden City.jpg", L"Secrets of the Golden City"),
		CardNameInfo(L"Dusk Charger.jpg", L"Dusk Charger"),
		CardNameInfo(L"Flamecast Wheel.jpg", L"Flamecast Wheel"),
		CardNameInfo(L"Far Away - Straight.jpg", L"Far // Away"),
		CardNameInfo(L"Far Away - Rotated.jpg", L"Far // Away"),
		CardNameInfo(L"Emblem - Huatli.jpg", L"Emblem"),
		CardNameInfo(L"Forest.jpg", L"Forest"),
		CardNameInfo(L"Island - Foiled.jpg", L"Island"),
		CardNameInfo(L"Pillar of Origins.jpg", L"Pillar of Origins"),
		CardNameInfo(L"Plant - Token.jpg", L"Plant"),
		CardNameInfo(L"Thaumatic Compass.jpg", L"Thaumatic Compass"),
		CardNameInfo(L"Treasure Map.jpg", L"Treasure Map"),
		CardNameInfo(L"Dire Fleet Captain - Foiled.jpg", L"Dire Fleet Captain"),
		CardNameInfo(L"Treasure Cove.jpg", L"Treasure Cove"),
		CardNameInfo(L"Spring Mind.jpg", L"Spring // Mind"),
		CardNameInfo(L"Start Finish.jpg", L"Start // Finish"),
		CardNameInfo(L"Shipwreck Looter - 1 - Straight.jpg", L"Shipwreck Looter"),
		CardNameInfo(L"Shipwreck Looter - 2 - Slightly rotated.jpg", L"Shipwreck Looter"),
		CardNameInfo(L"Shipwreck Looter - 6 - Upside-down.jpg", L"Shipwreck Looter"),
		CardNameInfo(L"Shipwreck Looter - 7 - Very upside-down.jpg", L"Shipwreck Looter"),
		CardNameInfo(L"Impeccable Timing.jpg", L"Impeccable Timing"),
		CardNameInfo(L"Ill-tempered Cyclops.jpg", L"Ill-Tempered Cyclops"),
		CardNameInfo(L"Tah-Crop Elite.jpg", L"Tah-Crop Elite"),
		CardNameInfo(L"Nimble-Blade Khenra.jpg", L"Nimble-Blade Khenra"),
		CardNameInfo(L"Ill-tempered Cyclops - Blurry.jpg", L"Ill-Tempered Cyclops")
	};

	return expectedResults;
}

vector<CardNameInfo> TestRunner::getCardListToSort() {

	vector<CardNameInfo> cardListToSort {

		CardNameInfo(L"", L"Vear"),
		CardNameInfo(L"", L"Buccaneer's Crest"),
		CardNameInfo(L"", L"Jötun Grunt"),
		CardNameInfo(L"", L"Buccaneers Bravado"),
		CardNameInfo(L"", L"Very Fast Raptor"),
		CardNameInfo(L"", L"Dark Martyr"),
		CardNameInfo(L"", L"Sun-Crested Raptor"),
		CardNameInfo(L"", L"Dark"),
		CardNameInfo(L"", L"Jotun Grunt"),
		CardNameInfo(L"", L"Suncrap Gather"),
		CardNameInfo(L"", L"ZOMBIE"),
		CardNameInfo(L"", L"Zonder"),
		CardNameInfo(L"", L"Darker Offering"),
		CardNameInfo(L"", L"Jötun Owl Keeper"),
		CardNameInfo(L"", L"Wear // Tear"),
		CardNameInfo(L"", L"Wear Away"),
		CardNameInfo(L"", L"Buccaneer's Armada"),
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
		L"Jötun Grunt\n"
		L"Jötun Owl Keeper\n"
		L"Suncrap Gather\n"
		L"Sun-Crested Raptor\n"
		L"Vear\n"
		L"Very Fast Raptor\n"
		L"Wear Away\n"
		L"Wear // Tear\n"
		L"ZOMBIE\n"
		L"Zonder\n";

	return result;
}

bool TestRunner::testLineAngleCalculation() {

	TrendLine lineA(2, -3);
	TrendLine lineB(5, 1);
	TrendLine lineC(4, 1);
	TrendLine lineD((1 / (double)2), -(1 / (double)3));
	TrendLine lineE(-1, 0);
	TrendLine lineF(1, 2);
	long double result1 = TrendLine::GetAngleBetweenLines(lineA, lineB); //15.3 degrees
	long double result2 = TrendLine::GetAngleBetweenLines(lineC, lineD); //49.4 degrees
	long double result3 = TrendLine::GetAngleBetweenLines(lineE, lineF); //90.0 degrees

	long double expectedResult1 = -15.255118703057775;
	long double expectedResult2 = 49.398705354995542;
	long double expectedResult3 = -90.000000000000000;

	bool isWorking =
		result1 == expectedResult1 &&
		result2 == expectedResult2 &&
		result3 == expectedResult3;
	return isWorking;
}
