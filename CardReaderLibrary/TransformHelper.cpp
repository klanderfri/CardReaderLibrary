#include "stdafx.h"
#include "TransformHelper.h"
#include "AlgorithmHelper.h"
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

TransformHelper::TransformHelper()
{
}

TransformHelper::~TransformHelper()
{
}

Mat TransformHelper::TransformToRectangle(const Mat rawImage, vector<Point2d> fourCornerPoints, double widthMarginFactor, double heightMarginFactor) {

	//Implemented according to:
	//https://www.pyimagesearch.com/2014/08/25/4-point-opencv-getperspective-transform-example/

	if (fourCornerPoints.size() != 4) {
		throw new ParameterException("The method was not provided exactly four corner points!", "fourCornerPoints");
	}

	vector<Point2d> originalCorners = orderCornerPointsForTransform(fourCornerPoints);
	Point2d tl = originalCorners[0];
	Point2d tr = originalCorners[1];
	Point2d br = originalCorners[2];
	Point2d bl = originalCorners[3];

	double width = max(AlgorithmHelper::FindDistance(tl, tr), AlgorithmHelper::FindDistance(bl, br));
	double height = max(AlgorithmHelper::FindDistance(tl, bl), AlgorithmHelper::FindDistance(tr, br));

	int widthMargin = width * widthMarginFactor;
	int heightMargin = height * heightMarginFactor;

	Size2d newImageSize(width + 2 * widthMargin, height + 2 * heightMargin);

	Point2f oldCorners[] = { tl, tr, br, bl };
	Point2f newCorners[] =
	{
		Point2f(0 + widthMargin, 0 + heightMargin),
		Point2f(width - 1 + widthMargin, 0 + heightMargin),
		Point2f(width - 1 + widthMargin, height - 1 + heightMargin),
		Point2f(0 + widthMargin, height - 1 + heightMargin)
	};

	Mat transformedImage;
	Mat transformationMatrix = getPerspectiveTransform(oldCorners, newCorners);
	warpPerspective(rawImage, transformedImage, transformationMatrix, newImageSize);

	return transformedImage;
}

vector<Point2d> TransformHelper::orderCornerPointsForTransform(vector<Point2d> fourUnorderedCornerPoints) {

	//Implemented according to:
	//https://www.pyimagesearch.com/2014/08/25/4-point-opencv-getperspective-transform-example/

	Point2d topLeft = fourUnorderedCornerPoints[0];
	Point2d bottomRight = fourUnorderedCornerPoints[1];
	Point2d topRight = fourUnorderedCornerPoints[2];
	Point2d bottomLeft = fourUnorderedCornerPoints[3];

	for (Point2d p : fourUnorderedCornerPoints) {

		double pSum = pointCoordinateSum(p);
		double pDifference = pointCoordinateDifference(p);

		double tlSum = pointCoordinateSum(topLeft);
		double brSum = pointCoordinateSum(bottomRight);
		double trDifference = pointCoordinateDifference(topRight);
		double blDifference = pointCoordinateDifference(bottomLeft);

		if (pSum < tlSum) {
			topLeft = p;
		}
		if (pSum > brSum) {
			bottomRight = p;
		}
		if (pDifference < blDifference) {
			bottomLeft = p;
		}
		if (pDifference > trDifference) {
			topRight = p;
		}
	}

	vector<Point2d> corners;
	corners.push_back(topLeft);
	corners.push_back(topRight);
	corners.push_back(bottomRight);
	corners.push_back(bottomLeft);

	return corners;
}

double TransformHelper::pointCoordinateSum(Point2d point) {

	return point.x + point.y;
}

double TransformHelper::pointCoordinateDifference(Point2d point) {

	return point.x - point.y;
}
