#pragma once
#include "FigureArea.h"
#include "TrendLine.h"
#include "ImageHandlerBase.h"
//Class for filtering out image parts that are noise.
class LetterFilter : 
	public ImageHandlerBase
{
public:
	LetterFilter(Session* session, std::wstring imageFilePath, cv::Mat originalImageData);
	~LetterFilter();

	//Runs the filter and returns the letters.
	FigureAreas RunFilter(Contours contours, int numberOfTries);
	//Gets the center line of the text.
	TrendLine GetTextCenterLine();
	//Gets the base line of the text.
	TrendLine GetTextBaseLine();

private:
	//Finds the center line for a set of letters.
	TrendLine findCenterLine(FigureAreas letters);
	//Finds the base line (the line marking the bottom) for a set of letters.
	TrendLine findBaseLine(FigureAreas letters);
	//Filter out letter areas that are identical, ie letter areas pointing at the same letter.
	FigureAreas filterOutDuplicates(FigureAreas lettersToFilter);
	//Removes the areas that are noise.
	FigureAreas filterOutNoise(FigureAreas lettersToFilter);
	//Filter out the holes inside the letters. Like the hole in 'o' and 'P'.
	FigureAreas filterOutLetterHoles(FigureAreas lettersToFilter);
	//Filter out the symbols not belonging to the title elements.
	FigureAreas filterOutNonTitleSymbols(FigureAreas lettersToFilter);
	//Removes the areas containing symbols not belonging to the card name.
	FigureAreas filterOutNonNameSymbols(FigureAreas lettersToFilter);
	//Removes the areas that belongs to the transform symbol.
	FigureAreas filterOutTransformSymbol(FigureAreas lettersToFilter);
	//Groups the letters by section separated by wide distances.
	std::vector<FigureAreas> groupLettersBySection(FigureAreas lettersToFilter);
	//Checks if to letter areas has a "wide" distance between its' centers, indicating that one is a title letter and the other a mana cost.
	bool hasWideLimitDistance(cv::RotatedRect leftLetterArea, cv::RotatedRect rightLetterArea);

	//The approximal line going through the center of the text.
	TrendLine textCenterLine;
	//The approximal line going through the base of the text.
	TrendLine textBaseLine;
};

