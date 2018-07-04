#include "stdafx.h"
#include "PointToPoint.h"
#include "AlgorithmHelper.h"

using namespace cv;
using namespace std;

PointToPoint::PointToPoint()
{
}

PointToPoint::PointToPoint(Point2d p1, Point2d p2)
{
	this->p1 = p1;
	this->p2 = p2;
	this->distance = -1;
}

PointToPoint::~PointToPoint()
{
}

Point2d PointToPoint::P1() {

	return p1;
}

Point2d PointToPoint::P2() {

	return p2;
}

double PointToPoint::Distance() {

	if (distance < 0) {

		distance = AlgorithmHelper::FindDistance(p1, p2);
	}

	return distance;
}
