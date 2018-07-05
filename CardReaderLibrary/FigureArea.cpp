#include "stdafx.h"
#include "FigureArea.h"
#include "RectangleHelper.h"

using namespace cv;
using namespace std;

FigureArea::FigureArea()
{
}

FigureArea::~FigureArea()
{
}

bool FigureArea::CompareFigureAreaByLeftBorderXAscending(FigureArea area1, FigureArea area2) {

	float borderX1 = area1.Box.center.x - RectangleHelper::SmallestDistanceCenterToLimit(area1.Box);
	float borderX2 = area2.Box.center.x - RectangleHelper::SmallestDistanceCenterToLimit(area2.Box);

	return (borderX1 < borderX2);
}

bool FigureArea::CompareFigureAreaByHeightAscending(FigureArea area1, FigureArea area2) {

	return (area1.Box.size.height < area2.Box.size.height);
}

Point2d FigureArea::GetMiddleBottomPoint() {

	//Implemented according to section "But which one?" at:
	//http://www.mathsisfun.com/algebra/trig-inverse-sin-cos-tan.html

	double height = Box.size.height;
	double angle = Box.angle;
	if (angle < -45) {

		height = Box.size.width;
		angle += 90.0;
	}

	double hypotenuse = height / 2;
	double angleFactor = (-1) * sin(angle * CV_PI / 180);
	double xdistance = angleFactor * hypotenuse;
	double yDistance = sqrt(pow(hypotenuse, 2) - pow(xdistance, 2));

	double x = Box.center.x + xdistance;
	double y = Box.center.y + yDistance;
	Point2d bottomMiddle(x, y);

	return bottomMiddle;
}
