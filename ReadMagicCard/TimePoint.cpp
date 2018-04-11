#include "stdafx.h"
#include "TimePoint.h"

using namespace std;

long long TimePoint::GetExecutionTime(TimePoint startTime, TimePoint endTime) {

	return chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
}
