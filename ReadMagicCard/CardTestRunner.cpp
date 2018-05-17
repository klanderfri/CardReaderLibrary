#include "stdafx.h"
#include "CardTestRunner.h"
#include "MtgCardInfoHelper.h"
#include <iostream>
#include "boost\algorithm\string.hpp"

using namespace std;

CardTestRunner::CardTestRunner(Session* session) :
	SessionBound(session)
{
}

CardTestRunner::~CardTestRunner()
{
}

bool CardTestRunner::RunTestCases(vector<CardNameInfo> result) {

	bool allTestsWasSuccessful;

	//Indicate errors by using red text colour.
	session->systemMethods->SetCommandLineTextColour(Colour::Red);

	//Check that all test cases are accounted for.
	bool cardTestCasesMissing = (getExpectedCardResult().size() != result.size());
	if (cardTestCasesMissing) {

		wcout << L"There are missing card test cases!" << endl;
		allTestsWasSuccessful = false;
	}
	else {

		//Test the cards.
		vector<CardNameInfo> incorrectNameResults;
		bool cardNameTestsSucceded = runCardNameTests(result, incorrectNameResults);

		//Inform about card identifying trouble.
		if (!cardNameTestsSucceded) {

			wcout << L"There are " + to_wstring(incorrectNameResults.size()) + L" cards with the wrong name!" << endl;
			for (CardNameInfo subResult : incorrectNameResults) {
				wcout << L"\t" + subResult.FileName + L" got '" + subResult.CardName + L"'" << endl;
			}
		}

		//Test the confidence.
		double actualAverageConfidence, expectedAverageConfidence = 83.053571428571431;
		int actualLowestConfidence, expectedLowestConfidence = 78;
		bool confidenceTestSucceded = runConfidenceTest(result, expectedAverageConfidence, actualAverageConfidence, expectedLowestConfidence, actualLowestConfidence);

		//Inform about degraded confidence.
		if (!confidenceTestSucceded) {
			wcout << L"The average confidence of the title text decoding has degraded!" << endl;
			wcout << L"\tAverage confidence is " + to_wstring(actualAverageConfidence) + L", expected at least " + to_wstring(expectedAverageConfidence) + L"." << endl;
			wcout << L"\tLowest confidence is " + to_wstring(actualLowestConfidence) + L", expected at least " + to_wstring(expectedLowestConfidence) + L"." << endl;
		}

		//Test the card type.
		vector<CardNameInfo> incorrectTypeResults;
		bool cardTypeTestsSucceded = runCardTypeTests(result, incorrectTypeResults);

		//Inform about card type trouble.
		if (!cardTypeTestsSucceded) {

			wcout << L"There are " + to_wstring(incorrectTypeResults.size()) + L" cards with the wrong type!" << endl;
			for (CardNameInfo subResult : incorrectTypeResults) {
				wcout << L"\t" + subResult.FileName + L" got enum number '" + to_wstring(subResult.CardType) + L"'" << endl;
			}
		}

		//Check if any test failed.
		allTestsWasSuccessful =
			cardNameTestsSucceded &&
			cardTypeTestsSucceded &&
			confidenceTestSucceded;
	}

	if (!allTestsWasSuccessful) {
		wcout << endl;
	}

	//Reset the consol colour.
	session->systemMethods->ResetCommandLineTextColour();

	return allTestsWasSuccessful;
}

vector<CardNameInfo> CardTestRunner::getExpectedCardResult() {

	//The test data files, i.e the image files, can be downloaded from a third-party server.
	//Check the CONTRIBUTING.md for information about the adress.

	vector<CardNameInfo> expectedResults{

		CardNameInfo(L"Raptor Companion - Rotated.jpg", L"Raptor Companion", NormalTitle),
		CardNameInfo(L"Enter the Unknown.jpg", L"Enter the Unknown", NormalTitle),
		CardNameInfo(L"Hardy Veteran.jpg", L"Hardy Veteran", NormalTitle),
		CardNameInfo(L"Arterial Flow.jpg", L"Arterial flow", NormalTitle),
		CardNameInfo(L"Ravenous Chupacabra.jpg", L"Ravenous Chupacabra", NormalTitle),
		CardNameInfo(L"Mountain - Foiled.jpg", L"Mountain", NormalTitle),
		CardNameInfo(L"Snubhorn Sentry.jpg", L"Snubhorn Sentry", NormalTitle),
		CardNameInfo(L"Frilled Deathspitter - Foiled.jpg", L"Frilled Deathspitter", NormalTitle),
		CardNameInfo(L"Raptor Companion - Shifted.jpg", L"Raptor Companion", NormalTitle),
		CardNameInfo(L"Buccaneer's Bravado.jpg", L"Buccaneer's Bravado", NormalTitle),
		CardNameInfo(L"Hornswoggle.jpg", L"Hornswoggle", NormalTitle),
		CardNameInfo(L"Evolving Wilds.jpg", L"Evolving Wilds", NormalTitle),
		CardNameInfo(L"Nicol Bolas, Planeswalker.jpg", L"Nicol Bolas, Planeswalker", NormalTitle),
		CardNameInfo(L"Zombie - Token.jpg", L"ZOMBIE", Token),
		CardNameInfo(L"Saproling - Token.jpg", L"SAPROLING", Token),
		CardNameInfo(L"Gatstaf Shepard.jpg", L"Gatstaf Shepherd", NormalTitle),
		CardNameInfo(L"Swaggering Corsair.jpg", L"Swaggering Corsair", NormalTitle),
		CardNameInfo(L"Progenitus.jpg", L"Progenitus", NormalTitle),
		CardNameInfo(L"Raptor Companion - Straight.jpg", L"Raptor Companion", NormalTitle),
		CardNameInfo(L"Daring Buccaneer.jpg", L"Daring Buccaneer", NormalTitle),
		CardNameInfo(L"Martyr of Dusk.jpg", L"Martyr of Dusk", NormalTitle),
		CardNameInfo(L"Fathom Fleet Boarder.jpg", L"Fathom Fleet Boarder", NormalTitle),
		CardNameInfo(L"Giltgrove Stalker.jpg", L"Giltgrove Stalker", NormalTitle),
		CardNameInfo(L"Secrets of the Golden City.jpg", L"Secrets of the Golden City", NormalTitle),
		CardNameInfo(L"Dusk Charger.jpg", L"Dusk Charger", NormalTitle),
		CardNameInfo(L"Flamecast Wheel.jpg", L"Flamecast Wheel", NormalTitle),
		CardNameInfo(L"Far Away - Straight.jpg", L"Far // Away", SplitCardTitle),
		CardNameInfo(L"Far Away - Rotated.jpg", L"Far // Away", SplitCardTitle),
		CardNameInfo(L"Emblem - Huatli.jpg", L"Emblem", Emblem),
		CardNameInfo(L"Forest.jpg", L"Forest", NormalTitle),
		CardNameInfo(L"Island - Foiled.jpg", L"Island", NormalTitle),
		CardNameInfo(L"Pillar of Origins.jpg", L"Pillar of Origins", NormalTitle),
		CardNameInfo(L"Plant - Token.jpg", L"Plant", Token),
		CardNameInfo(L"Thaumatic Compass.jpg", L"Thaumatic Compass", NormalTitle),
		CardNameInfo(L"Treasure Map.jpg", L"Treasure Map", NormalTitle),
		CardNameInfo(L"Dire Fleet Captain - Foiled.jpg", L"Dire Fleet Captain", NormalTitle),
		CardNameInfo(L"Treasure Cove.jpg", L"Treasure Cove", TransformedTitle),
		CardNameInfo(L"Spring Mind.jpg", L"Spring // Mind", AkhSplitCardTitle),
		CardNameInfo(L"Start Finish.jpg", L"Start // Finish", AkhSplitCardTitle),
		CardNameInfo(L"Shipwreck Looter - 1 - Straight.jpg", L"Shipwreck Looter", NormalTitle),
		CardNameInfo(L"Shipwreck Looter - 2 - Slightly rotated.jpg", L"Shipwreck Looter", NormalTitle),
		CardNameInfo(L"Shipwreck Looter - 3 - Very rotated.jpg", L"Shipwreck Looter", NormalTitle),
		CardNameInfo(L"Shipwreck Looter - 4 - On side.jpg", L"Shipwreck Looter", NormalTitle),
		CardNameInfo(L"Shipwreck Looter - 5 - Slightly upside-down.jpg", L"Shipwreck Looter", NormalTitle),
		CardNameInfo(L"Shipwreck Looter - 6 - Very upside-down.jpg", L"Shipwreck Looter", NormalTitle),
		CardNameInfo(L"Shipwreck Looter - 7 - Completely Upside-down.jpg", L"Shipwreck Looter", NormalTitle),
		CardNameInfo(L"Impeccable Timing.jpg", L"Impeccable Timing", NormalTitle),
		CardNameInfo(L"Ill-tempered Cyclops.jpg", L"Ill-Tempered Cyclops", NormalTitle),
		CardNameInfo(L"Tah-Crop Elite.jpg", L"Tah-Crop Elite", NormalTitle),
		CardNameInfo(L"Nimble-Blade Khenra.jpg", L"Nimble-Blade Khenra", NormalTitle),
		CardNameInfo(L"Ill-tempered Cyclops - Blurry.jpg", L"Ill-Tempered Cyclops", NormalTitle),
		CardNameInfo(L"Hijack.jpg", L"Hijack", NormalTitle),
		CardNameInfo(L"Pirate's Prize.jpg", L"Pirate's Prize", NormalTitle),
		CardNameInfo(L"Renewed Faith.jpg", L"Renewed Faith", NormalTitle),
		CardNameInfo(L"Spring Mind - Rotated.jpg", L"Spring // Mind", AkhSplitCardTitle),
		CardNameInfo(L"Start Finish - Rotated.jpg", L"Start // Finish", AkhSplitCardTitle)
	};

	return expectedResults;
}

bool CardTestRunner::runCardNameTests(vector<CardNameInfo> actualResults, vector<CardNameInfo>& incorrectResults) {

	return runCardTests(actualResults, incorrectResults, cardNameIsOK);
}

bool CardTestRunner::cardNameIsOK(CardNameInfo expectedResult, CardNameInfo actualResult) {

	return actualResult.HasGotResult() && resultsCorresponds(expectedResult, actualResult);
}

bool CardTestRunner::runCardTypeTests(vector<CardNameInfo> actualResults, vector<CardNameInfo>& incorrectResults) {

	return runCardTests(actualResults, incorrectResults, cardTypeIsOK);
}

bool CardTestRunner::cardTypeIsOK(CardNameInfo expectedResult, CardNameInfo actualResult) {

	return expectedResult.CardType == actualResult.CardType;
}

bool CardTestRunner::runCardTests(vector<CardNameInfo> actualResults, vector<CardNameInfo>& incorrectResults, bool (*resultSanityCheckMethod)(CardNameInfo, CardNameInfo)) {

	vector<CardNameInfo> expectedResults = getExpectedCardResult();

	//Edit the result format so it will be comparable.
	sort(expectedResults.begin(), expectedResults.end(), CardNameInfo::CompareByFileName);
	sort(actualResults.begin(), actualResults.end(), CardNameInfo::CompareByFileName);

	//Check that the result corresponds with the expected result.
	bool success = true;
	for (size_t i = 0; i < expectedResults.size(); i++) {

		bool actualResultIsOK = (*resultSanityCheckMethod)(expectedResults[i], actualResults[i]);

		if (!actualResultIsOK) {

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
