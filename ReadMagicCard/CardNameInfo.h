#pragma once
#include <string>
//Class holding information about a card.
class CardNameInfo
{
public:
	CardNameInfo();
	CardNameInfo(std::wstring fileName, std::wstring cardName);
	~CardNameInfo();

	//Tells if the algorithm extracting the card name was successful.
	bool Success = false;
	//The name of the file containing the card.
	std::wstring FileName;
	//The extracted card name.
	std::wstring CardName;
	//Tells how confident the OCR algorithm was of decoding the title.
	int Confidence = 0;
};

