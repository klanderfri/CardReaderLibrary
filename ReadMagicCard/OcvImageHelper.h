#pragma once
#include "Contour.h"
#include "Hierarchy.h"
#include <opencv2\imgproc.hpp>
//Class holding image processing methods.
class OcvImageHelper
{
public:
	OcvImageHelper();
	~OcvImageHelper();

	//Converts an rotated rectangle to a straight rectangle in portrait orientation.
	static cv::Rect2f ToStraightRectangle(const cv::RotatedRect rotatedRectToConvert, bool enforcePortraitMode = true);
	//Creates a grey scale image out of the provided image.
	static cv::Mat ToGreyImage(const cv::Mat imageToConvert);
	//Creates a colour image out of the provided image. Note: This method doesn't add any colours to the image.
	static cv::Mat ToColourImage(const cv::Mat imageToConvert);
	//Displays a specified image in a window.
	static int ShowImage(const cv::Mat imageToShow);
	//Rotates an image.
	static void RotateImage(const cv::Mat rawImage, cv::Mat& outImage, const double angleToRotate, const cv::Point2f centerPoint);
	//Draws the specified limits to the image.
	static cv::Mat DrawLimits(const cv::Mat image, const cv::RotatedRect rotatedLimitRectangle, const cv::Rect straightLimitRectangle = cv::Rect(), const Contour limitContour = Contour());
	//Draws the specified limits to the image.
	static cv::Mat DrawLimits(const cv::Mat image, const Contours contours, const Hierarchy hierarchy, bool useRandomColours = true);
	//Draws the specified center point to the image.
	static cv::Mat DrawCenterPoint(const cv::Mat image, const cv::Point imageCenter, cv::Scalar colour = cv::Scalar(0, 0, 255), int radius = 10);
	//Gets how many degrees the rectangle needs to be rotated in positive direction (counter-clockwise) to be straight.
	static double GetAnglesToStrightenUp(const cv::RotatedRect rotatedRectangle, bool enforcePortraitMode = true);
	//Gets the center point of an image.
	static cv::Point2f GetImageCenter(const cv::Mat image);
	//Straighten up an image so it won't be slightly rotated.
	static void StraightenUpImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::RotatedRect rawCardArea, cv::Rect2f& outCardArea, bool enforcePortraitMode = true);
	//Crops the image.
	static void CropImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea);
	//Crops the image and adds a solid border to it.
	static void CropImageWithSolidBorder(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea, int borderThickness);
	//Resizes an image to the specified height. The height/width relatio will be preserved.
	static void ResizeImage(const cv::Mat rawImage, cv::Mat& outImage, int height);
	//Inverts the image background from black to white or vice versa.
	static void SetBackgroundByInverting(cv::Mat& image, bool setblackBackground);
	//Gives the smallest distance between the center and the limit of a rectangle.
	static float SmallestDistanceCenterToLimit(cv::RotatedRect rectangle);
	//Cuts out an area of an image that contains the contour.
	static cv::Mat GetContourAreaFromImage(const Contour contour, const cv::Mat rawImage, const int margins, const bool drawContour = false);
	//Checks if two contours are identical.
	static bool IsIdenticalContours(Contour contour1, Contour contour2);

private:

	//The thickness of borders drawn on images for debugging.
	static const int DEBUG_BORDER_THICKNESS;
	//The line type (8-connected line, aka "solid") of borders drawn on images for debugging.
	static const int DEBUG_BORDER_LINE_TYPE;

	//Changes the image mode between grayscale and colour.
	static cv::Mat changeImageMode(const cv::Mat imageToConvert, cv::ColorConversionCodes code);
};
