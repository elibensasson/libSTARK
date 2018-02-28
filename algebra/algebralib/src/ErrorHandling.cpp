/** @file
*****************************************************************************
Common functionality needed by many components.
*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#include <cstdint>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <climits>

#include "algebraLib/ErrorHandling.hpp"


#ifdef __linux__
#include <unistd.h>
#include <cstdio>
#endif
#ifdef __GLIBC__
#include <execinfo.h> // backtraces
#endif

namespace Algebra{

	/********************************************************/
	/*************** Debug String Formatting ****************/
	/********************************************************/

#ifdef DEBUG
	const static size_t MAX_FMT = 256;
	::std::string ALGEBRALIB_FMT(const char* format, ...) {
		char buf[MAX_FMT];
		va_list args;
		va_start(args, format);
#ifdef __linux__
		const int strChk = snprintf(buf, MAX_FMT, format, args);
#else	// #ifdef __linux__
		const int strChk = vsnprintf_s(buf, MAX_FMT, MAX_FMT, format, args);
#endif	// #ifdef __linux__
		va_end(args);
		ALGEBRALIB_ASSERT(strChk >= 0 && (unsigned int)strChk < MAX_FMT, "String length larger than buffer. Shorten"
			" string or increase buffer size defined in \"MAX_FMT\".");
		return ::std::string(buf);
	}
#else // not DEBUG
	::std::string ALGEBRALIB_FMT(const char* format, ...) { return ""; }
#endif


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

} // namespace Algebra

