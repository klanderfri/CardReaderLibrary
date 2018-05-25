#include "stdafx.h"
#include "ConversionHelper.h"

using namespace cv;
using namespace std;

ConversionHelper::ConversionHelper()
{
}

ConversionHelper::~ConversionHelper()
{
}

Rect2f ConversionHelper::ToStraightRectangle(const RotatedRect rawRectangle, bool enforcePortraitMode) {

	//Get the size of the rectangle.
	float width = rawRectangle.size.width;
	float height = rawRectangle.size.height;

	//Change to portait.
	if (enforcePortraitMode && width > height) {
		swap(width, height);
	}

	//Caclulate the coordinates.
	float x = rawRectangle.center.x - (width / 2);
	float y = rawRectangle.center.y - (height / 2);

	return Rect2f(x, y, width, height);
}

RotatedRect ConversionHelper::ToRotatedRectangle(const Rect straightRectangle) {

	float x = straightRectangle.width / (float)2 + straightRectangle.x;
	float y = straightRectangle.height / (float)2 + straightRectangle.y;
	Point2f center(x, y);

	float width = (float)straightRectangle.width;
	float height = (float)straightRectangle.height;
	Size2f size(width, height);

	float angle = 0;

	RotatedRect rotatedRectangle(center, size, angle);

	return rotatedRectangle;
}

Mat ConversionHelper::ToGreyImage(const Mat imageToConvert) {

	return changeImageMode(imageToConvert, COLOR_BGR2GRAY);
}

Mat ConversionHelper::ToColourImage(const Mat imageToConvert) {

	return changeImageMode(imageToConvert, COLOR_GRAY2BGR);
}

Scalar ConversionHelper::ToScalarColour(const Colour colour) {

	//Scalar(BLUE, GREEN, RED)

	switch (colour)
	{
	case FadedDarkBlue:
		return Scalar(180, 117, 42);
	case FadedGreen:
		return Scalar(62, 136, 62);
	case FadedLightBlue:
		return Scalar(200, 150, 90);
	case FadedRed:
		return Scalar(119, 119, 224);
	case FadedPink:
		return Scalar(240, 144, 240);
	case FadedYellow:
		return Scalar(57, 205, 203);
	case FadedWhite:
		return Scalar(196, 196, 196);
	case FadedGrey:
		return Scalar(110, 110, 110);
	case DarkBlue:
		return Scalar(203, 20, 29);
	case Green:
		return Scalar(0, 120, 0);
	case LightBlue:
		return Scalar(255, 120, 40);
	case Red:
		return Scalar(0, 0, 255);
	case Pink:
		return Scalar(240, 107, 240);
	case Yellow:
		return Scalar(0, 255, 255);
	case White:
		return Scalar(255, 255, 255);
	case Purple:
		return Scalar(180, 0, 180);
	case FadedPurple:
		return Scalar(180, 76, 180);
	default:
		throw ParameterException("Unknown colour!", "colour");
	}
}

DblContours ConversionHelper::ConvertToDoubleFromInt(Contours iContours) {

	DblContours dContours;

	for (Contour iContour : iContours) {

		DblContour dContour = ConvertToDoubleFromInt(iContour);
		dContours.push_back(dContour);
	}

	return dContours;
}

Contours ConversionHelper::ConvertToIntFromDouble(DblContours dContours) {

	vector<Contour> iContours;

	for (DblContour dContour : dContours) {

		Contour iContour = ConvertToIntFromDouble(dContour);
		iContours.push_back(iContour);
	}

	return iContours;
}

DblContour ConversionHelper::ConvertToDoubleFromInt(Contour iContour) {

	DblContour dContour;

	for (Point iPoint : iContour) {

		Point2d dPoint = iPoint;
		dContour.push_back(dPoint);
	}

	return dContour;
}

Contour ConversionHelper::ConvertToIntFromDouble(DblContour dContour) {

	Contour iContour;

	for (Point dPoint : dContour) {

		int x = (int)round(dPoint.x);
		int y = (int)round(dPoint.y);
		Point2i iPoint(x, y);
		iContour.push_back(iPoint);
	}

	return iContour;
}

Mat ConversionHelper::changeImageMode(const Mat imageToConvert, ColorConversionCodes conversionCode) {

	Mat convertedImage(imageToConvert.size(), CV_8UC3);
	int amountOfChannels = imageToConvert.channels();

	if (conversionCode == COLOR_GRAY2BGR && amountOfChannels == 3 ||
		conversionCode == COLOR_BGR2GRAY && amountOfChannels == 1) {

		//The image already has the correct mode.
		imageToConvert.copyTo(convertedImage);
	}
	else {

		//Convert the colour mode.
		cvtColor(imageToConvert, convertedImage, conversionCode);
	}

	return convertedImage;
}
