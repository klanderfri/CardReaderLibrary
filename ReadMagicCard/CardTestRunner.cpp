#include "stdafx.h"
#include "CardTestRunner.h"
#include "FileHandling.h"
#include <iostream>
#include "boost\algorithm\string.hpp"

using namespace std;

CardTestRunner::CardTestRunner(Toolbox* toolbox)
{
	this->toolbox = toolbox;
}

CardTestRunner::~CardTestRunner()
{
}

bool CardTestRunner::RunTestCases(vector<Card> result) {

	bool allTestsWasSuccessful;

	//Indicate errors by using red text colour.
	toolbox->SetCommandLineTextColour(12);

	//Check that all test cases are accounted for.
	bool cardTestCasesMissing = (getExpectedCardResult().size() != result.size());
	if (cardTestCasesMissing) {

		wcout << L"There are missing card test cases!" << endl;
		allTestsWasSuccessful = false;
	}
	else {

		//Test the cards.
		vector<Card> incorrectNameResults;
		bool cardNameTestsSucceded = runCardNameTests(result, incorrectNameResults);

		//Inform about card identifying trouble.
		if (!cardNameTestsSucceded) {

			wcout << L"There are " + to_wstring(incorrectNameResults.size()) + L" cards with the wrong name!" << endl;
			for (Card subResult : incorrectNameResults) {
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
		vector<Card> incorrectTypeResults;
		bool cardTypeTestsSucceded = runCardTypeTests(result, incorrectTypeResults);

		//Inform about card type trouble.
		if (!cardTypeTestsSucceded) {

			wcout << L"There are " + to_wstring(incorrectTypeResults.size()) + L" cards with the wrong type!" << endl;
			for (Card subResult : incorrectTypeResults) {
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
	toolbox->ResetCommandLineTextColour();

	return allTestsWasSuccessful;
}

vector<Card> CardTestRunner::getExpectedCardResult() {

	//The test data files, i.e the image files, can be downloaded from a third-party server.
	//Check the CONTRIBUTING.md for information about the adress.

	vector<Card> expectedResults{

		Card(L"Raptor Companion - Rotated.jpg", L"Raptor Companion", 1),
		Card(L"Enter the Unknown.jpg", L"Enter the Unknown", 1),
		Card(L"Hardy Veteran.jpg", L"Hardy Veteran", 1),
		Card(L"Arterial Flow.jpg", L"Arterial flow", 1),
		Card(L"Ravenous Chupacabra.jpg", L"Ravenous Chupacabra", 1),
		Card(L"Mountain - Foiled.jpg", L"Mountain", 1),
		Card(L"Snubhorn Sentry.jpg", L"Snubhorn Sentry", 1),
		Card(L"Frilled Deathspitter - Foiled.jpg", L"Frilled Deathspitter", 1),
		Card(L"Raptor Companion - Shifted.jpg", L"Raptor Companion", 1),
		Card(L"Buccaneer's Bravado.jpg", L"Buccaneer's Bravado", 1),
		Card(L"Hornswoggle.jpg", L"Hornswoggle", 1),
		Card(L"Evolving Wilds.jpg", L"Evolving Wilds", 1),
		Card(L"Nicol Bolas, Planeswalker.jpg", L"Nicol Bolas, Planeswalker", 1),
		Card(L"Zombie - Token.jpg", L"ZOMBIE", 8),
		Card(L"Saproling - Token.jpg", L"SAPROLING", 8),
		Card(L"Gatstaf Shepard.jpg", L"Gatstaf Shepherd", 1),
		Card(L"Swaggering Corsair.jpg", L"Swaggering Corsair", 1),
		Card(L"Progenitus.jpg", L"Progenitus", 1),
		Card(L"Raptor Companion - Straight.jpg", L"Raptor Companion", 1),
		Card(L"Daring Buccaneer.jpg", L"Daring Buccaneer", 1),
		Card(L"Martyr of Dusk.jpg", L"Martyr of Dusk", 1),
		Card(L"Fathom Fleet Boarder.jpg", L"Fathom Fleet Boarder", 1),
		Card(L"Giltgrove Stalker.jpg", L"Giltgrove Stalker", 1),
		Card(L"Secrets of the Golden City.jpg", L"Secrets of the Golden City", 1),
		Card(L"Dusk Charger.jpg", L"Dusk Charger", 1),
		Card(L"Flamecast Wheel.jpg", L"Flamecast Wheel", 1),
		Card(L"Far Away - Straight.jpg", L"Far // Away", 2),
		Card(L"Far Away - Rotated.jpg", L"Far // Away", 2),
		Card(L"Emblem - Huatli.jpg", L"Emblem", 7),
		Card(L"Forest.jpg", L"Forest", 1),
		Card(L"Island - Foiled.jpg", L"Island", 1),
		Card(L"Pillar of Origins.jpg", L"Pillar of Origins", 1),
		Card(L"Plant - Token.jpg", L"Plant", 8),
		Card(L"Thaumatic Compass.jpg", L"Thaumatic Compass", 1),
		Card(L"Treasure Map.jpg", L"Treasure Map", 1),
		Card(L"Dire Fleet Captain - Foiled.jpg", L"Dire Fleet Captain", 1),
		Card(L"Treasure Cove.jpg", L"Treasure Cove", 4),
		Card(L"Spring Mind.jpg", L"Spring // Mind", 3),
		Card(L"Start Finish.jpg", L"Start // Finish", 3),
		Card(L"Shipwreck Looter - 1 - Straight.jpg", L"Shipwreck Looter", 1),
		Card(L"Shipwreck Looter - 2 - Slightly rotated.jpg", L"Shipwreck Looter", 1),
		Card(L"Shipwreck Looter - 3 - Very rotated.jpg", L"Shipwreck Looter", 1),
		Card(L"Shipwreck Looter - 4 - On side.jpg", L"Shipwreck Looter", 1),
		Card(L"Shipwreck Looter - 5 - Slightly upside-down.jpg", L"Shipwreck Looter", 1),
		Card(L"Shipwreck Looter - 6 - Very upside-down.jpg", L"Shipwreck Looter", 1),
		Card(L"Shipwreck Looter - 7 - Completely Upside-down.jpg", L"Shipwreck Looter", 1),
		Card(L"Impeccable Timing.jpg", L"Impeccable Timing", 1),
		Card(L"Ill-tempered Cyclops.jpg", L"Ill-Tempered Cyclops", 1),
		Card(L"Tah-Crop Elite.jpg", L"Tah-Crop Elite", 1),
		Card(L"Nimble-Blade Khenra.jpg", L"Nimble-Blade Khenra", 1),
		Card(L"Ill-tempered Cyclops - Blurry.jpg", L"Ill-Tempered Cyclops", 1),
		Card(L"Hijack.jpg", L"Hijack", 1),
		Card(L"Pirate's Prize.jpg", L"Pirate's Prize", 1),
		Card(L"Renewed Faith.jpg", L"Renewed Faith", 1),
		Card(L"Spring Mind - Rotated.jpg", L"Spring // Mind", 3),
		Card(L"Start Finish - Rotated.jpg", L"Start // Finish", 3)
	};

	return expectedResults;
}

bool CardTestRunner::runCardNameTests(vector<Card> actualResults, vector<Card>& incorrectResults) {

	return runCardTests(actualResults, incorrectResults, cardNameIsOK);
}

bool CardTestRunner::cardNameIsOK(Card expectedResult, Card actualResult) {

	return actualResult.IsSuccessful && resultsCorresponds(expectedResult, actualResult);
}

bool CardTestRunner::runCardTypeTests(vector<Card> actualResults, vector<Card>& incorrectResults) {

	return runCardTests(actualResults, incorrectResults, cardTypeIsOK);
}

bool CardTestRunner::cardTypeIsOK(Card expectedResult, Card actualResult) {

	return expectedResult.CardType == actualResult.CardType;
}

bool CardTestRunner::runCardTests(vector<Card> actualResults, vector<Card>& incorrectResults, bool (*resultSanityCheckMethod)(Card, Card)) {

	vector<Card> expectedResults = getExpectedCardResult();

	//Edit the result format so it will be comparable.
	sort(expectedResults.begin(), expectedResults.end(), compareByFileName);
	sort(actualResults.begin(), actualResults.end(), compareByFileName);

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

bool CardTestRunner::runConfidenceTest(vector<Card> actualResults, double expectedAverageConfidence, double& actualAverageConfidence, int expectedLowestConfidence, int& actualLowestConfidence) {

	if (actualResults.empty()) {

		actualLowestConfidence = 0;
		actualAverageConfidence = 0;
	}
	else {

		int totalConfidence = 0;
		actualLowestConfidence = 100;

		for (Card card : actualResults) {

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

bool CardTestRunner::resultsCorresponds(Card expectedResult, Card actualResult) {

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

bool CardTestRunner::compareByFileName(Card card1, Card card2)
{
	return FileHandling::CompareFilenames(card1.FileName, card2.FileName) <= 0;
}
