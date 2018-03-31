#include "stdafx.h"
#include "OcvTitleExtractor.h"
#include "OcvLetterCheckHelper.h"
#include "OcvMtgCardInfoHelper.h"
#include "OcvSaveImage.h"

using namespace cv;
using namespace std;

OcvTitleExtractor::OcvTitleExtractor(wstring imageFileName, Mat originalImageData, OcvSystemDependencyClass* systemMethods, bool doDebugging)
	: OcvSectionExtractor(imageFileName, originalImageData, systemMethods, doDebugging)
{
}

OcvTitleExtractor::~OcvTitleExtractor()
{
}

bool OcvTitleExtractor::ExtractTitle(Mat& outImage) {

	int amountOfGauss = (int)(originalImageData.size().height / 18.1818);
	amountOfGauss = errorProtectGaussAmount(amountOfGauss);

	//Make the title grey scale.
	outImage = OcvImageHelper::ToGreyImage(originalImageData);

	OcvImageHelper::ResizeImage(outImage, outImage, (int)(WORKING_CARD_HEIGHT / 3.67));

	//Blur the image to smoothing it out.
	GaussianBlur(outImage, outImage, Size(amountOfGauss, amountOfGauss), 0, 0);

	//Threshold the image to get the important pixels.
	int thresh = 120; //Default: 120
	threshold(outImage, outImage, thresh, 255, THRESH_BINARY);

	//Extract a clean image containing the title.
	bool success = getTitleText(outImage, outImage);

	return success;
}

int OcvTitleExtractor::errorProtectGaussAmount(int amountOfGauss) {

	if (amountOfGauss < 1) { return 1; }

	if (amountOfGauss % 2 == 0) {

		if (amountOfGauss == INT_MAX) {
			return INT_MAX - 1;
		}
		return amountOfGauss + 1;
	}

	return amountOfGauss;
}

bool OcvTitleExtractor::getTitleText(const Mat titleImage, Mat& textImage) {

	int thresh = 120; //Default: 120

	Contours contours;
	Hierarchy hierarchy;
	Mat canny;

	Canny(titleImage, canny, thresh, thresh * 2, 3);
	findContours(canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	LetterAreas letters = getPossibleLetterAreas(contours);
	letters = filterOutNoise(letters);
	letters = filterOutTransformSymbol(letters);
	letters = filterOutManaCost(letters);

	if (letters.size() < (size_t)OcvMtgCardInfoHelper::LettersInShortestCardName()) { return false; }

	Contour combinedLetterContorus;
	Mat onlyLettersBound;
	for (size_t i = 0; i < letters.size(); i++) {

		LetterArea area = letters[i];
		Contour letterContour = area.contour;
		RotatedRect letterBox = area.box;

		combinedLetterContorus.insert(combinedLetterContorus.end(), letterContour.begin(), letterContour.end());

		if (doDebugging) {

			onlyLettersBound = onlyLettersBound.empty() ? titleImage : onlyLettersBound;

			//Draw the area.
			onlyLettersBound = OcvImageHelper::DrawLimits(onlyLettersBound, letterBox, Rect(), letterContour);

			//Draw the center.
			int radius = (int)(WORKING_CARD_HEIGHT / 226.5); //3
			Scalar colour = Scalar(255, 0, 0); //Blue
			onlyLettersBound = OcvImageHelper::DrawCenterPoint(onlyLettersBound, letterBox.center, colour, radius);
		}
	}

	//Something is wrong if there are no letters.
	if (combinedLetterContorus.empty()) {

		return false;
	}

	RotatedRect textArea = minAreaRect(combinedLetterContorus);

	//Store result for debugging.
	if (doDebugging) {
		OcvSaveImage::SaveImageData(systemMethods, onlyLettersBound, systemMethods->AddToEndOfFilename(imageFileName, L"_LetterRectangles"), L"7 - Only Title Letters");
		Mat possibleTitleArea = OcvImageHelper::DrawLimits(titleImage, textArea, Rect(), combinedLetterContorus);
		OcvSaveImage::SaveImageData(systemMethods, possibleTitleArea, systemMethods->AddToEndOfFilename(imageFileName, L"_TitleRectangle"), L"8 - Possible Title Area");
	}

	//Rotate the image so it is straight (as much as possible at least).
	Rect2f straightTextArea;
	if (textArea.size.height > textArea.size.width) {
		swap(textArea.size.height, textArea.size.width);
	}
	OcvImageHelper::StraightenUpImage(titleImage, textImage, textArea, straightTextArea, false);

	//Make black text on white background.
	OcvImageHelper::SetBackgroundByInverting(textImage, false);

	//Cut out the title text.
	OcvImageHelper::CropImageWithSolidBorder(textImage, textImage, straightTextArea, 10);

	//Store result for debugging.
	if (doDebugging) {
		OcvSaveImage::SaveImageData(systemMethods, textImage, imageFileName, L"9 - Title Text");
	}

	return true;
}

LetterAreas OcvTitleExtractor::getPossibleLetterAreas(Contours contours) {

	LetterAreas possibleLetterAreas;

	for (size_t i = 0; i < contours.size(); i++) {

		Contour letterContour = ContoursConvexHull({ contours[i] });
		RotatedRect letterBox = minAreaRect(letterContour);

		LetterArea area;
		area.contour = letterContour;
		area.box = letterBox;

		possibleLetterAreas.push_back(area);
	}

	sort(possibleLetterAreas.begin(), possibleLetterAreas.end(), LetterArea::CompareLetterAreaByXAscending);

	return possibleLetterAreas;
}

LetterAreas OcvTitleExtractor::filterOutNoise(LetterAreas lettersToFilter) {

	OcvLetterCheckHelper letterCheck(WORKING_CARD_HEIGHT);
	LetterAreas letters;

	for (size_t i = 0; i < lettersToFilter.size(); i++)
	{
		LetterArea letter = lettersToFilter[i];

		if (letterCheck.IsLetter(letter.box)) {

			letters.push_back(letter);
		}
	}

	return letters;
}

LetterAreas OcvTitleExtractor::filterOutManaCost(LetterAreas lettersToFilter) {

	LetterAreas letters;
	RotatedRect lastLetterBox;
	bool hasFoundNameManaGap = false;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		LetterArea letter = lettersToFilter[i];

		//Check if there is a wide distance between the centers.
		//Since we have sorted the letters by the center X coordinate,
		//all subsequent countours will be mana symbols or noise.
		if (hasWideCenterDistance(letter.box, lastLetterBox)) {
			break;
		}

		lastLetterBox = letter.box;
		letters.push_back(letter);
	}

	return letters;
}

LetterAreas OcvTitleExtractor::filterOutTransformSymbol(LetterAreas lettersToFilter) {

	//This method works for 'Gatstaf Shepard' but probably not for transform
	//cards from later setts since they have a different transform symbol.

	//If the title contains a transform symbol then the first four letters
	//will be parts of that symbol.
	size_t amountOfTransformLetters = 4;

	if (lettersToFilter.size() < amountOfTransformLetters) { return lettersToFilter; }

	RotatedRect lastLetterBox;
	float biggestCenterDistance = 0;
	bool hasOneRotated = false;
	for (size_t i = 0; i < amountOfTransformLetters; i++) {

		LetterArea letter = lettersToFilter[i];

		if (i == 0) {
			lastLetterBox = letter.box;
			continue;
		}

		if (letter.box.angle < -30 && letter.box.angle > -60) {
			hasOneRotated = true;
		}

		float xDistance = abs(lastLetterBox.center.x - letter.box.center.x);
		float yDistance = abs(lastLetterBox.center.y - letter.box.center.y);
		float distance = sqrt(xDistance*xDistance + yDistance * yDistance);
		biggestCenterDistance = max(biggestCenterDistance, distance);
	}

	int allowedDistance = (int)(WORKING_CARD_HEIGHT / 136); //5

	if (biggestCenterDistance < allowedDistance && hasOneRotated) {

		LetterAreas::const_iterator first = lettersToFilter.begin() + amountOfTransformLetters;
		LetterAreas::const_iterator last = lettersToFilter.end();
		return LetterAreas(first, last);
	}

	return lettersToFilter;
}

bool OcvTitleExtractor::hasWideCenterDistance(RotatedRect letterArea, RotatedRect lastLetterArea) {

	//The first letter probably isn't a mana symbol.
	if (!isInitialized(lastLetterArea)) { return false; }

	int wideDistance = (int)(WORKING_CARD_HEIGHT / 6.5); //104
	float distance = letterArea.center.x - lastLetterArea.center.x;

	bool isWideSpace = distance > wideDistance;
	return isWideSpace;
}

bool OcvTitleExtractor::isInitialized(RotatedRect rectangleToCheck) {

	float sum = abs(rectangleToCheck.angle)
		+ abs(rectangleToCheck.center.x)
		+ abs(rectangleToCheck.center.y)
		+ abs(rectangleToCheck.size.area());

	return sum != 0;
}
