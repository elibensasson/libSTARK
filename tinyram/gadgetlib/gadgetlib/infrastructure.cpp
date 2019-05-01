/** @file
*****************************************************************************
Common functionality needed by many components.
*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#include <gadgetlib/infrastructure.hpp>

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <climits>

#ifndef WIN32
#include <unistd.h>
#include <cstdio>
#include <execinfo.h> // backtraces
#endif

namespace gadgetlib {

	/********************************************************/
	/*************** Debug String Formatting ****************/
	/********************************************************/

#ifdef DEBUG
	const static size_t MAX_FMT = 256;
	::std::string GADGETLIB3_FMT(const char* format, ...) {
		char buf[MAX_FMT];
		va_list args;
		va_start(args, format);
#ifdef __linux__
		const int strChk = snprintf(buf, MAX_FMT, format, args);
#else	// #ifdef __linux__
		const int strChk = vsnprintf_s(buf, MAX_FMT, MAX_FMT, format, args);
#endif	// #ifdef __linux__
		va_end(args);
		GADGETLIB_ASSERT(strChk >= 0 && strChk < (int)MAX_FMT, "String length larger than buffer. Shorten"
			" string or increase buffer size defined in \"MAX_FMT\".");
		return ::std::string(buf);
	}
#else // not DEBUG
	::std::string GADGETLIB3_FMT(const char* format, ...) { return ""; }
#endif

	/** Safely converts 64-bit types to 32-bit. */
	long safeConvert(const int64_t num) {
		assert(num <= INT_MAX && num >= INT_MIN);
		return (long)num;
	}

	/*****************************************************************************/
	/***********************  ErrorHandling********** ****************************/
	/*****************************************************************************/

	/*
	TODO add dumping of environment variables and run command to a log file and add log file path
	to release mode error message. We don't want people running release version to get any internal
	information (variable values, stack trace, etc.) but want to have every data possible to
	reproduce assertion.
	*/
	void ErrorHandling::fatalError(const ::std::string& msg) {
#   ifdef DEBUG
		::std::cerr << "ERROR:  " << msg << ::std::endl << ::std::endl;
		printStacktrace();
		throw ::std::runtime_error(msg);
#   else // not DEBUG
		const ::std::string releaseMsg("Fatal error encoutered. Run debug build for more"
			" information and stack trace.");
		::std::cerr << "ERROR:  " << releaseMsg << ::std::endl << ::std::endl;
		throw ::std::runtime_error(releaseMsg);
#   endif
	}

	void ErrorHandling::fatalError(const ::std::stringstream& msg) {
		fatalError(msg.str());
	}

	void ErrorHandling::printStacktrace() {
#ifdef __GNUC__
		std::cerr << "Stack trace (pipe through c++filt to demangle identifiers):" << std::endl;
		const int maxFrames = 100;
		void* frames[maxFrames];
		// Fill array with pointers to stack frames
		int numFrames = backtrace(frames, maxFrames);
		// Decode frames and print them to stderr
		backtrace_symbols_fd(frames, numFrames, STDERR_FILENO);
#else
		//TODO make this available for Windows
		std::cerr << "  (stack trace not available on this platform)" << std::endl;
#endif // __GNUC__
	}

	/*****************************************************************************/
	/****************************  Basic Math  ***********************************/
	/*****************************************************************************/

	double Log2(double n)  {
		return log(n) / log((double)2);
	}

	//returns the ceiling of log2(i) - note that the Log2ceil method currently returns this value  + 1 when i is a power of 2.
	unsigned int Log2ceiled(uint64_t n)  {
		return std::ceil(log(n) / log((double)2));
	}


	/// Returns an upper bound on log2(i). Namely, returns the number of binary digits needed to store
	/// the value 'i'. When i == 0 returns 0.
	unsigned int Log2ceil(uint64_t i) {
		int retval = i ? 1 : 0;
		while (i >>= 1) { ++retval; }
		return retval;
	}

	///Returns true iff x is a power of 2
	bool IsPower2(const long x)  {
		return ((x > 0) && ((x & (x - 1)) == 0));
	}

} // namespace gadgetlib

