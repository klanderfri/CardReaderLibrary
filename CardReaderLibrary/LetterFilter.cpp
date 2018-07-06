#include "stdafx.h"
#include "LetterFilter.h"
#include "LetterCheckHelper.h"
#include "MtgCardInfoHelper.h"
#include "AlgorithmHelper.h"
#include "SaveOcvImage.h"

using namespace cv;
using namespace std;

LetterFilter::LetterFilter(Session* session, wstring imageFilePath, Mat originalImageData)
	: ImageHandlerBase(session, imageFilePath, originalImageData)
{
}

LetterFilter::~LetterFilter()
{
}

TrendLine LetterFilter::GetTextCenterLine() {

	return textCenterLine;
}

TrendLine LetterFilter::GetTextBaseLine() {

	return textBaseLine;
}

FigureAreas LetterFilter::RunFilter(Contours contours, int numberOfTries) {

	//Reset base and center line.
	textBaseLine = textCenterLine = TrendLine(0, originalImageData.rows / 2);

	//Do a crude filtering of the the letters.
	FigureAreas allPossibleLetters(contours);
	FigureAreas letters = filterOutNonTitleSymbols(allPossibleLetters);

	//Check if there was any letters at all.
	if (letters.empty()) {
		return FigureAreas();
	}

	//Find the center of the title.
	findCenterLine(letters);
	
	//Redo the filtering with now when we got the center line to work with.
	//Sometimes real letters are missed but the trend line makes it easier
	//to avoid those false negatives.
	letters = filterOutNonTitleSymbols(allPossibleLetters);
	letters = filterOutNonNameSymbols(letters);

	//Find the center of the title.
	findCenterLine(letters);

	//Store result for debugging.
	if (session->inputData->runDebugging) {

		bool hasLetters = !letters.empty();
		float leftLimit = hasLetters ? letters[0].Box.center.x : 0;
		float rightLimit = hasLetters ? letters[letters.size() - 1].Box.center.x : originalImageData.cols;
		vector<Point2d> cLine = textCenterLine.GetEndPoints(leftLimit, rightLimit);
		vector<Point2d> bLine = textBaseLine.GetEndPoints(leftLimit, rightLimit);

		int radius = 3;
		Mat trendImage = session->imageHelper->drawingMethods->DrawFigureAreas(originalImageData, letters, radius);
		for (FigureArea letter : letters) {
			trendImage = session->imageHelper->drawingMethods->DrawCircle(trendImage, letter.GetMiddleBottomPoint(), radius, Green);
		}
		trendImage = session->imageHelper->drawingMethods->DrawLine(trendImage, cLine[0], cLine[1]);
		trendImage = session->imageHelper->drawingMethods->DrawLine(trendImage, bLine[0], bLine[1]);

		session->fileSystem->imageSaver->SaveImageData(trendImage, imageFileName, L"5 - Title Center Line", numberOfTries);
	}

	return letters;
}

TrendLine LetterFilter::findCenterLine(FigureAreas letters) {

	LetterCheckHelper letterCheck(session->WORKING_CARD_HEIGHT, textCenterLine);
	vector<long double> xCoordinates, yCoordinates;

	for (FigureArea letter : letters) {

		if (letterCheck.IsDotLikeCharacter(letter.Box)) { continue; }

		xCoordinates.push_back(letter.Box.center.x);
		yCoordinates.push_back(letter.Box.center.y);
	}

	TrendLine baseLine = findBaseLine(letters);

	double avgX = AlgorithmHelper::Average(xCoordinates);
	double avgY = AlgorithmHelper::Average(yCoordinates);
	Point2d averageCenter(avgX, avgY);

	double extraOffset = baseLine.GetVerticalDistance(averageCenter);
	TrendLine centerLine(baseLine.Slope, baseLine.Offset + extraOffset);

	textCenterLine = centerLine;
	textBaseLine = baseLine;

	assert(textCenterLine.Slope == textBaseLine.Slope);

	return textCenterLine;
}

TrendLine LetterFilter::findBaseLine(FigureAreas letters) {

	if (letters.size() == 1) {

		FigureArea letter = letters[0];
		double angle = (letter.Box.angle < -45) ? -(90 + letter.Box.angle) : -letter.Box.angle;
		TrendLine baseLine(angle, letter.Box.center);
		return baseLine;
	}

	LetterCheckHelper letterCheck(session->WORKING_CARD_HEIGHT, textCenterLine);
	vector<Point2d> bottomPoints;
	FigureAreas baseLineLetters;

	for (FigureArea letter : letters) {

		//Ignore dots, commas and the like to avoid having them disturb the text base line.
		if (letterCheck.IsDotLikeCharacter(letter.Box)) { continue; }

		bottomPoints.push_back(letter.GetMiddleBottomPoint());
		baseLineLetters.push_back(letter);
	}

	TrendLine baseLine(bottomPoints);
	double allowedPerpendicularDistance = (double)(session->WORKING_CARD_HEIGHT / 85.0); //8
	bottomPoints.clear();

	for (FigureArea letter : baseLineLetters) {

		double perpendicularDistance = abs(baseLine.GetPerpendicularDistance(letter.GetMiddleBottomPoint()));
		if (perpendicularDistance > allowedPerpendicularDistance) { continue; }

		bottomPoints.push_back(letter.GetMiddleBottomPoint());
	}

	baseLine = TrendLine(bottomPoints);
	return baseLine;
}

FigureAreas LetterFilter::filterOutNonTitleSymbols(FigureAreas lettersToFilter) {

	FigureAreas letters;
	letters = filterOutDuplicates(lettersToFilter);
	letters = filterOutNoise(letters);
	letters = filterOutTransformSymbol(letters);
	letters = filterOutLetterHoles(letters);

	return letters;
}

FigureAreas LetterFilter::filterOutDuplicates(FigureAreas lettersToFilter) {

	if (lettersToFilter.size() < 2) { return lettersToFilter; }

	FigureAreas letters;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		//The first letter is always the first letter of any group of identical letters.
		if (i == 0) {
			letters.push_back(lettersToFilter[i]);
			continue;
		}

		//Bring out the last unique letter.
		FigureArea letterA = letters[letters.size() - 1];

		//Bring out the next letter to check for duplication.
		FigureArea letterB = lettersToFilter[i];

		//Check if the letters are identical.
		bool isDuplicates = session->imageHelper->contourMethods->IsIdenticalContours(letterA.OuterContour, letterB.OuterContour);
		if (!isDuplicates) {

			//The were not identical so add the second letter to the collection of unique letters.
			letters.push_back(letterB);
		}
	}

	return letters;
}

FigureAreas LetterFilter::filterOutNoise(FigureAreas lettersToFilter) {

	LetterCheckHelper letterCheck(session->WORKING_CARD_HEIGHT, textCenterLine);
	FigureAreas letters;

	for (size_t i = 0; i < lettersToFilter.size(); i++)
	{
		FigureArea letter = lettersToFilter[i];

		if (letterCheck.IsLetter(letter.Box)) {

			letters.push_back(letter);
		}
	}

	return letters;
}

FigureAreas LetterFilter::filterOutLetterHoles(FigureAreas lettersToFilter) {

	FigureAreas letters;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		FigureAreas neighbours;
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

		FigureArea letter = lettersToFilter[i];
		bool hasParent = FigureAreas::HasParentFigure(letter, neighbours);
		if (!hasParent) {

			letters.push_back(letter);
		}
	}

	return letters;
}

FigureAreas LetterFilter::filterOutNonNameSymbols(FigureAreas lettersToFilter) {

	vector<FigureAreas> sections = groupLettersBySection(lettersToFilter);

	//Remove unworthy sections.
	vector<FigureAreas> letterSections;
	for (FigureAreas section : sections) {

		//Remove sections not containing enough letters to be the title.
		size_t sectionSize = section.size();
		if (sectionSize < (size_t)session->cardInfo->LettersInShortestCardName()) {
			continue;
		}

		//Remove sections that has to much vertical spread between the letters, indicating noise.
		//This would probably work better if we could find a center trend line and check the spread related to that line.
		float toppestY = (float)originalImageData.rows, bottomestY = 0;
		for (FigureArea letter : section) {

			if (toppestY > letter.Box.center.y) {
				toppestY = letter.Box.center.y;
			}
			if (bottomestY < letter.Box.center.y) {
				bottomestY = letter.Box.center.y;
			}
		}
		float verticalSpread = bottomestY - toppestY;
		float allowedSpread = (float)(session->WORKING_CARD_HEIGHT / 10.46); //65
		if (verticalSpread > allowedSpread) {
			continue;
		}

		//Seems like an OK section.
		letterSections.push_back(section);
	}

	//Make sure there actually are any sections left.
	size_t numberOfSections = letterSections.size();
	if (numberOfSections == 0) {
		return FigureAreas();
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
	for (FigureAreas letters : letterSections) {

		Contour combinedLetterContorus = session->imageHelper->contourMethods->GetCombinedFigureContorus(letters);
		RotatedRect textArea = minAreaRect(combinedLetterContorus);

		bool isCentered = (minCoordinateX < textArea.center.x && maxCoordinateX > textArea.center.x);
		if (isCentered) { return letters; }
	}

	//If there are two sections and none is centered then it's probably the title and the mana cost.
	//Note that we have filtered out the transform  symbol previously.
	return letterSections[0];
}


FigureAreas LetterFilter::filterOutTransformSymbol(FigureAreas lettersToFilter) {

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

		FigureArea letter = lettersToFilter[i];

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

	int allowedDistance = (int)(session->WORKING_CARD_HEIGHT / 136); //5

	if (biggestCenterDistance < allowedDistance && hasOneRotated) {

		FigureAreas::const_iterator first = lettersToFilter.begin() + amountOfTransformLetters;
		FigureAreas::const_iterator last = lettersToFilter.end();
		return FigureAreas(first, last);
	}

	return lettersToFilter;
}

vector<FigureAreas> LetterFilter::groupLettersBySection(FigureAreas lettersToFilter) {

	vector<FigureAreas> letters;
	FigureArea lastLetter;
	bool hasFoundNameManaGap = false;

	letters.push_back(FigureAreas());
	size_t sectionIndex = 0;

	for (size_t i = 0; i < lettersToFilter.size(); i++) {

		FigureArea letter = lettersToFilter[i];

		//Check if there is a wide distance between the centers.
		//Since we have sorted the letters by the center X coordinate,
		//all subsequent countours will be mana symbols or noise.
		if (hasWideLimitDistance(lastLetter.Box, letter.Box)) {

			letters.push_back(FigureAreas());
			sectionIndex++;
		}

		lastLetter = letter;
		letters[sectionIndex].push_back(letter);
	}

	return letters;
}

bool LetterFilter::hasWideLimitDistance(RotatedRect leftLetterArea, RotatedRect rightLetterArea) {

	//The first letter probably isn't a mana symbol.
	if (!session->imageHelper->rectangleMethods->IsInitialized(leftLetterArea)) { return false; }

	float wideDistance = (float)(session->WORKING_CARD_HEIGHT / 13.062478); //52.0575-ish

	//The distance between the centers.
	float centerDistance = rightLetterArea.center.x - leftLetterArea.center.x;

	float halfWidthA = session->imageHelper->rectangleMethods->SmallestDistanceCenterToLimit(leftLetterArea);
	float halfWidthB = session->imageHelper->rectangleMethods->SmallestDistanceCenterToLimit(rightLetterArea);
	//The distance between the limits if the rectangles would stand up.
	float limitDistance = centerDistance - halfWidthA - halfWidthB;

	bool isWideSpace = limitDistance > wideDistance;
	return isWideSpace;
}
