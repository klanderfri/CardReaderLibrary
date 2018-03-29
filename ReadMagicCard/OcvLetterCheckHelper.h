#pragma once
//Class holding methods checking if a letter area actually contains a letter.
class OcvLetterCheckHelper
{
public:
	OcvLetterCheckHelper(int workingCardHeight);
	~OcvLetterCheckHelper();

	//Checks if a specified rotated rectangle would contain a card title letter.
	bool IsLetter(cv::RotatedRect letterArea);

private:

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_CARD_HEIGHT;

	//Check if the letter is within the area containing letters.
	bool isWithinTitleArea(cv::RotatedRect letterArea);
	//Check if its a i-dot.
	bool isIDot(cv::RotatedRect letterArea);
	//Check if the letter is big/small enough to be a letter.
	bool isLetterSize(cv::RotatedRect letterArea);
};
