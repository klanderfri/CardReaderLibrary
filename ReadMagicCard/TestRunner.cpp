#include "stdafx.h"
#include "TestRunner.h"
#include "OcvMtgCardInfoHelper.h"
#include "boost\algorithm\string.hpp"

using namespace std;

TestRunner::TestRunner(OcvSystemDependencyClass* systemMethods)
{
	this->systemMethods = systemMethods;
}

TestRunner::~TestRunner()
{
}

bool TestRunner::RunTests(vector<CardNameInfo> result) {

	vector<FileCardPair> expectedResults = getExpectedResult();

	//Check for things that, even before the result is analysed, indicate broken code.
	if (expectedResults.size() != result.size()) { return false; }
	for (CardNameInfo info : result) {
		if (!info.Success) { return false; }
	}

	//Edit the result format so it will be comparable.
	vector<FileCardPair> actualResults = convertActualResultToPair(result);
	sort(expectedResults.begin(), expectedResults.end(), TestRunner::compareByCardName);
	sort(actualResults.begin(), actualResults.end(), TestRunner::compareByCardName);

	//Check that the result corresponds with the expected result.
	for (size_t i = 0; i < expectedResults.size(); i++) {

		if (!resultsCorresponds(expectedResults[i], actualResults[i])) { return false; }
	}
	return true;
}

vector<TestRunner::FileCardPair> TestRunner::convertActualResultToPair(vector<CardNameInfo> result) {

	vector<FileCardPair> actualResults;
	for (CardNameInfo info : result) {

		FileCardPair subresult{ info.FileName, info.CardName };
		actualResults.push_back(subresult);
	}

	return actualResults;
}

bool TestRunner::resultsCorresponds(FileCardPair expectedResult, FileCardPair actualResult) {

	wstring exp_Filename = expectedResult[0];
	wstring exp_Cardname = expectedResult[1];
	wstring act_Filename = actualResult[0];
	wstring act_Cardname = actualResult[1];

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

bool TestRunner::compareByCardName(FileCardPair pair1, FileCardPair pair2)
{
	wstring firstName = pair1[1];
	wstring secondName = pair2[1];

	return OcvMtgCardInfoHelper::CompareCardNames(firstName, secondName) < 0;
}

vector<TestRunner::FileCardPair> TestRunner::getExpectedResult() {

	vector<FileCardPair> expectedResults{

		FileCardPair{ L"Raptor Companion - Rotated.jpg", L"Raptor Companion" },
		FileCardPair{ L"Enter the Unknown.jpg", L"Enter the Unknown" },
		FileCardPair{ L"Hardy Veteran.jpg", L"Hardy Veteran" },
		FileCardPair{ L"Arterial Flow.jpg", L"Arterial flow" },
		FileCardPair{ L"Ravenous Chupacabra.jpg", L"Ravenous Chupacabra" },
		FileCardPair{ L"Mountain - Foiled.jpg", L"Mountain" },
		FileCardPair{ L"Snubhorn Sentry.jpg", L"Snubhorn Sentry" },
		FileCardPair{ L"Frilled Deathspitter - Foiled.jpg", L"Frilled Deathspitter" },
		FileCardPair{ L"Raptor Companion - Shifted.jpg", L"Raptor Companion" },
		FileCardPair{ L"Buccaneer's Bravado.jpg", L"Buccaneer's Bravado" },
		FileCardPair{ L"Hornswoggle.jpg", L"Hornswoggle" },
		FileCardPair{ L"Evolving Wilds.jpg", L"Evolving Wilds" },
		FileCardPair{ L"Nicol Bolas, Planeswalker.jpg", L"Nicol Bolas, Planeswalker" },
		FileCardPair{ L"Zombie - Token.jpg", L"ZOMBIE" },
		FileCardPair{ L"Saproling - Token.jpg", L"SAPROLING" },
		FileCardPair{ L"Gatstaf Shepard.jpg", L"Gatstaf Shepherd" },
		FileCardPair{ L"Swaggering Corsair.jpg", L"Swaggering Corsair" },
		FileCardPair{ L"Progenitus.jpg", L"Progenitus" },
		FileCardPair{ L"Raptor Companion - Straight.jpg", L"Raptor Companion" },
		FileCardPair{ L"Daring Buccaneer.jpg", L"Daring Buccaneer" },
		FileCardPair{ L"Martyr of Dusk.jpg", L"Martyr of Dusk" },
		FileCardPair{ L"Fathom Fleet Boarder.jpg", L"Fathom Fleet Boarder" },
		FileCardPair{ L"Giltgrove Stalker.jpg", L"Giltgrove Stalker" },
		FileCardPair{ L"Secrets of the Golden City.jpg", L"Secrets of the Golden City" },
		FileCardPair{ L"Dusk Charger.jpg", L"Dusk Charger" },
		FileCardPair{ L"Flamecast Wheel.jpg", L"Flamecast Wheel" }
	};

	return expectedResults;
}
