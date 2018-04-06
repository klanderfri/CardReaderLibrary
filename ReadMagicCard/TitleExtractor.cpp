#include "stdafx.h"
#include "TitleExtractor.h"
#include "LetterCheckHelper.h"
#include "MtgCardInfoHelper.h"
#include "SaveOcvImage.h"

using namespace cv;
using namespace std;

TitleExtractor::TitleExtractor(wstring imageFileName, Mat originalImageData, SystemMethods* systemMethods, bool doDebugging)
	: SectionExtractor(imageFileName, originalImageData, systemMethods, doDebugging)
{
}

TitleExtractor::~TitleExtractor()
{
}

bool TitleExtractor::ExtractTitle(Mat& outImage) {

	int amountOfGauss = (int)(originalImageData.size().height / 18.1818);
	amountOfGauss = errorProtectGaussAmount(amountOfGauss);

	//Make the title grey scale.
	outImage = ImageHelper::ToGreyImage(originalImageData);

	ImageHelper::ResizeImage(outImage, outImage, (int)(WORKING_CARD_HEIGHT / 3.67));

	//Blur the image to smoothing it out.
	GaussianBlur(outImage, outImage, Size(amountOfGauss, amountOfGauss), 0, 0);

	//Threshold the image to get the important pixels.
	int thresh = 120; //Default: 120
	threshold(outImage, outImage, thresh, 255, THRESH_BINARY);

	//Extract a clean image containing the title.
	bool success = getTitleText(outImage, outImage);

	return success;
}

int TitleExtractor::errorProtectGaussAmount(int amountOfGauss) {

	if (amountOfGauss < 1) { return 1; }

	if (amountOfGauss % 2 == 0) {

		if (amountOfGauss == INT_MAX) {
			return INT_MAX - 1;
		}
		return amountOfGauss + 1;
	}

	return amountOfGauss;
}

bool TitleExtractor::getTitleText(const Mat titleImage, Mat& textImage) {

	int thresh = 120; //Default: 120

	Contours contours;
	Hierarchy hierarchy;
	Mat canny;

	Canny(titleImage, canny, thresh, thresh * 2, 3);
	findContours(canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	LetterAreas letters = getPossibleLetterAreas(contours);
	letters = filterOutDuplicates(letters);
	letters = filterOutNoise(letters);
	letters = filterOutTransformSymbol(letters);
	letters = filterOutLetterHoles(letters);
	letters = filterOutManaCost(letters);

	if (letters.size() < (size_t)MtgCardInfoHelper::LettersInShortestCardName()) { return false; }

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
			onlyLettersBound = ImageHelper::DrawLimits(onlyLettersBound, letterBox, Rect(), letterContour);

			//Draw the center.
			int radius = (int)(WORKING_CARD_HEIGHT / 226.5); //3
			Scalar colour = Scalar(255, 0, 0); //Blue
			onlyLettersBound = ImageHelper::DrawCenterPoint(onlyLettersBound, letterBox.center, colour, radius);
		}
	}

	//Something is wrong if there are no letters.
	if (combinedLetterContorus.empty()) {

		return false;
	}

	RotatedRect textArea = minAreaRect(combinedLetterContorus);

	//Store result for debugging.
	if (doDebugging) {
		SaveOcvImage::SaveImageData(systemMethods, onlyLettersBound, systemMethods->AddToEndOfFilename(imageFileName, L"_LetterRectangles"), L"7 - Only Title Letters");
		Mat possibleTitleArea = ImageHelper::DrawLimits(titleImage, textArea, Rect(), combinedLetterContorus);
		SaveOcvImage::SaveImageData(systemMethods, possibleTitleArea, systemMethods->AddToEndOfFilename(imageFileName, L"_TitleRectangle"), L"8 - Possible Title Area");
	}

	//Rotate the image so it is straight (as much as possible at least).
	Rect2f straightTextArea;
	if (textArea.size.height > textArea.size.width) {
		swap(textArea.size.height, textArea.size.width);
	}
	ImageHelper::StraightenUpImage(titleImage, textImage, textArea, straightTextArea, false);

	//Make black text on white background.
	ImageHelper::SetBackgroundByInverting(textImage, false);

	//Cut out the title text.
	ImageHelper::CropImageWithSolidBorder(textImage, textImage, straightTextArea, 10);

	//Store result for debugging.
	if (doDebugging) {
		SaveOcvImage::SaveImageData(systemMethods, textImage, imageFileName, L"9 - Title Text");
	}

	return true;
}

LetterAreas TitleExtractor::getPossibleLetterAreas(Contours contours) {

	LetterAreas possibleLetterAreas;

	for (size_t i = 0; i < contours.size(); i++) {

		Contour letterContour = ContoursConvexHull({ contours[i] });
		RotatedRect letterBox = minAreaRect(letterContour);

		LetterArea area;
		area.contour = letterContour;
		area.box = letterBox;

		possibleLetterAreas.push_back(area);
	}

	sort(possibleLetterAreas.begin(), possibleLetterAreas.end(), LetterArea::CompareLetterAreaByBorderXAscending);

	return possibleLetterAreas;
}

LetterAreas TitleExtractor::filterOutDuplicates(LetterAreas lettersToFilter) {

	if (lettersToFilter.size() < 2) { return lettersToFilter; }
	
	LetterAreas letters;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		//The first letter is always the first letter of any group of identical letters.
		if (i == 0) {
			letters.push_back(lettersToFilter[i]);
			continue;
		}

		//Bring out the last unique letter.
		LetterArea letterA = letters[letters.size() - 1];
		
		//Bring out the next letter to check for duplication.
		LetterArea letterB = lettersToFilter[i];

		//Check if the letters are identical.
		bool isDuplicates = ImageHelper::IsIdenticalContours(letterA.contour, letterB.contour);
		if (!isDuplicates) {

			//The were not identical so add the second letter to the collection of unique letters.
			letters.push_back(letterB);
		}
	}

	return letters;
}

LetterAreas TitleExtractor::filterOutNoise(LetterAreas lettersToFilter) {

	LetterCheckHelper letterCheck(WORKING_CARD_HEIGHT);
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

LetterAreas TitleExtractor::filterOutLetterHoles(LetterAreas lettersToFilter) {

	LetterAreas letters;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		LetterAreas neighbours;
		if (i != 0) {
			neighbours.push_back(lettersToFilter[i - 1]);
		}
		if (i < lettersToFilter.size() - 1) {
			neighbours.push_back(lettersToFilter[i + 1]);
		}
		//If there are several holes in a letter then the neighbours above might be other holes.
		//Therefore, also check agains the last orphan letter to avoid 
		//Usually letters dont have more than two holes, but fonts and noise could create more.
		if (letters.size() > 0 && i > 1) { //The letter has already been added to the neighbours when i == 1.
			neighbours.push_back(letters[letters.size() - 1]);
		}
		
		LetterArea letter = lettersToFilter[i];
		bool hasParent = LetterArea::HasParentLetter(letter, neighbours);
		if (!hasParent) {

			letters.push_back(letter);
		}
	}

	return letters;
}

LetterAreas TitleExtractor::filterOutManaCost(LetterAreas lettersToFilter) {

	LetterAreas letters;
	LetterArea lastLetter;
	bool hasFoundNameManaGap = false;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		LetterArea letter = lettersToFilter[i];

		//Check if there is a wide distance between the centers.
		//Since we have sorted the letters by the center X coordinate,
		//all subsequent countours will be mana symbols or noise.
		if (hasWideLimitDistance(lastLetter.box, letter.box)) {
			break;
		}

		lastLetter = letter;
		letters.push_back(letter);
	}

	return letters;
}

LetterAreas TitleExtractor::filterOutTransformSymbol(LetterAreas lettersToFilter) {

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

bool TitleExtractor::hasWideLimitDistance(RotatedRect leftLetterArea, RotatedRect rightLetterArea) {

	//The first letter probably isn't a mana symbol.
	if (!isInitialized(leftLetterArea)) { return false; }

	float wideDistance = (float)(WORKING_CARD_HEIGHT / 13.062478); //52.0575-ish

	//The distance between the centers.
	float centerDistance = rightLetterArea.center.x - leftLetterArea.center.x;

	float halfWidthA = ImageHelper::SmallestDistanceCenterToLimit(leftLetterArea);
	float halfWidthB = ImageHelper::SmallestDistanceCenterToLimit(rightLetterArea);
	//The distance between the limits if the rectangles would stand up.
	float limitDistance = centerDistance - halfWidthA - halfWidthB;

	bool isWideSpace = limitDistance > wideDistance;
	return isWideSpace;
}

bool TitleExtractor::isInitialized(RotatedRect rectangleToCheck) {

	float sum = abs(rectangleToCheck.angle)
		+ abs(rectangleToCheck.center.x)
		+ abs(rectangleToCheck.center.y)
		+ abs(rectangleToCheck.size.area());

	return sum != 0;
}
