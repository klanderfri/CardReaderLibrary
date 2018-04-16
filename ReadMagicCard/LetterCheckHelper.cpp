#include "stdafx.h"
#include "LetterCheckHelper.h"

using namespace std;
using namespace cv;

LetterCheckHelper::LetterCheckHelper(int workingCardHeight)
	: WORKING_CARD_HEIGHT(workingCardHeight)
{
}

LetterCheckHelper::~LetterCheckHelper()
{
}

bool LetterCheckHelper::IsLetter(RotatedRect letterArea) {

	if (!isWithinTitleArea(letterArea)) {
		return false;
	}

	if (isIDot(letterArea)) {
		return true;
	}

	if (isComma(letterArea)) {
		return true;
	}

	if (isDash(letterArea)) {
		return true;
	}

	if (!isLetterSize(letterArea)) {
		return false;
	}

	return true;
}

int LetterCheckHelper::getMaxTitleLetterCoordinateX() {
	return (int)(WORKING_CARD_HEIGHT * 2.477941); //1685
}

int LetterCheckHelper::getMinTitleLetterCoordinateX() {
	return (int)(WORKING_CARD_HEIGHT / 13.6); //50
}

int LetterCheckHelper::getMaxTitleLetterCoordinateY() {
	return (int)(WORKING_CARD_HEIGHT / 4.689655); //145
}

int LetterCheckHelper::getMinTitleLetterCoordinateY() {
	return (int)(WORKING_CARD_HEIGHT / 17.0); //40
}

int LetterCheckHelper::getTitleAreaHeight() {
	return getMaxTitleLetterCoordinateY() - getMinTitleLetterCoordinateY();
}

int LetterCheckHelper::getTitleAreaMiddleCoordinateY() {
	return getMinTitleLetterCoordinateY() + getTitleAreaHeight() / 2;
}

bool LetterCheckHelper::isWithinTitleArea(RotatedRect letterArea) {

	int approximateCoordinateX = (int)(letterArea.center.x - letterArea.size.width / 2);

	if (approximateCoordinateX > getMaxTitleLetterCoordinateX() ||
		approximateCoordinateX < getMinTitleLetterCoordinateX() ||
		letterArea.center.y < getMinTitleLetterCoordinateY() ||
		letterArea.center.y > getMaxTitleLetterCoordinateY()) {

		//To far off to be a letter!
		return false;
	}

	return true;
}

bool LetterCheckHelper::isIDot(RotatedRect letterArea) {

	int iDotMax = (int)(WORKING_CARD_HEIGHT / 34.0); //20
	int iDotMin = (int)(WORKING_CARD_HEIGHT / 68.0); //10

	double minAngle = -50;
	double maxAngle = -40;

	if (letterArea.size.height < iDotMax &&
		letterArea.size.width < iDotMax &&
		letterArea.size.height > iDotMin &&
		letterArea.size.width > iDotMin &&
		letterArea.angle < maxAngle &&
		letterArea.angle> minAngle) {

		//It's probably the dot above the i... Or the area in a, e, g...
		return true;
	}

	return false;
}

bool LetterCheckHelper::isComma(RotatedRect letterArea) {

	if (letterArea.center.y < getTitleAreaMiddleCoordinateY()) { return false; }

	int maxHeight = (int)(WORKING_CARD_HEIGHT / 20); //34
	int minHeight = (int)(WORKING_CARD_HEIGHT / 34); //20
	float height = max(letterArea.size.height, letterArea.size.width); //OpenCV doesn't allways set width and height logical.
	if (height < minHeight || height > maxHeight) { return false; }

	return true;
}

bool LetterCheckHelper::isDash(RotatedRect letterArea) {

	//Check if it is in the middle line of the title.
	int titleHeight = getTitleAreaHeight();
	int middleLine = getTitleAreaMiddleCoordinateY();
	int minY = middleLine - titleHeight / 5;
	int maxY = middleLine + titleHeight / 5;
	if (letterArea.center.y < minY || letterArea.center.y> maxY) {
		return false;
	}

	//Get the height and width.
	float height = letterArea.size.height;
	float width = letterArea.size.width;
	if (letterArea.angle < -45) {
		swap(height, width);
	}

	//Check if it has the form of a dash.
	float sideRelation = height / width;
	if (sideRelation > 0.3 || sideRelation < 0.2) {
		return false;
	}

	//Check if it has the right size.
	int minimumDashHeight = (int)round(WORKING_CARD_HEIGHT / 170); //4
	int minimumDashWidth = (int)round(WORKING_CARD_HEIGHT / 42.5); //16
	int maximumDashHeight = (int)round(WORKING_CARD_HEIGHT / 34); //20
	int maximumDashWidth = (int)round(WORKING_CARD_HEIGHT / 8.5); //80

	if (height < minimumDashHeight ||
		width < minimumDashWidth ||
		height > maximumDashHeight ||
		width > maximumDashWidth) {

		//Wrong size to be a dash.
		return false;
	}

	return true;
}

bool LetterCheckHelper::isLetterSize(RotatedRect letterArea) {

	//Always take the smallest int as width and the biggest as height.
	//This will cause letters like 'm' to "stand up" but that won't be a problem
	//if the max height and max width are the same.
	//Doing this will prevent 't' from "laying down" causing it to miss the min  height limits.
	//That can happen sometimes when the Rotated rect has height/width incorrectly assigned.
	float letterHeight = max(letterArea.size.height, letterArea.size.width);
	float letterWidth = min(letterArea.size.height, letterArea.size.width);

	int minimumLetterHeight = (int)round(WORKING_CARD_HEIGHT / 22.65); //30
	int minimumLetterWidth = (int)round(WORKING_CARD_HEIGHT / 52.3); //13
	int maximumLetterHeight = (int)round(WORKING_CARD_HEIGHT / 5.9); //115
	int maximumLetterWidth = (int)round(WORKING_CARD_HEIGHT / 5.9); //115

	if (letterHeight < minimumLetterHeight ||
		letterWidth < minimumLetterWidth ||
		letterHeight > maximumLetterHeight ||
		letterWidth > maximumLetterWidth) {

		//Wrong size to be a letter.
		return false;
	}

	return true;
}
