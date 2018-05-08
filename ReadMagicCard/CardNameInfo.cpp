#include "stdafx.h"
#include "CardNameInfo.h"
#include "MtgCardInfoHelper.h"
#include "FileHandling.h"

using namespace std;

CardNameInfo::CardNameInfo()
{
}

CardNameInfo::CardNameInfo(wstring fileName, wstring cardName)
	: CardNameInfo(fileName, cardName, (CardTitleType)(-1))
{
}

CardNameInfo::CardNameInfo(wstring fileName, wstring cardName, CardTitleType cardType)
{
	this->FileName = fileName;
	this->CardName = cardName;

	if (cardType >= NormalTitle) {
		this->CardType = cardType;
	}
}

CardNameInfo::~CardNameInfo()
{
}

bool CardNameInfo::HasGotResult() const {

	return (Confidence > 0 && !CardName.empty());
}

bool CardNameInfo::IsConfidentTitle(int minimumConfidence) const {

	//An empty result cannot be considered a confident title.
	if (!HasGotResult()) { return false; }

	//The Tesseract algorithm needs to be confident with the result otherwise we shouldn't either.
	if (Confidence < minimumConfidence) { return false; }

	//The shortest card name is three letters.
	if (!MtgCardInfoHelper::IsNameLongEnough(CardName)) { return false; }

	//Check for illegal characters.
	if (MtgCardInfoHelper::ContainsInvalidCharacters(CardName)) { return false; }

	return true;
}

bool CardNameInfo::CompareByCardName(CardNameInfo info1, CardNameInfo info2)
{
	return MtgCardInfoHelper::CompareCardNames(info1.CardName, info2.CardName) <= 0;
}

bool CardNameInfo::CompareByFileName(CardNameInfo info1, CardNameInfo info2)
{
	return FileHandling::CompareFilenames(info1.FileName, info2.FileName) <= 0;
}
