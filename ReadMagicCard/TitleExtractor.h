#pragma once
#include "SectionExtractor.h"
#include "LetterArea.h"
//Class for extracting the title section from a card.
class TitleExtractor :
	public SectionExtractor
{
public:
	TitleExtractor(std::wstring imageFileName, cv::Mat originalImageData, SystemMethods* systemMethods, bool runDebugging);
	~TitleExtractor();

	//Extracts an image containing the card title.
	bool ExtractTitle(std::vector<cv::Mat>& outImages);

private:

	//Makes sure the amount of gauss is valid to use when adding Gaussian blur.
	int errorProtectGaussAmount(int amountOfGauss);
	//Extracts the title text area out of an image containing the entire title area.
	bool getTitleText(const cv::Mat titleImage, std::vector<cv::Mat>& textImages);
	//Gets the areas that might contain a title letter.
	LetterAreas getPossibleLetterAreas(Contours contours);
	//Filter out letter areas that are identical, ie letter areas pointing at the same letter.
	LetterAreas filterOutDuplicates(LetterAreas lettersToFilter);
	//Removes the areas that are noise.
	LetterAreas filterOutNoise(LetterAreas lettersToFilter);
	//Filter out the holes inside the letters. Like the hole in 'o' and 'P'.
	LetterAreas filterOutLetterHoles(LetterAreas lettersToFilter);
	//Removes the areas that belongs to the mana cost.
	LetterAreas filterOutManaCost(LetterAreas lettersToFilter);
	//Removes the areas that belongs to the transform symbol.
	LetterAreas filterOutTransformSymbol(LetterAreas lettersToFilter);
	//Checks if to letter areas has a "wide" distance between its' centers, indicating that one is a title letter and the other a mana cost.
	bool hasWideLimitDistance(cv::RotatedRect leftLetterArea, cv::RotatedRect rightLetterArea);
	//Checks if a rotated rectangle has been initialized.
	bool isInitialized(cv::RotatedRect rectangle);
};
