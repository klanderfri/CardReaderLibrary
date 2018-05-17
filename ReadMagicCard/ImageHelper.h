#pragma once
#include "LetterArea.h"
#include "Hierarchy.h"
#include "TrendLine.h"
#include "Colour.h"
#include <opencv2\imgproc.hpp>
//Class holding image processing methods.
class ImageHelper
{
public:
	ImageHelper();
	~ImageHelper();

	//The thickness of borders drawn on images for debugging.
	static const int DEFAULT_BORDER_THICKNESS;
	//The line type (8-connected line, aka "solid") of borders drawn on images for debugging.
	static const int DEFAULT_BORDER_LINE_TYPE;

	//Converts a rotated rectangle to a straight rectangle.
	cv::Rect2f ToStraightRectangle(const cv::RotatedRect rotatedRectToConvert, bool enforcePortraitMode = true);
	//Converts a rectangle to a rotated rectangle.
	cv::RotatedRect ToRotatedRectangle(const cv::Rect straightRectangle);
	//Creates a grey scale image out of the provided image.
	cv::Mat ToGreyImage(const cv::Mat imageToConvert);
	//Creates a colour image out of the provided image. Note: This method doesn't add any colours to the image.
	cv::Mat ToColourImage(const cv::Mat imageToConvert);
	//Gets letter area data from a set of contours.
	LetterAreas ToLetterAreas(Contours contours);
	//Converts the enum colour to a scalar colour.
	cv::Scalar ToScalarColour(const Colour colour);
	//Creates the rectangle bound by the the specified borders.
	cv::RotatedRect GetRotatedRectangle(std::vector<TrendLine> verticalBorders, std::vector<TrendLine> horizontalBorders, double angleAdjustment = 0.0);
	//Displays a specified image in a window.
	int ShowImage(const cv::Mat imageToShow);
	//Rotates an image.
	void RotateImage(const cv::Mat rawImage, cv::Mat& outImage, const double angleToRotate, const cv::Point2f centerPoint);
	//Draws a line on an image between two points.
	cv::Mat DrawLine(const cv::Mat image, cv::Point point1, cv::Point point2, Colour colour = Purple, int borderThickness = DEFAULT_BORDER_THICKNESS);
	//Draws a line on an image.
	cv::Mat DrawLine(const cv::Mat image, TrendLine line, Colour colour = Purple, int borderThickness = DEFAULT_BORDER_THICKNESS);
	//Draws the specified limits to the image.
	cv::Mat DrawLimits(const cv::Mat image, const cv::RotatedRect rotatedLimitRectangle, const cv::Rect straightLimitRectangle = cv::Rect(), const Contour limitContour = Contour());
	//Draws the specified limits to the image.
	cv::Mat DrawLimits(const cv::Mat image, const Contours contours, const Hierarchy hierarchy, bool useRandomColours = true);
	//Creates an image showing the limits of the individual letters.
	cv::Mat DrawLimits(const cv::Mat image, const LetterAreas letters, int letterCenterRadius);
	//Draws the specified center point to the image.
	cv::Mat DrawCenterPoint(const cv::Mat image, const cv::Point imageCenter, const Colour colour = Red, int radius = 3);
	//Fills a contour in the image with the specified colour.
	void FillContour(cv::Mat& image, const Contour contour, const Colour colour);
	//Gets how many degrees the rectangle needs to be rotated in positive direction (counter-clockwise) to be straight.
	double GetAnglesToStrightenUp(const cv::RotatedRect rotatedRectangle, bool enforcePortraitMode = true);
	//Gets the center point of an image.
	cv::Point2f GetImageCenter(const cv::Mat image);
	//Straighten up an image so it won't be slightly rotated (it can still be upside down or on the side).
	void StraightenUpImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::RotatedRect rawCardArea, cv::Rect2f& outCardArea, bool enforcePortraitMode = true);
	//Crops the image.
	void CropImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea);
	//Crops the image and adds a solid border to it.
	void CropImageWithSolidBorder(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea, int borderThickness);
	//Resizes an image to the specified height. The height/width relatio will be preserved.
	void ResizeImage(const cv::Mat rawImage, cv::Mat& outImage, int height);
	//Gives the percentage of pixels that are not zero.
	double PercentageOfNonZero(const cv::Mat blackAndWhiteimage);
	//Checks if the image is black text on white background, i.e if it is mostly white.
	bool IsBlackTextWhiteBackground(const cv::Mat blackAndWhiteimage);
	//Inverts the image background from black to white or vice versa.
	void SetBackgroundByInverting(cv::Mat& blackAndWhiteimage, bool setblackBackground);
	//Gives the smallest distance between the center and the limit of a rectangle.
	static float SmallestDistanceCenterToLimit(cv::RotatedRect rectangle);
	//Cuts out an area of an image that contains the contour.
	cv::Mat GetContourAreaFromImage(const Contour contour, const cv::Mat rawImage, const int margins, const bool drawContour = false);
	//Gives the contours of an image using the Canny method.
	Contours GetCannyContours(const cv::Mat greyScaleImage, int thresh);
	//Checks if two contours are identical.
	bool IsIdenticalContours(Contour contour1, Contour contour2);
	//Checks if a rotated rectangle has been initialized.
	bool IsInitialized(cv::RotatedRect rectangle);
	//Checks if a rectangle contains the specified point.
	bool DoesRectangleContainPoint(cv::RotatedRect rectangle, cv::Point2f point);
	//Finds the convex hull of multiple contours.
	Contour ContoursConvexHull(const Contours contours);
	//Gives the combined contour of several letters.
	Contour GetCombinedLetterContorus(LetterAreas letters);
	//Creates an image containing the contour.
	cv::Mat GetContourDrawing(const Contour contour);
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
