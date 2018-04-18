#include "stdafx.h"
#include "CardNameInfo.h"
#include "MtgCardInfoHelper.h"

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

bool CardNameInfo::IsConfidentTitle(int minimumConfidence) {

	//First of, we need to have a successful extraction of the title.
	if (!IsSuccessful()) { return false; }

	//The Tesseract algorithm needs to be confident with the result otherwise we shouldn't either.
	if (Confidence < minimumConfidence) { return false; }

	//The shortest card name is three letters.
	if (!MtgCardInfoHelper::IsNameLongEnough(CardName)) { return false; }

	//Check for illegal characters.
	if (MtgCardInfoHelper::ContainsInvalidCharacters(CardName)) { return false; }

	return true;
}

bool CardNameInfo::IsSuccessful() {

	if (Confidence == 0) { return false; }
	if (CardName == L"") { return false; }

	return true;
}
