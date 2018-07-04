#include "stdafx.h"
#include "ImageEditHelper.h"
#include <opencv2\imgproc.hpp>

using namespace cv;
using namespace std;

ImageEditHelper::ImageEditHelper()
{
	this->converter = new ConversionHelper();
	this->rectangleMethods = new RectangleHelper();
	this->imageInfo = new ImageInformationHelper();
	this->transformations = new TransformHelper();
}

ImageEditHelper::~ImageEditHelper()
{
	delete converter;
	delete rectangleMethods;
	delete imageInfo;
	delete transformations;
}

void ImageEditHelper::RotateImage(const Mat rawImage, Mat& outImage, const double angleToRotate, const Point2f centerPoint)
{
	//Implemented as suggested at:
	//http://opencvexamples.blogspot.com/2014/01/rotate-image.html

	Mat r = getRotationMatrix2D(centerPoint, angleToRotate, 1.0);

	//Seems like INTER_LANCZOS4 causes the least blur.
	//See https://stackoverflow.com/questions/24953935/opencv-image-getting-blurred-when-rotating-with-warpaffine
	warpAffine(rawImage, outImage, r, Size(rawImage.cols, rawImage.rows), INTER_LANCZOS4);
}

void ImageEditHelper::StraightenUpImage(const Mat rawImage, Mat& outImage, const RotatedRect rawCardArea, Rect2f& outCardArea, bool enforcePortraitMode) {

	//Rotate the image to straighten up the card.
	double angleToRotate = rectangleMethods->GetAnglesToStrightenUp(rawCardArea, enforcePortraitMode);
	RotateImage(rawImage, outImage, angleToRotate, rawCardArea.center);

	//Rotate the card area rectangle.
	outCardArea = converter->ToStraightRectangle(rawCardArea, enforcePortraitMode);
}

void ImageEditHelper::CropImage(const Mat rawImage, Mat& outImage, const Rect cropArea) {

	float centerX = (float)cropArea.x + cropArea.size().width / 2;
	float centerY = (float)cropArea.y + cropArea.size().height / 2;
	Point2f center(centerX, centerY);

	getRectSubPix(rawImage, cropArea.size(), center, outImage);
}

void ImageEditHelper::CropImageWithSolidBorder(const Mat rawImage, Mat& outImage, const Rect cropArea, int borderThickness) {

	//Crop the image.
	CropImage(rawImage, outImage, cropArea);

	//Add the border.
	copyMakeBorder(
		outImage, outImage,
		borderThickness, borderThickness, borderThickness, borderThickness,
		BORDER_ISOLATED, converter->ToScalarColour(White));
}

void ImageEditHelper::ResizeImage(const Mat rawImage, Mat& outImage, int height) {

	float ratio = (float)rawImage.size().height / rawImage.size().width;
	int width = (int)round(height / ratio);
	Size newSize(width, height);

	resize(rawImage, outImage, newSize);
}

void ImageEditHelper::SetBackgroundByInverting(Mat& blackAndWhiteimage, bool setblackBackground) {

	bool isblackOnWhite = imageInfo->IsBlackTextWhiteBackground(blackAndWhiteimage);

	if (setblackBackground && isblackOnWhite ||
		!setblackBackground && !isblackOnWhite) {
		blackAndWhiteimage = ~blackAndWhiteimage;
	}
}
