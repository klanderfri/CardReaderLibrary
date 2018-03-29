#include "stdafx.h"
#include "OcvMtgCardInfoHelper.h"

using namespace std;
using namespace cv;

OcvMtgCardInfoHelper::OcvMtgCardInfoHelper()
{
}

OcvMtgCardInfoHelper::~OcvMtgCardInfoHelper()
{
}

int OcvMtgCardInfoHelper::LettersInShortestCardName() {

	return 3;
}

Rect OcvMtgCardInfoHelper::GetNormalTitleSectionBox(Size cardSize) {

	double verticalFactor = 0.037;
	double horizontalFactor = 0.045;
	double heightFactor = 0.068;

	return getSectionBox(cardSize, verticalFactor, horizontalFactor, heightFactor);
}

Rect OcvMtgCardInfoHelper::getSectionBox(Size cardSize, double verticalFactor, double horizontalFactor, double heightFactor) {

	int imgHeight = cardSize.height;
	int imgWidth = cardSize.width;

	int verticalMargins = (int)round(imgHeight * verticalFactor);
	int horizontalMargins = (int)round(imgHeight * horizontalFactor);
	int titleHeight = (int)round(imgHeight * heightFactor);
	int titleWidth = imgWidth - verticalMargins * 2;

	Rect titleBox(verticalMargins, horizontalMargins, titleWidth, titleHeight);
	return titleBox;
}
