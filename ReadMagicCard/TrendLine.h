#pragma once
class TrendLine
{
public:
	TrendLine();
	TrendLine(double slope, double offset);
	~TrendLine();

	//Gets the end points of the line.
	std::vector<cv::Point2f> GetEndPoints(float leftLimitX, float rightLimitX);
	//Gets the Y value for a given X value.
	double GetY(double x);
	//Gets the X value for a given Y value.
	double GetX(double y);
	//Tells if the line is a zero line, i.e equivalent with most often is the X-axis.
	bool IsZeroLine();

	//The slope of the line.
	double Slope = 0;
	//The offset of the line.
	double Offset = 0;
};

