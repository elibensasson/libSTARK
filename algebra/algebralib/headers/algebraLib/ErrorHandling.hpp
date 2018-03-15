/** @file
*****************************************************************************
Common functionality needed by many components.
*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#ifndef  ALGEBRALIB_ERRORHANDALING_HPP_
#define  ALGEBRALIB_ERRORHANDALING_HPP_

#include <cstdint>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdarg>

#ifndef _MSC_VER // emulate the MSVC-specific sprintf_s using the standard snprintf
#define sprintf_s snprintf //TODO: sprintf_s!=snprintf (http://blog.verg.es/2008/09/sprintfs-is-not-snprintf.html)
#endif

#ifdef _DEBUG // MSVC Debug build
#define DEBUG // gcc Debug flag
#endif

namespace Algebra{
	// someday, if/when MSVC supports C++0x variadic templates, change FMT in release version to the
	// following in order to increase efficiency:
	// #define GADGETLIB3_FMT(...) ""
	::std::string ALGEBRALIB_FMT(const char* format, ...);


	/********************************************************/
	/******************* Error Handling *********************/
	/********************************************************/

	// declare a function as never returning, to quiet down "control reaches end of non-void function" warnings
	#if defined(_MSC_VER) // VisualC++
	#define __noreturn _declspec(noreturn)
	#elif defined(__GNUC__)
	#define __noreturn __attribute__((noreturn))
	#else
	#define __noreturn
	#endif

	/**
	* The ErrorHandling class contains and implements the functionality of displaying the content of error
	* messages (including content of call stack when error happened), and exiting the program.
	*/
	class ErrorHandling {
	public:
		static void __noreturn fatalError(const ::std::string& msg);
		static void __noreturn fatalError(const std::stringstream& msg);
		static void printStacktrace();

	};

#define ALGEBRALIB_FATAL(msg) do {  \
	::std::stringstream msgStream; \
	msgStream << msg << " (In file " << __FILE__ << " line " << __LINE__ << ".)"; \
	ErrorHandling::fatalError(msgStream.str()); \
	} while (0)

	// TODO change GADGETLIB_ASSERT to not run in debug
#define ALGEBRALIB_ASSERT(predicate, msg) if(!(bool(predicate))) ALGEBRALIB_FATAL(msg);


} // namespace Algebra

#endif   // ALGEBRALIB_ERRORHANDALING_HPP_
