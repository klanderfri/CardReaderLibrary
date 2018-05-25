#pragma once
#include "DrawingHelper.h"
#include "ImageEditHelper.h"
//Class for handling contours.
class ContourHelper
{
public:
	ContourHelper();
	~ContourHelper();

	//Gives the contours of an image using the Canny method.
	Contours GetCannyContours(const cv::Mat greyScaleImage, int thresh);
	//Cuts out an area of an image that contains the contour.
	cv::Mat GetContourAreaFromImage(const Contour contour, const cv::Mat rawImage, const int margins, const bool drawContour = false);
	//Finds the convex hull of multiple contours.
	static Contour GetConvexHullContours(const Contours contours);
	//Gives the combined contour of several letters.
	Contour GetCombinedLetterContorus(LetterAreas letters);
	//Creates an image containing the contour.
	cv::Mat GetContourDrawing(const Contour contour);
	//Checks if two contours are identical.
	bool IsIdenticalContours(Contour contour1, Contour contour2);

private:

	//Pointer to object drawing figures on images.
	DrawingHelper * drawingMethods;
	//Pointer to object editing images.
	ImageEditHelper* imageEditor;
};
