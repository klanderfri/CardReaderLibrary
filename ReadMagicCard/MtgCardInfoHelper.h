#pragma once
//Class holding methods giving information about the MtG card environment.
class MtgCardInfoHelper
{
public:
	MtgCardInfoHelper();
	~MtgCardInfoHelper();

	//Gives the amount of letters in the shortest card name.
	static int LettersInShortestCardName();
	//Checks if a card name is long enough top actually be a card name.
	static bool IsNameLongEnough(const std::wstring title);
	//Gets the area of the card that normally contains the title section.
	static cv::Rect GetNormalTitleSectionBox(cv::Size cardSize);
	//Gets the area of the card that contains the title section on split cards (assumed that the card is cropped to look like a normal card).
	static cv::Rect GetSplitTitleSectionBox(cv::Size cardSize);
	//Gets the area of the card that contains the title section on Amonkhet split cards (assumed that the card is cropped to look like a normal card).
	static cv::Rect GetAmonkhetSplitTitleSectionBox(cv::Size cardSize);
	//Compares two card names. Negative if 1st name comes first. Positive if 2nd name comes first. Zero if the names are equal.
	static double CompareCardNames(std::wstring name1, std::wstring name2);
	//Removes characters, that are not relevant for sorting, from a card name.
	static std::wstring RemoveCharactersNotRelevantForNameSorting(const std::wstring cardName);
	//Checks if there are any illeagal characters in the title.
	static bool ContainsInvalidCharacters(const std::wstring title);
	//Gets the characters that are allowed in a card name.
	static std::vector<wchar_t> GetAllowedCharacters();
	//Gets the non-letter characters that are allowed in a card name.
	static std::vector<wchar_t> GetAllowedNonLetterCharacters();
	//Gets the characters that are ignored when sorting/comparing card names.
	static std::vector<wchar_t> GetAllowedNotRelevantForSortingCharacters();
	//Checks if a card is an emblem.
	static bool IsEmblem(const std::wstring title);

private:

	//Gets the section specified by the factors.
	static cv::Rect getSectionBox(cv::Size cardSize, double xCoordinateFactor, double yCoordinateFactor, double widthFactor, double heightFactor);
};
