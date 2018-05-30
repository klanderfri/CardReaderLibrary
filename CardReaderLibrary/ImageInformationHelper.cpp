#include "stdafx.h"
#include "ImageInformationHelper.h"

using namespace cv;
using namespace std;

ImageInformationHelper::ImageInformationHelper()
{
}

ImageInformationHelper::~ImageInformationHelper()
{
}

Point2f ImageInformationHelper::GetImageCenter(const Mat image) {

	return Point2f(image.cols / (float)2, image.rows / (float)2);
}

double ImageInformationHelper::PercentageOfNonZero(const Mat blackAndWhiteimage) {

	int white = countNonZero(blackAndWhiteimage);
	int size = blackAndWhiteimage.size().area();

	double percentage = white / (float)size;

	return percentage;
}

bool ImageInformationHelper::IsBlackTextWhiteBackground(const Mat blackAndWhiteimage) {

	return PercentageOfNonZero(blackAndWhiteimage) > 0.5;
}
