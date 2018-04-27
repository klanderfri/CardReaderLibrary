#pragma once
#include "TrendLine.h"
//Class holding methods checking if a letter area actually contains a letter.
class LetterCheckHelper
{
public:
	LetterCheckHelper(int workingCardHeight, TrendLine textCenterLine);
	~LetterCheckHelper();

	//Checks if a specified rotated rectangle would contain a card title letter.
	bool IsLetter(cv::RotatedRect letterArea);
	//Checks a specified rotated rectangle would contain a character or character part that looks liek a dot.
	bool IsDotLikeCharacter(cv::RotatedRect letterArea);

private:

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_CARD_HEIGHT;

	//Gets the maximum X coordinate a title letter may have.
	int getMaxTitleLetterCoordinateX();
	//Gets the minimum X coordinate a title letter may have.
	int getMinTitleLetterCoordinateX();
	//Gets the maximum Y coordinate a title letter may have.
	int getMaxTitleLetterCoordinateY(double x);
	//Gets the minimum Y coordinate a title letter may have.
	int getMinTitleLetterCoordinateY(double x);
	//Gets the height of the title area.
	int getTitleAreaHeight();
	//Gets the Y coordinate of a line cutting the title area horizontal in two.
	int getTitleAreaMiddleCoordinateY(double x);
	//Check if the letter is within the area containing letters.
	bool isWithinTitleArea(cv::RotatedRect letterArea);
	//Checks if it's a i-dot.
	bool isIDot(cv::RotatedRect letterArea);
	//Checks if it's a comma.
	bool isComma(cv::RotatedRect letterArea);
	//Checks if it's a dash (-).
	bool isDash(cv::RotatedRect letterArea);
	//Check if the letter is big/small enough to be a letter.
	bool isLetterSize(cv::RotatedRect letterArea);

	//The approximal line going through the center of the text.
	TrendLine textCenterLine;
};
