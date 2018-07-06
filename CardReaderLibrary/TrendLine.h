#pragma once
//Class representing a straight line.
class TrendLine
{
public:
	TrendLine();
	//Creates a line with the specified slope and offset.
	TrendLine(long double slope, long double offset);
	//Creates a trend line for the provided points.
	TrendLine(const std::vector<cv::Point2d>& points);
	//Creates a line throught the provided point with the specified slope (provided in degrees, clockwise, related to the X-axis).
	TrendLine(const double slopeInDegrees, const cv::Point2d point);
	~TrendLine();

	//Gets the end points of the line.
	std::vector<cv::Point2d> GetEndPoints(long double leftLimitX, long double rightLimitX);
	//Gets the Y value for a given X value.
	long double GetY(long double x);
	//Gets the X value for a given Y value.
	long double GetX(long double y);
	//Tells if the line is a zero line, i.e equivalent with most often is the X-axis.
	bool IsZeroLine();
	//Gets the equation in the form 'y = kx + m'.
	std::wstring GetEquation();
	//Gets the equation in the form 'y - kx - m = 0'.
	std::wstring GetZeroEquation();
	//Gets the perpendicular distance between two lines.
	static long double GetPerpendicularDistance(TrendLine lineA, TrendLine lineB);
	//Gets the perpendicular distance between the line and a point.
	long double GetPerpendicularDistance(cv::Point2d point);
	//Gets the vertical distance between the line and a point.
	long double GetVerticalDistance(cv::Point2d point);
	//Gets another line perpendicular to the line.
	TrendLine GetPerpendicularLine(cv::Point2d pointOnPerpendicularLine);
	//Gets another line parallel to the line.
	TrendLine GetParallelLine(cv::Point2d pointOnParallelLine);
	//Gets the point in which the two lines meet.
	static cv::Point2d GetIntersectionPoint(TrendLine lineA, TrendLine lineB);
	//Gets the clockwise angle (degrees) from the norm line to the relation line.
	static long double GetDegreesBetweenLines(TrendLine normLine, TrendLine relationLine);
	//Gets the angle (degrees) between the line and the X axis.
	long double GetDegreesToAxisX();
	//Gets the two lines parallel to the line, having all the points between them.
	std::vector<TrendLine> GetBoundLines(std::vector<cv::Point2d> points);
	//Tells if two lines are parallel.
	static bool IsParallel(TrendLine lineA, TrendLine lineB);

	//The slope of the line.
	long double Slope = 0;
	//The offset of the line.
	long double Offset = 0;

	//The line representing the X axis.
	static TrendLine XAxis;

private:

	//Sets up the member variables.
	void setupMemberVariables(long double slope, long double offset);
	//Converts radians to degrees.
	static long double degreesFromRadians(long double value);
	//Converts degrees to radians;
	static long double radiansFromDegrees(long double degrees);
};

