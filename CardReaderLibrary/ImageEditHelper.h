#pragma once
#include "ConversionHelper.h"
#include "RectangleHelper.h"
#include "ImageInformationHelper.h"
//Class for editing images.
class ImageEditHelper
{
public:
	ImageEditHelper();
	~ImageEditHelper();

	//Crops the image.
	void CropImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea);
	//Crops the image and adds a solid border to it.
	void CropImageWithSolidBorder(const cv::Mat rawImage, cv::Mat& outImage, const cv::Rect cropArea, int borderThickness);
	//Rotates an image.
	void RotateImage(const cv::Mat rawImage, cv::Mat& outImage, const double angleToRotate, const cv::Point2f centerPoint);
	//Straighten up an image so it won't be slightly rotated (it can still be upside down or on the side).
	void StraightenUpImage(const cv::Mat rawImage, cv::Mat& outImage, const cv::RotatedRect rawCardArea, cv::Rect2f& outCardArea, bool enforcePortraitMode = true);
	//Resizes an image to the specified height. The height/width relatio will be preserved.
	void ResizeImage(const cv::Mat rawImage, cv::Mat& outImage, int height);
	//Inverts the image background from black to white or vice versa.
	void SetBackgroundByInverting(cv::Mat& blackAndWhiteimage, bool setblackBackground);

private:

	//Pointer to object doing conversions.
	ConversionHelper * converter;
	//Pointer to object handling rectangles.
	RectangleHelper* rectangleMethods;
	//Pointer to object providing image information.
	ImageInformationHelper* imageInfo;
};

