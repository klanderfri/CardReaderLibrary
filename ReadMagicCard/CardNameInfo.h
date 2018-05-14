#pragma once
#include "CardTitleType.h"
//Class holding information about a card.
class CardNameInfo
{
public:
	CardNameInfo();
	CardNameInfo(std::wstring fileName, std::wstring cardName);
	CardNameInfo(std::wstring fileName, std::wstring cardName, CardTitleType cardType);
	~CardNameInfo();

	//Checks if there is a result.
	bool HasGotResult() const;
	//Checks if we got a confident decode of a MtG title.
	bool IsConfidentTitle(int minimumConfidence) const;
	//Compares two card info sets by the card name. Returns true if they are in the right order.
	static bool CompareByCardName(CardNameInfo info1, CardNameInfo info2);
	//Compares two card info set by the file name. Returns true if they are in the right order.
	static bool CompareByFileName(CardNameInfo info1, CardNameInfo info2);

	//The name of the file containing the card.
	std::wstring FileName = L"";
	//The extracted card name.
	std::wstring CardName = L"";
	//Tells how confident the OCR algorithm was of decoding the title.
	int Confidence = 0;
	//Tells the type of the card.
	CardTitleType CardType = NormalTitle;
	//The OCR-prepared images containing the card name.
	std::vector<cv::Mat> OcrTitleImages;
	//Tells if the title background of the original image is so dark it causes the background of a binary image to be black.
	bool HasDarkTitleBackground = false;
	//An image holding the card and only the card.
	cv::Mat ExtractedCardImage;
};

