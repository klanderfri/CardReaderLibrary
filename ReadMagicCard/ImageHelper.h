#pragma once
#include "LetterArea.h"
#include "Hierarchy.h"
#include <opencv2\imgproc.hpp>
//Class holding image processing methods.
class ImageHelper
{
public:
	ImageHelper();
	~ImageHelper();

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
	//Draws a line on an image between two points.
	static cv::Mat DrawLine(const cv::Mat image, cv::Point point1, cv::Point point2);
	//Draws the specified limits to the image.
	static cv::Mat DrawLimits(const cv::Mat image, const cv::RotatedRect rotatedLimitRectangle, const cv::Rect straightLimitRectangle = cv::Rect(), const Contour limitContour = Contour());
	//Draws the specified limits to the image.
	static cv::Mat DrawLimits(const cv::Mat image, const Contours contours, const Hierarchy hierarchy, bool useRandomColours = true);
	//Creates an image showing the limits of the individual letters.
	static cv::Mat DrawLimits(const cv::Mat image, const LetterAreas letters, int letterCenterRadius);
	//Draws the specified center point to the image.
	static cv::Mat DrawCenterPoint(const cv::Mat image, const cv::Point imageCenter, cv::Scalar colour = cv::Scalar(0, 0, 255), int radius = 3);
	//Gets how many degrees the rectangle needs to be rotated in positive direction (counter-clockwise) to be straight.
	static double GetAnglesToStrightenUp(const cv::RotatedRect rotatedRectangle, bool enforcePortraitMode = true);
	//Gets the center point of an image.
	static cv::Point2f GetImageCenter(const cv::Mat image);
	//Straighten up an image so it won't be slightly rotated (it can still be upside down or on the side).
	static void StraightenUpImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::RotatedRect rawCardArea, cv::Rect2f& outCardArea, bool enforcePortraitMode = true);
	//Crops the image.
	static void CropImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea);
	//Crops the image and adds a solid border to it.
	static void CropImageWithSolidBorder(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea, int borderThickness);
	//Resizes an image to the specified height. The height/width relatio will be preserved.
	static void ResizeImage(const cv::Mat rawImage, cv::Mat& outImage, int height);
	//Gives the percentage of pixels that are not zero.
	static double PercentageOfNonZero(const cv::Mat blackAndWhiteimage);
	//Checks if the image is black text on white background, i.e if it is mostly white.
	static bool IsBlackTextWhiteBackground(const cv::Mat blackAndWhiteimage);
	//Inverts the image background from black to white or vice versa.
	static void SetBackgroundByInverting(cv::Mat& blackAndWhiteimage, bool setblackBackground);
	//Gives the smallest distance between the center and the limit of a rectangle.
	static float SmallestDistanceCenterToLimit(cv::RotatedRect rectangle);
	//Cuts out an area of an image that contains the contour.
	static cv::Mat GetContourAreaFromImage(const Contour contour, const cv::Mat rawImage, const int margins, const bool drawContour = false);
	//Gives the contours of an image using the Canny method.
	static Contours GetCannyContours(const cv::Mat greyScaleImage, int thresh);
	//Checks if two contours are identical.
	static bool IsIdenticalContours(Contour contour1, Contour contour2);
	//Checks if a rotated rectangle has been initialized.
	static bool IsInitialized(cv::RotatedRect rectangle);
	//Finds the convex hull of multiple contours.
	static Contour ContoursConvexHull(const Contours contours);
	//Gives the combined contour of several letters.
	static Contour GetCombinedLetterContorus(LetterAreas letters);
	//Creates an image containing the contour.
	static cv::Mat GetContourDrawing(const Contour contour);

private:

	//The thickness of borders drawn on images for debugging.
	static const int DEBUG_BORDER_THICKNESS;
	//The line type (8-connected line, aka "solid") of borders drawn on images for debugging.
	static const int DEBUG_BORDER_LINE_TYPE;

	//Changes the image mode between grayscale and colour.
	static cv::Mat changeImageMode(const cv::Mat imageToConvert, cv::ColorConversionCodes code);
};
