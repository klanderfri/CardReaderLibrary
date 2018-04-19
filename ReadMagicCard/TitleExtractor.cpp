#include "stdafx.h"
#include "TitleExtractor.h"
#include "LetterCheckHelper.h"
#include "MtgCardInfoHelper.h"
#include "SaveOcvImage.h"
#include "LetterFilter.h"

using namespace cv;
using namespace std;

TitleExtractor::TitleExtractor(wstring imageFileName, Mat originalImageData, SystemMethods* systemMethods, bool runDebugging)
	: SectionExtractor(imageFileName, originalImageData, systemMethods, runDebugging)
{
}

TitleExtractor::~TitleExtractor()
{
}

bool TitleExtractor::ExtractTitle(vector<Mat>& outImages, int binaryThreshold, int& numberOfTries) {

	int amountOfGauss = (int)(originalImageData.size().height / 18.1818);
	amountOfGauss = errorProtectGaussAmount(amountOfGauss);

	//Make the title grey scale.
	Mat outImage = ImageHelper::ToGreyImage(originalImageData);

	int normalizedTitleHeight = (int)(WORKING_CARD_HEIGHT / 3.67);
	ImageHelper::ResizeImage(outImage, outImage, normalizedTitleHeight);

	//Blur the image to smoothing it out.
	GaussianBlur(outImage, outImage, Size(amountOfGauss, amountOfGauss), 0, 0);

	//Threshold the image to get the important pixels.
	outImage = getBinaryImage(outImage, binaryThreshold);

	//Extract a clean image containing the title.
	bool success = getTitleText(outImage, outImages, numberOfTries);

	return success;
}

Mat TitleExtractor::getBinaryImage(const Mat titleImage, int binaryThreshold) {

	Mat binaryImage, workOriginal;
	titleImage.copyTo(workOriginal);

	threshold(workOriginal, binaryImage, binaryThreshold, 255, THRESH_BINARY);
	if (ImageHelper::PercentageOfNonZero(binaryImage) < 0.3) {
		workOriginal = ~workOriginal;
	}

	do {
		threshold(workOriginal, binaryImage, binaryThreshold, 255, THRESH_BINARY);
		binaryThreshold -= 20;

	} while (!ImageHelper::IsBlackTextWhiteBackground(binaryImage) && binaryThreshold > 0);
	
	return binaryImage;
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

bool TitleExtractor::getTitleText(const Mat titleImage, vector<Mat>& textImages, int& numberOfTries) {

	numberOfTries++;

	Contours contours = ImageHelper::GetCannyContours(titleImage, 120);
	LetterFilter filter(imageFileName, titleImage, systemMethods, runDebugging);
	LetterAreas letters = filter.RunFilter(contours, numberOfTries);
	
	//Something is wrong if there are fewer letters than there are in the shortest MtG card name.
	bool toShortTitle = letters.size() < (size_t)MtgCardInfoHelper::LettersInShortestCardName();
	if (toShortTitle) { return false; }

	//Get the areas of the entire title.
	Contour combinedLetterContorus = ImageHelper::GetCombinedLetterContorus(letters);
	RotatedRect textArea = minAreaRect(combinedLetterContorus);
	Rect boundTextArea = boundingRect(combinedLetterContorus);

	//Store result for debugging.
	Mat dbg_onlyLettersBoundImage, dbg_possibleTitleAreaImage;
	if (runDebugging) {

		int radius = (int)(WORKING_CARD_HEIGHT / 226.5); //3
		dbg_onlyLettersBoundImage = ImageHelper::DrawLimits(titleImage, letters, radius);
		dbg_possibleTitleAreaImage = ImageHelper::DrawLimits(titleImage, textArea, boundTextArea, combinedLetterContorus);
	}

	//The title image cropped using a RotatedRect.
	Mat straightenTitleImage;
	//The title image cropped using a Rect.
	Mat boundedTitleImage;

	//Rotate the image so it is straight (as much as possible at least).
	Rect2f straightTextArea;
	if (textArea.size.height > textArea.size.width) {
		swap(textArea.size.height, textArea.size.width);
	}
	ImageHelper::StraightenUpImage(titleImage, straightenTitleImage, textArea, straightTextArea, false);

	//The bounded text image doesn't need any rotation.
	titleImage.copyTo(boundedTitleImage);

	//Make black text on white background.
	ImageHelper::SetBackgroundByInverting(straightenTitleImage, false);
	ImageHelper::SetBackgroundByInverting(boundedTitleImage, false);

	//Cut out the title text.
	ImageHelper::CropImageWithSolidBorder(straightenTitleImage, straightenTitleImage, straightTextArea, 10);
	ImageHelper::CropImageWithSolidBorder(boundedTitleImage, boundedTitleImage, boundTextArea, 10);

	textImages.push_back(straightenTitleImage);
	textImages.push_back(boundedTitleImage);

	//Store result for debugging.
	if (runDebugging) {

		SaveOcvImage::SaveImageData(systemMethods, dbg_onlyLettersBoundImage, imageFileName, L"7 - Only Title Letters", numberOfTries);
		SaveOcvImage::SaveImageData(systemMethods, dbg_possibleTitleAreaImage, imageFileName, L"8 - Possible Title Area", numberOfTries);

		SaveOcvImage::SaveImageData(systemMethods, straightenTitleImage, imageFileName, L"9 - Title Text (Straighten)", numberOfTries);
		SaveOcvImage::SaveImageData(systemMethods, boundedTitleImage, imageFileName, L"10 - Title Text (Bounded)", numberOfTries);
	}

	return true;
}
