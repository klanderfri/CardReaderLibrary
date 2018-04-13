#include "stdafx.h"
#include "TestRunner.h"
#include "MtgCardInfoHelper.h"
#include "boost\algorithm\string.hpp"

using namespace std;

TestRunner::TestRunner(SystemMethods* systemMethods)
{
	this->systemMethods = systemMethods;
}

TestRunner::~TestRunner()
{
}

bool TestRunner::RunTests(vector<CardNameInfo> actualResults, vector<CardNameInfo>& incorrectResults) {

	vector<CardNameInfo> expectedResults = getExpectedResult();

	//Check that all test cases are acounted for.
	if (expectedResults.size() != actualResults.size()) { return false; }

	//Edit the result format so it will be comparable.
	sort(expectedResults.begin(), expectedResults.end(), TestRunner::compareByFileName);
	sort(actualResults.begin(), actualResults.end(), TestRunner::compareByFileName);

	//Check that the result corresponds with the expected result.
	bool success = true;
	for (size_t i = 0; i < expectedResults.size(); i++) {

		if (!actualResults[i].Success ||
			!resultsCorresponds(expectedResults[i], actualResults[i])) {

			incorrectResults.push_back(actualResults[i]);
			success = false;
		}
	}

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
	//But it should be fine as long as they are sorted in the same way.
	return MtgCardInfoHelper::CompareCardNames(info1.FileName, info2.FileName) < 0;
}

vector<CardNameInfo> TestRunner::getExpectedResult() {

	//The test data files, i.e the image files, can be downloaded from a third-party server.
	//Check the ReadMe for information about the adress.

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
		CardNameInfo(L"Treasure Cove.jpg", L"Treasure Cove")
	};

	return expectedResults;
}
