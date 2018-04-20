#pragma once
class TrendLine
{
public:
	TrendLine();
	TrendLine(double slope, double offset);
	TrendLine(const std::vector<cv::Point>& points);
	~TrendLine();

	//Gets the end points of the line.
	std::vector<cv::Point2d> GetEndPoints(float leftLimitX, float rightLimitX);
	//Gets the Y value for a given X value.
	double GetY(double x);
	//Gets the X value for a given Y value.
	double GetX(double y);
	//Tells if the line is a zero line, i.e equivalent with most often is the X-axis.
	bool IsZeroLine();
	//Gets the equation in the form 'y = kx + m'.
	std::wstring GetEquation();
	//Gets the equation in the form 'y - kx - m = 0'.
	std::wstring GetZeroEquation();
	//Gets the perpendicular distance between the line and a point.
	double GetPerpendicularDistance(cv::Point point);
	//Gets another line perpendicular to the line.
	TrendLine GetPerpendicularLine(cv::Point pointOnPerpendicularLine);
	//Gets the point in which thw two lines meet.
	static cv::Point2d GetIntersectionPoint(TrendLine lineA, TrendLine lineB);

	//The slope of the line.
	double Slope = 0;
	//The offset of the line.
	double Offset = 0;

private:

	//Sets up the member variables.
	void setupMemberVariables(double slope, double offset);
};

