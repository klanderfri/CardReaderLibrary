#include "stdafx.h"
#include "ImageHelper.h"
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

const int ImageHelper::DEBUG_BORDER_THICKNESS = 1;
const int ImageHelper::DEBUG_BORDER_LINE_TYPE = 8;

ImageHelper::ImageHelper()
{
}

ImageHelper::~ImageHelper()
{
}

Rect2f ImageHelper::ToStraightRectangle(const RotatedRect rawRectangle, bool enforcePortraitMode) {

	//Get the size of the rectangle.
	float width = rawRectangle.size.width;
	float height = rawRectangle.size.height;

	//Change to portait.
	if (enforcePortraitMode && width > height) {
		swap(width, height);
	}

	//Caclulate the coordinates.
	float x = rawRectangle.center.x - (width / 2);
	float y = rawRectangle.center.y - (height / 2);

	return Rect2f(x, y, width, height);
}

RotatedRect ImageHelper::ToRotatedRectangle(const Rect straightRectangle) {

	float x = straightRectangle.width / (float)2 + straightRectangle.x;
	float y = straightRectangle.height / (float)2 + straightRectangle.y;
	Point2f center(x, y);
	
	float width = (float)straightRectangle.width;
	float height = (float)straightRectangle.height;
	Size2f size(width, height);
	
	float angle = 0;
	
	RotatedRect rotatedRectangle(center, size, angle);

	return rotatedRectangle;
}

Mat ImageHelper::ToGreyImage(const Mat imageToConvert) {

	return changeImageMode(imageToConvert, COLOR_BGR2GRAY);
}

Mat ImageHelper::ToColourImage(const Mat imageToConvert) {

	return changeImageMode(imageToConvert, COLOR_GRAY2BGR);
}

RotatedRect ImageHelper::GetRotatedRectangle(vector<TrendLine> verticalBorders, vector<TrendLine> horizontalBorders, double angleAdjustment) {

	//Make sure the borders are correct.
	bool isRightSize = verticalBorders.size() == 2 && horizontalBorders.size() == 2;
	if (!isRightSize) {
		throw ParameterException("Wrong number of borders! There should be exactly 2 vertical borders and 2 horizontal borders!");
	}

	bool isParallel = TrendLine::IsParallel(verticalBorders[0], verticalBorders[1]);
	isParallel = TrendLine::IsParallel(horizontalBorders[0], horizontalBorders[1]) && isParallel;
	if (!isParallel) {
		throw ParameterException("The side borders are not paralell!");
	}

	long double angle1 = TrendLine::GetAngleBetweenLines(verticalBorders[0], horizontalBorders[0]);
	long double angle2 = TrendLine::GetAngleBetweenLines(verticalBorders[1], horizontalBorders[1]);
	bool isPerpendicular = abs(angle1) == 90.0 && abs(angle2) == 90;
	if (!isPerpendicular) {
		throw ParameterException("The horizontal and vertical borders are not perpendicular!");
	}

	//Get the corners.
	Point2d corner = TrendLine::GetIntersectionPoint(horizontalBorders[0], verticalBorders[0]);
	Point2d oppositeCorner = TrendLine::GetIntersectionPoint(horizontalBorders[1], verticalBorders[1]);

	//Calculate center.
	Point2d center = 0.5f * (corner + oppositeCorner);

	//Calculate angle.
	long double angle = (-1) * horizontalBorders[0].GetAngleToAxisX();
	angle += angleAdjustment;

	//Calculate size.
	long double height = TrendLine::GetPerpendicularDistance(horizontalBorders[0], horizontalBorders[1]);
	long double width = TrendLine::GetPerpendicularDistance(verticalBorders[0], verticalBorders[1]);
	Size2d size(width, height);

	//Create rectangle.
	RotatedRect rectangle(center, size, (float)angle);

	return rectangle;
}

Mat ImageHelper::changeImageMode(const Mat imageToConvert, ColorConversionCodes conversionCode) {

	Mat convertedImage(imageToConvert.size(), CV_8UC3);
	int amountOfChannels = imageToConvert.channels();

	if (conversionCode == COLOR_GRAY2BGR && amountOfChannels == 3 ||
		conversionCode == COLOR_BGR2GRAY && amountOfChannels == 1) {

		//The image already has the correct mode.
		imageToConvert.copyTo(convertedImage);
	}
	else {

		//Convert the colour mode.
		cvtColor(imageToConvert, convertedImage, conversionCode);
	}

	return convertedImage;
}

int ImageHelper::ShowImage(const Mat imageToShow) {

	//Create a window for display.
	namedWindow("Display Window", WINDOW_AUTOSIZE);

	//Show our image inside it.
	imshow("Display Window", imageToShow);

	return 0;
}

void ImageHelper::RotateImage(const Mat rawImage, Mat& outImage, const double angleToRotate, const Point2f centerPoint)
{
	//Implemented as suggested at:
	//http://opencvexamples.blogspot.com/2014/01/rotate-image.html

	Mat r = getRotationMatrix2D(centerPoint, angleToRotate, 1.0);

	//Seems like INTER_LANCZOS4 causes the least blur.
	//See https://stackoverflow.com/questions/24953935/opencv-image-getting-blurred-when-rotating-with-warpaffine
	warpAffine(rawImage, outImage, r, Size(rawImage.cols, rawImage.rows), INTER_LANCZOS4);
}

Mat ImageHelper::DrawLine(const Mat image, Point point1, Point point2) {
	
	//Create a working RGB image so the border lines are colour even if the image isn't.
	Mat outImage = ToColourImage(image);

	Scalar lineColour = Scalar(180, 0, 180); //Purple
	line(outImage, point1, point2, lineColour, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE);

	return outImage;
}

Mat ImageHelper::DrawLine(const Mat image, TrendLine line) {

	vector<Point2d> points = line.GetEndPoints(0, image.cols);
	Mat outImage = DrawLine(image, points[0], points[1]);

	return outImage;
}

Mat ImageHelper::DrawLimits(const Mat image, const RotatedRect rotatedLimitRectangle, const Rect straightLimitRectangle, const Contour limitContour)
{
	//Create a working RGB image so the border lines are colour even if the image isn't.
	Mat workingImage = ToColourImage(image);

	//Create colours to indicate the border lines' types.
	Scalar colourContour = Scalar(0, 255, 255); //Yellow
	Scalar colourStraightRectangle = Scalar(0, 120, 0); //Green
	Scalar colourRotatedRectangle = Scalar(180, 130, 70); //Blue
														  //Scalar(BLUE, GREEN, RED)

	//Draw the contours.
	if (!limitContour.empty()) {
		drawContours(workingImage, Contours{ limitContour }, 0, colourContour, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE, Hierarchy(), 0, Point());
	}
	//Draw the straight rectangle.
	if (!straightLimitRectangle.empty()) {
		rectangle(workingImage, straightLimitRectangle.tl(), straightLimitRectangle.br(), colourStraightRectangle, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE, 0);
	}
	//Draw the rotated rectangle
	Point2f rect_points[4];
	rotatedLimitRectangle.points(rect_points);
	for (int j = 0; j < 4; j++) {
		line(workingImage, rect_points[j], rect_points[(j + 1) % 4], colourRotatedRectangle, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE);
	}

	return workingImage;
}

Mat ImageHelper::DrawLimits(const Mat image, const Contours contours, Hierarchy hierarchy, bool useRandomColours) {

	RNG rng(time(0));
	Mat drawing = ToColourImage(image);

	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar colour;
		if (useRandomColours) {
			colour = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		}
		else {
			colour = Scalar(0, 0, 200);
		}

		drawContours(drawing, contours, i, colour, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE, hierarchy, 0, Point());
	}

	return drawing;
}

Mat ImageHelper::DrawLimits(const Mat image, const LetterAreas letters, int letterCenterRadius) {

	Mat drawing = ToColourImage(image);

	for (size_t i = 0; i < letters.size(); i++) {

		LetterArea area = letters[i];
		
		//Draw the area.
		drawing = drawing.empty() ? image : drawing;
		drawing = ImageHelper::DrawLimits(drawing, area.Box, Rect(), area.OuterContour);

		//Draw the center.
		Scalar colour = Scalar(255, 0, 0); //Blue
		drawing = ImageHelper::DrawCenterPoint(drawing, area.Box.center, colour, letterCenterRadius);
	}

	return drawing;
}

Mat ImageHelper::DrawCenterPoint(const Mat image, const Point imageCenter, Scalar colour, int radius) {

	//Create a working rgb image so the border lines are colour even if the image isn't.
	Mat workingImage = ToColourImage(image);

	circle(workingImage, imageCenter, radius, colour, DEBUG_BORDER_THICKNESS, DEBUG_BORDER_LINE_TYPE);

	return workingImage;
}

double ImageHelper::GetAnglesToStrightenUp(const RotatedRect rotatedRectangle, bool enforcePortraitMode) {

	if (enforcePortraitMode) {

		return (rotatedRectangle.size.height < rotatedRectangle.size.width) ? rotatedRectangle.angle + 90 : rotatedRectangle.angle;
	}
	else {

		double rotateAlternative1 = rotatedRectangle.angle; //Should always be negative.
		double rotateAlternative2 = rotatedRectangle.angle + 90; //Should always be positive.
		double smallestRotation = abs(rotateAlternative1) < rotateAlternative2 ? rotateAlternative1 : rotateAlternative2;

		return smallestRotation;
	}
}

Point2f ImageHelper::GetImageCenter(const Mat image) {

	return Point2f(image.cols / (float)2, image.rows / (float)2);
}

void ImageHelper::StraightenUpImage(const Mat rawImage, Mat& outImage, const RotatedRect rawCardArea, Rect2f& outCardArea, bool enforcePortraitMode) {

	//Rotate the image to straighten up the card.
	double angleToRotate = GetAnglesToStrightenUp(rawCardArea, enforcePortraitMode);
	RotateImage(rawImage, outImage, angleToRotate, rawCardArea.center);

	//Rotate the card area rectangle.
	outCardArea = ToStraightRectangle(rawCardArea, enforcePortraitMode);
}

void ImageHelper::CropImage(const Mat rawImage, Mat& outImage, const Rect cropArea) {

	float centerX = (float)cropArea.x + cropArea.size().width / 2;
	float centerY = (float)cropArea.y + cropArea.size().height / 2;
	Point2f center(centerX, centerY);

	getRectSubPix(rawImage, cropArea.size(), center, outImage);
}

void ImageHelper::CropImageWithSolidBorder(const Mat rawImage, Mat& outImage, const Rect cropArea, int borderThickness) {

	//Crop the image.
	CropImage(rawImage, outImage, cropArea);

	//Add the border.
	copyMakeBorder(
		outImage, outImage,
		borderThickness, borderThickness, borderThickness, borderThickness,
		BORDER_ISOLATED, Scalar(255));
}

void ImageHelper::ResizeImage(const Mat rawImage, Mat& outImage, int height) {

	float ratio = (float)rawImage.size().height / rawImage.size().width;
	int width = (int)round(height / ratio);
	Size newSize(width, height);

	resize(rawImage, outImage, newSize);
}

double ImageHelper::PercentageOfNonZero(const Mat blackAndWhiteimage) {

	int white = countNonZero(blackAndWhiteimage);
	int size = blackAndWhiteimage.size().area();

	double percentage = white / (float)size;

	return percentage;
}

bool ImageHelper::IsBlackTextWhiteBackground(const Mat blackAndWhiteimage) {

	return PercentageOfNonZero(blackAndWhiteimage) > 0.5;
}

void ImageHelper::SetBackgroundByInverting(Mat& blackAndWhiteimage, bool setblackBackground) {

	bool isblackOnWhite = IsBlackTextWhiteBackground(blackAndWhiteimage);

	if (setblackBackground && isblackOnWhite ||
		!setblackBackground && !isblackOnWhite) {
		blackAndWhiteimage = ~blackAndWhiteimage;
	}
}

float ImageHelper::SmallestDistanceCenterToLimit(RotatedRect rectangle) {

	return min(rectangle.size.height, rectangle.size.width) / 2;
}

Mat ImageHelper::GetContourAreaFromImage(const Contour contour, const Mat rawImage, const int margins, const bool drawContour) {

	//This method crashes when the cut area goes outside the image.
	//But that probably won't happen often so we'll cross that bridge when we get to it.

	Mat outImage;
	rawImage.copyTo(outImage);

	if (drawContour) {
		Contours contours{ contour };
		outImage = DrawLimits(outImage, contours, Hierarchy(), false);
	}

	Rect bounds = boundingRect(Mat(contour));
	Rect cutArea(bounds.x - margins, bounds.y - margins, bounds.width + margins * 2, bounds.height + margins * 2);
	CropImage(outImage, outImage, cutArea);

	return outImage;
}

Contours ImageHelper::GetCannyContours(const Mat greyScaleImage, int thresh) {

	Contours contours;
	Hierarchy hierarchy;
	Mat canny;

	Canny(greyScaleImage, canny, thresh, thresh * 2, 3);
	findContours(canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	return contours;
}

bool ImageHelper::IsIdenticalContours(Contour contour1, Contour contour2) {

	if (contour1.size() != contour2.size()) { return false; }

	for (size_t i = 0; i < contour1.size(); i++) {

		Point point1 = contour1[i];
		Point point2 = contour2[i];

		if (point1 != point2) { return false; }
	}

	return true;
}

bool ImageHelper::IsInitialized(RotatedRect rectangleToCheck) {

	float sum = abs(rectangleToCheck.angle)
		+ abs(rectangleToCheck.center.x)
		+ abs(rectangleToCheck.center.y)
		+ abs(rectangleToCheck.size.area());

	return sum != 0;
}

bool ImageHelper::DoesRectangleContainPoint(RotatedRect rectangle, Point2f point) {

	//Implemented as suggested at:
	//http://answers.opencv.org/question/30330/check-if-a-point-is-inside-a-rotatedrect/?answer=190773#post-id-190773

	//Get the corner points.
	Point2f corners[4];
	rectangle.points(corners);

	//Convert the point array to a vector.
	//https://stackoverflow.com/a/8777619/1997617
	Point2f* lastItemPointer = (corners + sizeof corners / sizeof corners[0]);
	vector<Point2f> contour(corners, lastItemPointer);

	//Check if the point is within the rectangle.
	double indicator = pointPolygonTest(contour, point, false);
	bool rectangleContainsPoint = (indicator >= 0);

	return rectangleContainsPoint;
}

Contour ImageHelper::ContoursConvexHull(const Contours contours)
{
	//Implemented as suggested at:
	//http://answers.opencv.org/question/74777/how-to-use-approxpolydp-to-close-contours/

	Contour result;
	Contour pts;

	for (size_t i = 0; i < contours.size(); i++) {

		for (size_t j = 0; j < contours[i].size(); j++) {

			pts.push_back(contours[i][j]);
		}
	}

	convexHull(pts, result);

	return result;
}

Contour ImageHelper::GetCombinedLetterContorus(LetterAreas letters) {

	Contour combinedLetterContorus;

	for (size_t i = 0; i < letters.size(); i++) {

		LetterArea area = letters[i];

		combinedLetterContorus.insert(combinedLetterContorus.end(), area.OuterContour.begin(), area.OuterContour.end());
	}

	return combinedLetterContorus;
}

Mat ImageHelper::GetContourDrawing(const Contour contour) {

	Rect2i bounds = boundingRect(contour);
	int margins = (int)ceil(min(bounds.width, bounds.height) * 0.1);

	Mat contourImage;
	contourImage.create(bounds.y + bounds.height + 1, bounds.x + bounds.width + 1, 0);
	contourImage = DrawLimits(contourImage, Contours{ contour }, Hierarchy(), false);

	CropImage(contourImage, contourImage, bounds);
	
	return contourImage;
}

DblContours ImageHelper::ConvertToDoubleFromInt(Contours iContours) {

	DblContours dContours;

	for (Contour iContour : iContours) {

		DblContour dContour = ConvertToDoubleFromInt(iContour);
		dContours.push_back(dContour);
	}

	return dContours;
}

Contours ImageHelper::ConvertToIntFromDouble(DblContours dContours) {

	vector<Contour> iContours;

	for (DblContour dContour : dContours) {

		Contour iContour = ConvertToIntFromDouble(dContour);
		iContours.push_back(iContour);
	}

	return iContours;
}

DblContour ImageHelper::ConvertToDoubleFromInt(Contour iContour) {

	DblContour dContour;

	for (Point iPoint : iContour) {

		Point2d dPoint = iPoint;
		dContour.push_back(dPoint);
	}

	return dContour;
}

Contour ImageHelper::ConvertToIntFromDouble(DblContour dContour) {

	Contour iContour;

	for (Point dPoint : dContour) {

		int x = (int)round(dPoint.x);
		int y = (int)round(dPoint.y);
		Point2i iPoint(x, y);
		iContour.push_back(iPoint);
	}

	return iContour;
}
