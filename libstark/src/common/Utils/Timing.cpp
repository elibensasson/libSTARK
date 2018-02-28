#include "Timing.hpp"

namespace libstark {

/********************************************************/
/*******************     Timing    **********************/
/********************************************************/

Timer::Timer() {
#ifdef __linux__
	clock_gettime(CLOCK_MONOTONIC, &startTimeSpec);
#else
	startTime = clock(); // wallclock on Windows, process time (total over threads) on Linux
#endif
}

double Timer::getElapsed() {
#ifdef __linux__
	struct timespec endTimeSpec;
	clock_gettime(CLOCK_MONOTONIC, &endTimeSpec);
	double elapsed = (endTimeSpec.tv_sec - startTimeSpec.tv_sec);
	elapsed += (endTimeSpec.tv_nsec - startTimeSpec.tv_nsec) / 1000000000.0;
	return elapsed;
#else
	clock_t endTime = clock();
	return (double(endTime) - double(startTime)) / CLOCKS_PER_SEC;
#endif
}

} // namespace libstark 
