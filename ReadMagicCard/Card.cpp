#include "stdafx.h"
#include "Card.h"

using namespace std;

Card::Card()
{
}

Card::Card(wstring fileName, wstring cardName)
	: Card(fileName, cardName, -1)
{
}

Card::Card(wstring fileName, wstring cardName, int cardType)
	: Card()
{
	this->FileName = fileName;
	this->CardName = cardName;

	if (cardType >= 1) {
		this->CardType = cardType;
	}
}

Card::~Card()
{
}
