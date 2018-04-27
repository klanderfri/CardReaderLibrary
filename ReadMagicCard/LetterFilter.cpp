#include "stdafx.h"
#include "LetterFilter.h"
#include "LetterCheckHelper.h"
#include "MtgCardInfoHelper.h"
#include "AlgorithmHelper.h"
#include "SaveOcvImage.h"

using namespace cv;
using namespace std;

LetterFilter::LetterFilter(wstring imageFileName, Mat originalImageData, SystemMethods* systemMethods, bool runDebugging)
	: BasicReaderData(imageFileName, originalImageData, systemMethods, runDebugging)
{
}

LetterFilter::~LetterFilter()
{
}

TrendLine LetterFilter::GetTextCenterLine() {

	return textCenterLine;
}

LetterAreas LetterFilter::RunFilter(Contours contours, int numberOfTries) {

	//Reset center line.
	textCenterLine = TrendLine(0, originalImageData.rows / 2);

	//Do a crude filtering of the the letters.
	LetterAreas allPossibleLetters = getPossibleLetterAreas(contours);
	LetterAreas letters = filterOutNonTitleSymbols(allPossibleLetters);

	//Check if there was any letters at all.
	if (letters.empty()) {
		return LetterAreas();
	}

	//Find the center of the title.
	textCenterLine = findTitleCenterLine(letters);

	//Store result for debugging.
	if (runDebugging) {

		float leftLimit = letters[0].Box.center.x;
		float rightLimit = letters[letters.size() - 1].Box.center.x;
		vector<Point2d> line = textCenterLine.GetEndPoints(leftLimit, rightLimit);

		Mat trendImage = ImageHelper::DrawLimits(originalImageData, letters, 3);
		trendImage = ImageHelper::DrawLine(trendImage, line[0], line[1]);

		SaveOcvImage::SaveImageData(systemMethods, trendImage, imageFileName, L"6 - Title Center Line", numberOfTries);
	}
	
	//Redo the filtering with now when we got the center line to work with.
	//Sometimes real letters are missed but the trend line makes it easier
	//to avoid those false negatives.
	letters = filterOutNonTitleSymbols(allPossibleLetters);
	letters = filterOutNonNameSymbols(letters);

	//Find the center of the title.
	textCenterLine = findTitleCenterLine(letters);

	return letters;
}

TrendLine LetterFilter::findTitleCenterLine(LetterAreas letters) {

	vector<Point2d> points;

	for (LetterArea letter : letters) {
		points.push_back(letter.Box.center);
	}

	return TrendLine(points);
}

LetterAreas LetterFilter::getPossibleLetterAreas(Contours contours) {

	LetterAreas possibleLetterAreas;

	for (size_t i = 0; i < contours.size(); i++) {

		Contour letterContour = ImageHelper::ContoursConvexHull({ contours[i] });
		RotatedRect letterBox = minAreaRect(letterContour);

		LetterArea area;
		area.TightContour = contours[i];
		area.OuterContour = letterContour;
		area.Box = letterBox;

		possibleLetterAreas.push_back(area);
	}

	sort(possibleLetterAreas.begin(), possibleLetterAreas.end(), LetterArea::CompareLetterAreaByLeftBorderXAscending);

	return possibleLetterAreas;
}

LetterAreas LetterFilter::filterOutNonTitleSymbols(LetterAreas lettersToFilter) {

	LetterAreas letters;
	letters = filterOutDuplicates(lettersToFilter);
	letters = filterOutNoise(letters);
	letters = filterOutTransformSymbol(letters);
	letters = filterOutLetterHoles(letters);

	return letters;
}

LetterAreas LetterFilter::filterOutDuplicates(LetterAreas lettersToFilter) {

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
		bool isDuplicates = ImageHelper::IsIdenticalContours(letterA.OuterContour, letterB.OuterContour);
		if (!isDuplicates) {

			//The were not identical so add the second letter to the collection of unique letters.
			letters.push_back(letterB);
		}
	}

	return letters;
}

LetterAreas LetterFilter::filterOutNoise(LetterAreas lettersToFilter) {

	LetterCheckHelper letterCheck(WORKING_CARD_HEIGHT, textCenterLine);
	LetterAreas letters;

	for (size_t i = 0; i < lettersToFilter.size(); i++)
	{
		LetterArea letter = lettersToFilter[i];

		if (letterCheck.IsLetter(letter.Box)) {

			letters.push_back(letter);
		}
	}

	return letters;
}

LetterAreas LetterFilter::filterOutLetterHoles(LetterAreas lettersToFilter) {

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

LetterAreas LetterFilter::filterOutNonNameSymbols(LetterAreas lettersToFilter) {

	vector<LetterAreas> sections = groupLettersBySection(lettersToFilter);

	//Remove unworthy sections.
	vector<LetterAreas> letterSections;
	for (LetterAreas section : sections) {

		//Remove sections not containing enough letters to be the title.
		size_t sectionSize = section.size();
		if (sectionSize < (size_t)MtgCardInfoHelper::LettersInShortestCardName()) {
			continue;
		}

		//Remove sections that has to much vertical spread between the letters, indicating noise.
		//This would probably work better if we could find a center trend line and check the spread related to that line.
		float toppestY = (float)originalImageData.rows, bottomestY = 0;
		for (LetterArea letter : section) {

			if (toppestY > letter.Box.center.y) {
				toppestY = letter.Box.center.y;
			}
			if (bottomestY < letter.Box.center.y) {
				bottomestY = letter.Box.center.y;
			}
		}
		float verticalSpread = bottomestY - toppestY;
		float allowedSpread = (float)(WORKING_CARD_HEIGHT / 10.46); //65
		if (verticalSpread > allowedSpread) {
			continue;
		}

		//Seems like an OK section.
		letterSections.push_back(section);
	}

	//Make sure there actually are any sections left.
	size_t numberOfSections = letterSections.size();
	if (numberOfSections == 0) {
		return LetterAreas();
	}

	//Only one section? Lets hope it's the title!
	if (numberOfSections == 1) {
		return letterSections[0];
	}

	//It's probably the title if it's centered.
	int titleWidth = originalImageData.cols;
	int middleAreaWidth = titleWidth / 5;
	int minCoordinateX = (titleWidth / 2) - middleAreaWidth / 2;
	int maxCoordinateX = (titleWidth / 2) + middleAreaWidth / 2;
	for (LetterAreas letters : letterSections) {

		Contour combinedLetterContorus = ImageHelper::GetCombinedLetterContorus(letters);
		RotatedRect textArea = minAreaRect(combinedLetterContorus);

		bool isCentered = (minCoordinateX < textArea.center.x && maxCoordinateX > textArea.center.x);
		if (isCentered) { return letters; }
	}

	//If there are two sections and none is centered then it's probably the title and the mana cost.
	//Note that we have filtered out the transform  symbol previously.
	return letterSections[0];
}


LetterAreas LetterFilter::filterOutTransformSymbol(LetterAreas lettersToFilter) {

	//Sometimes the transform symbol is close enough the title to not have a wide distance.
	//Which means we need to handle those transform symbols separately.

	//This method works for 'Gatstaf Shepard' but probably not for transform
	//cards from later sets since they have a different transform symbol.

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
			lastLetterBox = letter.Box;
			continue;
		}

		if (letter.Box.angle < -30 && letter.Box.angle > -60) {
			hasOneRotated = true;
		}

		float xDistance = abs(lastLetterBox.center.x - letter.Box.center.x);
		float yDistance = abs(lastLetterBox.center.y - letter.Box.center.y);
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

vector<LetterAreas> LetterFilter::groupLettersBySection(LetterAreas lettersToFilter) {

	vector<LetterAreas> letters;
	LetterArea lastLetter;
	bool hasFoundNameManaGap = false;

	letters.push_back(LetterAreas());
	size_t sectionIndex = 0;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		LetterArea letter = lettersToFilter[i];

		//Check if there is a wide distance between the centers.
		//Since we have sorted the letters by the center X coordinate,
		//all subsequent countours will be mana symbols or noise.
		if (hasWideLimitDistance(lastLetter.Box, letter.Box)) {

			letters.push_back(LetterAreas());
			sectionIndex++;
		}

		lastLetter = letter;
		letters[sectionIndex].push_back(letter);
	}

	return letters;
}

bool LetterFilter::hasWideLimitDistance(RotatedRect leftLetterArea, RotatedRect rightLetterArea) {

	//The first letter probably isn't a mana symbol.
	if (!ImageHelper::IsInitialized(leftLetterArea)) { return false; }

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
