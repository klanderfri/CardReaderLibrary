#pragma once
#include <chrono>
typedef std::chrono::time_point<std::chrono::steady_clock> ChronoTimePoint;
//A chrono time point for steady clock.
class TimePoint
	: ChronoTimePoint
{
public:
	TimePoint(const ChronoTimePoint &x) : ChronoTimePoint(x) {}
	//Extracts the time (in milliseconds) that passed between two time points.
	static long long GetExecutionTime(TimePoint startTime, TimePoint endTime);
};
