#pragma once
#include "ConversionHelper.h"
#include "DrawingHelper.h"
#include "ImageEditHelper.h"
#include "ContourHelper.h"
#include "RectangleHelper.h"
#include "ImageInformationHelper.h"
//Class holding image processing methods.
class ImageHelper
{
public:
	ImageHelper();
	~ImageHelper();

	//Pointer to object doing conversions.
	ConversionHelper* converter;
	//Pointer to object drawing figures on images.
	DrawingHelper* drawingMethods;
	//Pointer to object editing images.
	ImageEditHelper* imageEditor;
	//Pointer to object handling contours.
	ContourHelper* contourMethods;
	//Pointer to object handling rectangles.
	RectangleHelper* rectangleMethods;
	//Pointer to object providing image information.
	ImageInformationHelper* imageInfo;

	//Displays a specified image in a window.
	int ShowImage(const cv::Mat imageToShow);
};
