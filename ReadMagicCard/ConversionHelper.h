#pragma once
#include "Contour.h"
#include "Colour.h"
#include <opencv2\imgproc.hpp>
//Class for doing conversions.
class ConversionHelper
{
public:
	ConversionHelper();
	~ConversionHelper();

	//Converts a rotated rectangle to a straight rectangle.
	cv::Rect2f ToStraightRectangle(const cv::RotatedRect rotatedRectToConvert, bool enforcePortraitMode = true);
	//Converts a rectangle to a rotated rectangle.
	cv::RotatedRect ToRotatedRectangle(const cv::Rect straightRectangle);
	//Creates a grey scale image out of the provided image.
	cv::Mat ToGreyImage(const cv::Mat imageToConvert);
	//Creates a colour image out of the provided image. Note: This method doesn't add any colours to the image.
	cv::Mat ToColourImage(const cv::Mat imageToConvert);
	//Converts the enum colour to a scalar colour.
	cv::Scalar ToScalarColour(const Colour colour);
	//Converts a set of int contours to a set of double contours.
	DblContours ConvertToDoubleFromInt(Contours iContours);
	//Converts a set of double contours to a set of int contours.
	Contours ConvertToIntFromDouble(DblContours dContours);
	//Converts an int contour to a double contour.
	DblContour ConvertToDoubleFromInt(Contour iContour);
	//Converts a double contour to an int contour.
	Contour ConvertToIntFromDouble(DblContour dContour);

private:

	//Changes the image mode between grayscale and colour.
	cv::Mat changeImageMode(const cv::Mat imageToConvert, cv::ColorConversionCodes code);
};

