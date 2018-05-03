#include "stdafx.h"
#include "CardTestRunner.h"
#include "MtgCardInfoHelper.h"
#include <iostream>
#include "boost\algorithm\string.hpp"

using namespace std;

CardTestRunner::CardTestRunner(SystemMethods* systemMethods)
{
	this->systemMethods = systemMethods;
}

CardTestRunner::~CardTestRunner()
{
}

bool CardTestRunner::RunTestCases(vector<CardNameInfo> result) {

	//Indicate errors by using red text colour.
	systemMethods->SetCommandLineTextColour(Colour::Red);

	//Check that all test cases are accounted for.
	bool cardTestCasesMissing = (getExpectedCardResult().size() != result.size());
	if (cardTestCasesMissing) {

		wcout << L"There are missing card test cases!" << endl;
		return false;
	}

	//Test the cards.
	vector<CardNameInfo> incorrectResults;
	bool cardTestsSucceded = runCardTests(result, incorrectResults);

	//Inform about card identifying trouble.
	if (!cardTestsSucceded) {

		wcout << L"There are " + to_wstring(incorrectResults.size()) + L" broken card test cases!" << endl;
		for (CardNameInfo subResult : incorrectResults) {
			wcout << subResult.FileName + L" got '" + subResult.CardName + L"'" << endl;
		}
		wcout << endl;
	}

	//Test the confidence.
	double actualAverageConfidence, expectedAverageConfidence = 83.3125;
	int actualLowestConfidence, expectedLowestConfidence = 78;
	bool confidenceTestSucceded = runConfidenceTest(result, expectedAverageConfidence, actualAverageConfidence, expectedLowestConfidence, actualLowestConfidence);

	//Inform about degraded confidence.
	if (!confidenceTestSucceded) {
		wcout << L"The average confidence of the title text decoding has degraded!" << endl;
		wcout << L"\tAverage confidence is " + to_wstring(actualAverageConfidence) + L", expected at least " + to_wstring(expectedAverageConfidence) + L"." << endl;
		wcout << L"\tLowest confidence is " + to_wstring(actualLowestConfidence) + L", expected at least " + to_wstring(expectedLowestConfidence) + L"." << endl << endl;
	}

	//Reset the consol colour.
	systemMethods->ResetCommandLineTextColour();

	//Check if any test failed.
	bool allTestWasSuccessful =
		cardTestsSucceded &&
		confidenceTestSucceded;

	return allTestWasSuccessful;
}

vector<CardNameInfo> CardTestRunner::getExpectedCardResult() {

	//The test data files, i.e the image files, can be downloaded from a third-party server.
	//Check the CONTRIBUTING.md for information about the adress.

	vector<CardNameInfo> expectedResults{

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

bool CardTestRunner::runCardTests(vector<CardNameInfo> actualResults, vector<CardNameInfo>& incorrectResults) {

	vector<CardNameInfo> expectedResults = getExpectedCardResult();

	//Edit the result format so it will be comparable.
	sort(expectedResults.begin(), expectedResults.end(), CardNameInfo::CompareByFileName);
	sort(actualResults.begin(), actualResults.end(), CardNameInfo::CompareByFileName);

	//Check that the result corresponds with the expected result.
	bool success = true;
	for (size_t i = 0; i < expectedResults.size(); i++) {

		if (!actualResults[i].IsConfidentTitle() ||
			!resultsCorresponds(expectedResults[i], actualResults[i])) {

			incorrectResults.push_back(actualResults[i]);
			success = false;
		}
	}

	return success;
}

bool CardTestRunner::runConfidenceTest(vector<CardNameInfo> actualResults, double expectedAverageConfidence, double& actualAverageConfidence, int expectedLowestConfidence, int& actualLowestConfidence) {

	if (actualResults.empty()) {

		actualLowestConfidence = 0;
		actualAverageConfidence = 0;
	}
	else {

		int totalConfidence = 0;
		actualLowestConfidence = 100;

		for (CardNameInfo card : actualResults) {

			totalConfidence += card.Confidence;

			if (card.Confidence < actualLowestConfidence) {
				actualLowestConfidence = card.Confidence;
			}
		}

		actualAverageConfidence = (double)totalConfidence / actualResults.size();
	}

	bool success =
		(actualAverageConfidence >= expectedAverageConfidence) &&
		(actualLowestConfidence >= expectedLowestConfidence);
	return success;
}

bool CardTestRunner::resultsCorresponds(CardNameInfo expectedResult, CardNameInfo actualResult) {

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
