#include "stdafx.h"
#include "OcvLetterCheckHelper.h"

using namespace std;
using namespace cv;

OcvLetterCheckHelper::OcvLetterCheckHelper(int workingCardHeight)
	: WORKING_CARD_HEIGHT(workingCardHeight)
{
}

OcvLetterCheckHelper::~OcvLetterCheckHelper()
{
}

bool OcvLetterCheckHelper::IsLetter(RotatedRect letterArea) {

	if (!isWithinTitleArea(letterArea)) {
		return false;
	}

	if (isIDot(letterArea)) {
		return true;
	}

	if (!isLetterSize(letterArea)) {
		return false;
	}

	return true;
}

bool OcvLetterCheckHelper::isWithinTitleArea(RotatedRect letterArea) {

	int approximateCoordinateX = (int)(letterArea.center.x - letterArea.size.width / 2);

	int maxCoordinateX = (int)(WORKING_CARD_HEIGHT * 2.477941); //1685
	int minCoordinateX = (int)(WORKING_CARD_HEIGHT / 13.6); //50
	int minCoordinateY = (int)(WORKING_CARD_HEIGHT / 17.0); //40
	int maxCoordinateY = (int)(WORKING_CARD_HEIGHT / 4.689655); //145

	if (approximateCoordinateX > maxCoordinateX ||
		approximateCoordinateX < minCoordinateX ||
		letterArea.center.y < minCoordinateY ||
		letterArea.center.y > maxCoordinateY) {

		//To far off to be a letter!
		return false;
	}

	return true;
}

bool OcvLetterCheckHelper::isIDot(RotatedRect letterArea) {

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

bool OcvLetterCheckHelper::isLetterSize(RotatedRect letterArea) {

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
