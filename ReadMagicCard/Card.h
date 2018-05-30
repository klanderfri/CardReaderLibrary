#pragma once
class Card
{
public:
	Card();
	Card(std::wstring fileName, std::wstring cardName);
	Card(std::wstring fileName, std::wstring cardName, int cardType);
	~Card();

	//The full file path of the file containing the card.
	std::wstring FilePath = L"";
	//Tells the file name of the file containing the card.
	std::wstring FileName = L"";
	//The extracted card name.
	std::wstring CardName = L"";
	//Tells how confident the OCR algorithm was of decoding the title.
	int Confidence = 0;
	//Tells the type of the card.
	int CardType = 1;
	//Tells if the reading of the card was successful.
	bool IsSuccessful = false;
};

