#pragma once
#include "Contour.h"
//Class holding information about the area of a letter.
class LetterArea
{
public:
	LetterArea();
	~LetterArea();

	//Compares two letter areas by the X-coordinate of the left border when the letters are standing stright up.
	static bool CompareLetterAreaByLeftBorderXAscending(LetterArea area1, LetterArea area2);
	//Compares two letter areas by the height of the bounding rectangle.
	static bool CompareLetterAreaByHeightAscending(LetterArea area1, LetterArea area2);
	//Gets the point marking the middle of the bottom line of the letter box.
	cv::Point2d GetMiddleBottomPoint();

	//The outer contour which wraps the outer limits of the letter.
	Contour OuterContour;
	//The contour that only wraps the actual letter part.
	Contour TightContour;
	//The box containing the letter.
	cv::RotatedRect Box;
};
