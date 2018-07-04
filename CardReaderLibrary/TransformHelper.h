#pragma once
class TransformHelper
{
public:
	TransformHelper();
	~TransformHelper();

	//Transforms an image, making a rectangle of the figure with the provided corner points.
	cv::Mat TransformToRectangle(const cv::Mat rawImage, std::vector<cv::Point2d> fourCornerPoints, double sideMarginFactor = 0, double topShiftFactor = 0);

private:

	//Orders the provided corner points in the order top-left, top-right, bottom-right, bottom-left.
	std::vector<cv::Point2d> orderCornerPointsForTransform(std::vector<cv::Point2d> fourUnorderedCornerPoints);
	//Calculates the sum of the coordinates (x+y) of the point.
	double pointCoordinateSum(cv::Point2d point);
	//Calculates the difference of the coordinates (x-y) of the point.
	double pointCoordinateDifference(cv::Point2d point);
};

