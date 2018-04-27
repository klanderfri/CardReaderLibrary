#include "stdafx.h"
#include "TitleExtractor.h"
#include "LetterCheckHelper.h"
#include "MtgCardInfoHelper.h"
#include "AlgorithmHelper.h"
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

bool TitleExtractor::ExtractTitle(vector<Mat>& outImages, int binaryThreshold, int& numberOfTries, CardTitleType titleType) {

	int amountOfGauss = (int)(originalImageData.size().height / 18.1818);
	amountOfGauss = errorProtectGaussAmount(amountOfGauss);

	//Make the title grey scale.
	Mat outImage = ImageHelper::ToGreyImage(originalImageData);

	int normalizedTitleHeight = (int)(WORKING_CARD_HEIGHT / 3.67);
	ImageHelper::ResizeImage(outImage, outImage, normalizedTitleHeight);

	//Blur the image to smoothing it out.
	GaussianBlur(outImage, outImage, Size(amountOfGauss, amountOfGauss), 0, 0);

	//Threshold the image to get the important pixels.
	outImage = getBinaryImage(outImage, binaryThreshold, titleType);

	//Extract a clean image containing the title.
	bool success = getTitleText(outImage, outImages, numberOfTries);

	return success;
}

Mat TitleExtractor::getBinaryImage(const Mat titleImage, int binaryThreshold, CardTitleType titleType) {

	Mat binaryImage, workOriginal;
	titleImage.copyTo(workOriginal);
	threshold(workOriginal, binaryImage, binaryThreshold, 255, THRESH_BINARY);

	bool couldHaveBlackBackground = titleType == NormalTitle || titleType == Emblem || titleType == Token;
	bool hasBlackBackground = couldHaveBlackBackground && ImageHelper::PercentageOfNonZero(binaryImage) < 0.3;

	if (hasBlackBackground) {
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
	TrendLine textCenterLine = filter.GetTextCenterLine();
	
	//Something is wrong if there are fewer letters than there are in the shortest MtG card name.
	bool toShortTitle = letters.size() < (size_t)MtgCardInfoHelper::LettersInShortestCardName();
	if (toShortTitle) { return false; }

	//Get the areas of the entire title.
	Contour combinedLetterContorus = ImageHelper::GetCombinedLetterContorus(letters);
	RotatedRect textArea = getTextArea(combinedLetterContorus, textCenterLine, titleImage, numberOfTries);

	//Store result for debugging.
	Mat dbg_onlyLettersBoundImage, dbg_possibleTitleAreaImage;
	if (runDebugging) {

		int radius = (int)(WORKING_CARD_HEIGHT / 226.5); //3
		dbg_onlyLettersBoundImage = ImageHelper::DrawLimits(titleImage, letters, radius);
		dbg_possibleTitleAreaImage = ImageHelper::DrawLimits(titleImage, textArea, Rect(), combinedLetterContorus);
	}

	//The title image cropped using a RotatedRect.
	Mat straightenTitleImage;

	//Rotate the image so it is straight (as much as possible at least).
	Rect2f straightTextArea;
	if (textArea.size.height > textArea.size.width) {
		swap(textArea.size.height, textArea.size.width);
	}
	ImageHelper::StraightenUpImage(titleImage, straightenTitleImage, textArea, straightTextArea, false);

	//Make black text on white background.
	ImageHelper::SetBackgroundByInverting(straightenTitleImage, false);

	//Cut out the title text.
	ImageHelper::CropImageWithSolidBorder(straightenTitleImage, straightenTitleImage, straightTextArea, 10);

	textImages.push_back(straightenTitleImage);

	//Store result for debugging.
	if (runDebugging) {

		SaveOcvImage::SaveImageData(systemMethods, dbg_onlyLettersBoundImage, imageFileName, L"8 - Only Title Letters", numberOfTries);
		SaveOcvImage::SaveImageData(systemMethods, dbg_possibleTitleAreaImage, imageFileName, L"9 - Possible Title Area", numberOfTries);
		SaveOcvImage::SaveImageData(systemMethods, straightenTitleImage, imageFileName, L"10 - Title Text", numberOfTries);
	}

	return true;
}

RotatedRect TitleExtractor::getTextArea(Contour letters, TrendLine centerLine, Mat titleImage, int numberOfTries) {

	if (letters.size() < 3) {
		throw ParameterException("There must be at least three text contour points to get a bounding rotated rectangle!", "combinedLetters");
	}

	//Use boudning rectangle if there is no slope.
	if (centerLine.Slope == 0) {

		Rect bounds = boundingRect(letters);
		RotatedRect rect = ImageHelper::ToRotatedRectangle(bounds);
		
		return rect;
	}

	//Find the top and bottom borders.
	DblContour dLettersContour = ImageHelper::ConvertToDoubleFromInt(letters);
	vector<TrendLine> horizontalBounds = centerLine.GetBoundLines(dLettersContour);

	//Find the left and right borders.
	Point2d firstContourPoint = dLettersContour[0];
	TrendLine perpendicularLine = centerLine.GetPerpendicularLine(firstContourPoint);
	vector<TrendLine> verticalBounds = perpendicularLine.GetBoundLines(dLettersContour);

	//Store result for debugging.
	if (runDebugging) {

		Mat lineImages;
		titleImage.copyTo(lineImages);
		lineImages = ImageHelper::DrawLimits(lineImages, RotatedRect(), Rect(), letters);
		lineImages = ImageHelper::DrawLine(lineImages, horizontalBounds[0]);
		lineImages = ImageHelper::DrawLine(lineImages, horizontalBounds[1]);
		lineImages = ImageHelper::DrawLine(lineImages, verticalBounds[0]);
		lineImages = ImageHelper::DrawLine(lineImages, verticalBounds[1]);

		SaveOcvImage::SaveImageData(systemMethods, lineImages, imageFileName, L"7 - Bounded Characters", numberOfTries);
	}

	//Get the bounded rectangle.
	RotatedRect textArea = AlgorithmHelper::GetRotatedRectangle(verticalBounds, horizontalBounds);

	//Debug
	Mat textAreaImage;
	titleImage.copyTo(textAreaImage);
	textAreaImage = ImageHelper::DrawLimits(textAreaImage, textArea, Rect(), letters);

	return textArea;
}
