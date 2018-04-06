#pragma once
//Class holding methods giving information about the MtG card environment.
class MtgCardInfoHelper
{
public:
	MtgCardInfoHelper();
	~MtgCardInfoHelper();

	//Gives the amount of letters in the shortest MtG card name.
	static int LettersInShortestCardName();
	//Gets the area of the card that normally contains the title section.
	static cv::Rect GetNormalTitleSectionBox(cv::Size cardSize);
	//Gets the area of the card that contains the title section on split cards (assumed that the card is rotated to have the text horizontal).
	static cv::Rect GetSplitTitleSectionBox(cv::Size cardSize);
	//Compares two card names. Negative if 1st name comes first. Positive if 2nd name comes first. Zero if the names are equal.
	static double CompareCardNames(std::wstring name1, std::wstring name2);
	//Removes characters, that are not relevant for sorting, from a card name.
	static void RemoveCharactersNotRelevantForNameSorting(const std::wstring cardName, std::wstring& result);

private:

	//Gets the section specified by the factors.
	static cv::Rect getSectionBox(cv::Size cardSize, double xCoordinateFactor, double yCoordinateFactor, double widthFactor, double heightFactor);
};
