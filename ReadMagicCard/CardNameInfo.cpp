#include "stdafx.h"
#include "CardNameInfo.h"

using namespace std;

CardNameInfo::CardNameInfo()
{
}

CardNameInfo::CardNameInfo(wstring fileName, wstring cardName)
{
	this->FileName = fileName;
	this->CardName = cardName;
}

CardNameInfo::~CardNameInfo()
{
}
