#pragma once
//Class for providing image information.
class ImageInformationHelper
{
public:
	ImageInformationHelper();
	~ImageInformationHelper();

	//Gets the center point of an image.
	cv::Point2f GetImageCenter(const cv::Mat image);
	//Gives the percentage of pixels that are not zero.
	double PercentageOfNonZero(const cv::Mat blackAndWhiteimage);
	//Checks if the image is black text on white background, i.e if it is mostly white.
	bool IsBlackTextWhiteBackground(const cv::Mat blackAndWhiteimage);
};

