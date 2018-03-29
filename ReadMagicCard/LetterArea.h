#pragma once
#include "Contour.h"
//The area of a letter.
class LetterArea
{
public:
	LetterArea();
	~LetterArea();

	//Compares two letter areas by the box center point X-coordinate.
	static bool CompareLetterAreaByXAscending(LetterArea area1, LetterArea area2);

	//The contour of the letter.
	Contour contour;
	//The box containing the letter.
	cv::RotatedRect box;
};
typedef std::vector<LetterArea> LetterAreas;
