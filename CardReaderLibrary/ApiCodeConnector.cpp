#include "stdafx.h"
#include "ApiCodeConnector.h"
#include "CardReaderLibrary.h"
#include "WindowsMethods.h"
#include "MtgCardInfoHelper.h"
#include "CardCollectionReader.h"
#include "CardReadInput.h"
#include <string>

using namespace std;

ApiCodeConnector::ApiCodeConnector()
{
}

ApiCodeConnector::~ApiCodeConnector()
{
}

string ApiCodeConnector::ReadCardTitles(char const* input) {

	string result;
	Session session(input);

	try {

		CardCollectionReader reader(&session);

		for (wstring fullCardPath : session.inputData->fullFilePathsOfImagesToDecode) {

			reader.AddCard(fullCardPath);
		}

		vector<CardNameInfo> cards = reader.ExtractCardNames();
		result = convertCardsToString(&session, cards);
	}
	catch (exception ex) {

		result = convertErrorToString(ex);
	}

	return result;
}

string ApiCodeConnector::convertCardsToString(Session* session, vector<CardNameInfo> cards) {

	string result = "";

	for (CardNameInfo card : cards) {

		result +=
			session->systemMethods->ToString(card.FilePath) + ";" +
			session->systemMethods->ToString(card.CardName) + ";" +
			to_string((int)card.CardType) + ";" +
			to_string(card.Confidence) + ";" +
			to_string((int)card.HasGotResult()) + ";";
	}

	return result;
}

string ApiCodeConnector::convertErrorToString(exception ex) {

	return "ERROR: " + string(ex.what());
}

bool ApiCodeConnector::CompareCardNames(char const* cardName1, char const* cardName2) {

	WindowsMethods methods = WindowsMethods();

	wstring name1 = methods.ToWString(string(cardName1));
	wstring name2 = methods.ToWString(string(cardName2));

	return MtgCardInfoHelper::CompareCardNames(name1, name2) <= 0;
}

int ApiCodeConnector::GetMaxCardAmount() {

	return CardCollectionReader::MaxSize();
}
