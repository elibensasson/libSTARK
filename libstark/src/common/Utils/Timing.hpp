#ifndef  __Timing_HPP
#define  __Timing_HPP

#include <ctime>

namespace libstark {

/********************************************************/
/*******************     Timing    **********************/
/********************************************************/

/**
 * A lightweight class for elapsed-time measurements
 */
class Timer {
	private:
#ifdef __linux__
		struct timespec startTimeSpec;
#else
		clock_t startTime;
#endif
	public:
		Timer();

		/** Elapsed time in seconds */
		double getElapsed();
};

} // namespace libstark 

#endif   // __Timing_HPP
