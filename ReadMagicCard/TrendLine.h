#pragma once
class TrendLine
{
public:
	TrendLine();
	TrendLine(double slope, double offset);
	~TrendLine();

	std::vector<cv::Point2f> GetEndPoints(float leftLimitX, float rightLimitX);

	double GetY(double x);

	double GetX(double y);

	bool IsZeroLine();

	//The slope of the line.
	double Slope = 0;
	//The offset of the line.
	double Offset = 0;
};

