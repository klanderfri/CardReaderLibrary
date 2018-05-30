#pragma once
#include "Session.h"
#include "CardNameInfo.h"
class ApiCodeConnector
{
public:
	ApiCodeConnector();
	~ApiCodeConnector();

	std::string ReadCardTitles(char const* input);
	bool CompareCardNames(char const* cardName1, char const* cardName2);
	int GetMaxCardAmount();

private:

	std::string convertCardsToString(Session* session,std::vector<CardNameInfo> cards);
	std::string convertErrorToString(std::exception ex);
};

