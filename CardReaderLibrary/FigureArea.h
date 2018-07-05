#pragma once
#include "Contour.h"
//Class holding information about the area of a figure.
class FigureArea
{
public:
	FigureArea();
	~FigureArea();

	//Compares two figure areas by the X-coordinate of the left border when the figures are oriented as portrait.
	static bool CompareFigureAreaByLeftBorderXAscending(FigureArea area1, FigureArea area2);
	//Compares two figure areas by the height of the bounding rectangle.
	static bool CompareFigureAreaByHeightAscending(FigureArea area1, FigureArea area2);
	//Gets the point marking the middle of the bottom line of the figure box.
	cv::Point2d GetMiddleBottomPoint();

	//The outer contour which wraps the outer limits of the figure (finger tip to finger tip).
	Contour OuterContour;
	//The contour that only wraps the actual figure part (finger tip to finger valley to finger tip).
	Contour TightContour;
	//The box containing the figure.
	cv::RotatedRect Box;
};
