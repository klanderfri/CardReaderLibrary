#pragma once
//Class relating one point to another point.
class PointToPoint
{
public:
	PointToPoint();
	PointToPoint(cv::Point2d p1, cv::Point2d p2);
	~PointToPoint();

	//The first point.
	cv::Point2d P1();
	//The second point.
	cv::Point2d P2();
	//The distances between the points.
	double Distance();

private:

	//The first point.
	cv::Point2d p1;
	//The second point.
	cv::Point2d p2;
	//A cache for the distances between the points.
	double distance;
};

