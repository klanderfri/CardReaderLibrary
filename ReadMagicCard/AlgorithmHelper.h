#pragma once
#include "TrendLine.h"
#include <vector>
//Class holding algorithm methods.
class AlgorithmHelper
{
public:
	AlgorithmHelper();
	~AlgorithmHelper();

	//Tells if a vector contains the specified object.
	template<typename T> static bool VectorContains(std::vector<T> heystack, T needle);
	//Joins two vectors.
	template<typename T> static std::vector<T> JoinVectors(const std::vector<T> vectorA, const std::vector<T> vectorB);
	//Appends a vector to another.
	template<typename T> static void AppendVector(std::vector<T>& mainVector, const std::vector<T> toAppend);
	//Finds the trend line slope for a set of values.
	static long double Slope(const std::vector<long double>& x, const std::vector<long double>& y);
	//Finds the avarage value for a set of numbers.
	static long double Average(const std::vector<long double>& numbers);
	//Finds the point within a set that has a X coordinate closes to the provided X coordinate.
	static cv::Point2f FindClosestPointX(double x, std::vector<cv::Point2f> points);
	//Finds the distance between two points.
	static double FindDistance(cv::Point2d point1, cv::Point2d point2);
	//Creates the rectangle bound by the the specified borders.
	static cv::RotatedRect GetRotatedRectangle(std::vector<TrendLine> verticalBorders, std::vector<TrendLine> horizontalBorders, double angleAdjustment = 0.0);
};

template<typename T>
inline bool AlgorithmHelper::VectorContains(std::vector<T> heystack, T needle)
{
	return find(heystack.begin(), heystack.end(), needle) != heystack.end();
}

template<typename T>
inline std::vector<T> AlgorithmHelper::JoinVectors(const std::vector<T> vectorA, const std::vector<T> vectorB)
{
	//Implemented as suggested at:
	//https://stackoverflow.com/a/3177252/1997617

	std::vector<T> joinedVector;

	joinedVector.reserve(vectorA.size() + vectorB.size());
	joinedVector.insert(joinedVector.end(), vectorA.begin(), vectorA.end());
	joinedVector.insert(joinedVector.end(), vectorB.begin(), vectorB.end());

	return joinedVector;
}

template<typename T>
inline void AlgorithmHelper::AppendVector(std::vector<T>& mainVector, const std::vector<T> toAppend)
{
	mainVector.insert(mainVector.end(), toAppend.begin(), toAppend.end());
}
