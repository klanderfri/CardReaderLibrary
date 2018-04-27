#pragma once
#include <opencv2\core.hpp>
//Type used for specifying a contour (using int).
typedef std::vector<cv::Point> Contour;
//Type used for specifying a contour (using double).
typedef std::vector<cv::Point2d> DblContour;
//Type used as a collection of contours (using int).
typedef std::vector<Contour> Contours;
//Type used as a collection of contours (using double).
typedef std::vector<DblContour> DblContours;
