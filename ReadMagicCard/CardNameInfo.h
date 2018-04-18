#pragma once
#include "SystemMethods.h"
#include "CardTitleType.h"
//Class holding information about a card.
class CardNameInfo
{
public:
	CardNameInfo();
	CardNameInfo(std::wstring fileName, std::wstring cardName);
	~CardNameInfo();

	//Checks if we got a confident decode of a MtG title.
	bool IsConfidentTitle(int minimumConfidence);
	//Tells if the algorithm extracting the card name was successful.
	bool IsSuccessful();

	//The name of the file containing the card.
	std::wstring FileName = L"";
	//The extracted card name.
	std::wstring CardName = L"";
	//Tells how confident the OCR algorithm was of decoding the title.
	int Confidence = 0;
	//Tells the type of the card.
	CardTitleType CardType = NormalTitle;
};

