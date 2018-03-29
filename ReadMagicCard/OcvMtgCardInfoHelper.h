#pragma once
//Class holding methods giving information about the MtG card environment.
class OcvMtgCardInfoHelper
{
public:
	OcvMtgCardInfoHelper();
	~OcvMtgCardInfoHelper();

	//Gives the amount of letters in the shortest MtG card name.
	static int LettersInShortestCardName();
	//Gets the area of the card that normally contains the title section.
	static cv::Rect GetNormalTitleSectionBox(cv::Size cardSize);

private:

	//Gets the section specified by the factors.
	static cv::Rect getSectionBox(cv::Size cardSize, double verticalFactor, double horizontalFactor, double heightFactor);
};
