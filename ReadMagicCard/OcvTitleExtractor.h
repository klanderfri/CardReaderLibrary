#pragma once
#include "OcvSectionExtractor.h"
#include "LetterArea.h"
//Class for extracting the title section from a card.
class OcvTitleExtractor :
	public OcvSectionExtractor
{
public:
	OcvTitleExtractor(std::wstring imageFileName, cv::Mat originalImageData, OcvSystemDependencyClass* systemMethods, bool debuggingOn);
	~OcvTitleExtractor();

	//Extracts an image containing the card title.
	bool ExtractTitle(cv::Mat& outImage);

private:

	//Makes sure the amount of gauss is valid to use when adding Gaussian blur.
	int errorProtectGaussAmount(int amountOfGauss);
	//Extracts the title text area out of an image containing the entire title area.
	bool getTitleText(const cv::Mat titleImage, cv::Mat& textImage);
	//Gets the areas that might contain a title letter.
	LetterAreas getPossibleLetterAreas(Contours contours);
	//Removes the areas that are noise.
	LetterAreas filterOutNoise(LetterAreas possibleLetterAreas);
	//Removes the areas that belongs to the mana cost.
	LetterAreas filterOutManaCost(LetterAreas possibleLetterAreas);
	//Removes the areas that belongs to the transform symbol.
	LetterAreas filterOutTransformSymbol(LetterAreas possibleLetterAreas);
	//Checks if to letter areas has a "wide" distance between its' centers, indicating that one is a title letter and the other a mana cost.
	bool hasWideCenterDistance(cv::RotatedRect letterArea, cv::RotatedRect lastLetterArea);
	//Checks if a rotated rectangle has been initialized.
	bool isInitialized(cv::RotatedRect rectangle);
};
