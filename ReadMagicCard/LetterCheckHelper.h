#pragma once
//Class holding methods checking if a letter area actually contains a letter.
class LetterCheckHelper
{
public:
	LetterCheckHelper(int workingCardHeight);
	~LetterCheckHelper();

	//Checks if a specified rotated rectangle would contain a card title letter.
	bool IsLetter(cv::RotatedRect letterArea);

private:

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_CARD_HEIGHT;

	//Gets the maximum X coordinate a title letter may have.
	int getMaxTitleLetterCoordinateX();
	//Gets the minimum X coordinate a title letter may have.
	int getMinTitleLetterCoordinateX();
	//Gets the maximum Y coordinate a title letter may have.
	int getMaxTitleLetterCoordinateY();
	//Gets the minimum Y coordinate a title letter may have.
	int getMinTitleLetterCoordinateY();
	//Check if the letter is within the area containing letters.
	bool isWithinTitleArea(cv::RotatedRect letterArea);
	//Check if it's a i-dot.
	bool isIDot(cv::RotatedRect letterArea);
	//Checks if it's a comma.
	bool isComma(cv::RotatedRect letterArea);
	//Check if the letter is big/small enough to be a letter.
	bool isLetterSize(cv::RotatedRect letterArea);
};
