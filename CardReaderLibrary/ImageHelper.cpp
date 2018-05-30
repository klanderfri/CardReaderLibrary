#include "stdafx.h"
#include "ImageHelper.h"
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

ImageHelper::ImageHelper(SystemMethods* systemMethods)
{
	this->converter = new ConversionHelper();
	this->drawingMethods = new DrawingHelper();
	this->imageEditor = new ImageEditHelper();
	this->contourMethods = new ContourHelper();
	this->rectangleMethods = new RectangleHelper();
	this->imageInfo = new ImageInformationHelper();
}

ImageHelper::~ImageHelper()
{
	delete converter;
	delete drawingMethods;
	delete imageEditor;
	delete contourMethods;
	delete rectangleMethods;
	delete imageInfo;
}

int ImageHelper::ShowImage(const Mat imageToShow) {

	//Create a window for display.
	namedWindow("Display Window", WINDOW_AUTOSIZE);

	//Show our image inside it.
	imshow("Display Window", imageToShow);

	return 0;
}
