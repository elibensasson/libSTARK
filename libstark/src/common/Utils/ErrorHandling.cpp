/********************************************** ErrorHandling.cpp ************************************************/
/**
 * @file.
 *
 * The file ErrorHandling.cpp contains various utility functions and date type definitions used by the different 
 * classes in the project.
 * 
 * For more information - Read the documentation of the header file ErrorHandling.hpp.
 */
  /************************************************************************************************************/
#include <iostream>
#ifndef WIN32
#include <unistd.h>
#endif
#include "ErrorHandling.hpp"

using namespace std;

namespace libstark {




/*****************************************************************************/
/***********************  ErrorHandling********** ****************************/
/*****************************************************************************/

void ErrorHandling::fatalError(string msg) {
	cerr << "ERROR:  " << msg << endl << endl;
	printStacktrace();
	throw std::runtime_error(msg);
}

void ErrorHandling::fatalError(const stringstream& msg) {
	fatalError(msg.str());
}

void ErrorHandling::warning(string msg) {
	cerr << "WARNING:  " << msg << endl << endl;
	printStacktrace();
}

void ErrorHandling::warning(const stringstream& msg) {
	warning(msg.str());
}

void ErrorHandling::info(string msg) {
	cerr << msg << endl << endl;
}

void ErrorHandling::info(const stringstream& msg) {
	info(msg.str());
}

void ErrorHandling::printStacktrace() {
#ifdef __GNUC__
	cerr << "Stack trace (pipe through c++filt to demangle identifiers):" << endl;
	const int maxFrames = 100;
	void* frames[maxFrames];
	// Fill array with pointers to stack frames
	int numFrames = backtrace(frames, maxFrames);
	// Decode frames and print them to stderr
	backtrace_symbols_fd(frames, numFrames, STDERR_FILENO);
#else
    //TODO make this available for Windows
	cerr << "  (stack trace not available on this platform)" << endl;
#endif // __GNUC__
}


} // of namespace
