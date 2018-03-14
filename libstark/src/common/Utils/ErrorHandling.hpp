#ifndef _COMMON_UTILS_HPP_
#define _COMMON_UTILS_HPP_

#include <string>
#include <sstream>

#ifdef __GLIBC__
#include <execinfo.h> // backtraces
#endif

namespace libstark {

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
    static void __noreturn fatalError(std::string msg);
    static void __noreturn fatalError(const std::stringstream& msg);
	static void warning(std::string msg);
	static void warning(const std::stringstream& msg);
	static void info(std::string msg);
	static void info(const std::stringstream& msg);
	static void printStacktrace();

};
    
#define _COMMON_DEBUG_MSG(msg) do {  \
            std::cerr << msg << " (In file " << __FILE__ << " line " << __LINE__ << ".)"; \
        } while (0)


#define _COMMON_FATAL(msg) do {  \
            std::stringstream msgStream; \
            msgStream << msg << " (In file " << __FILE__ << " line " << __LINE__ << ".)"; \
            libstark::ErrorHandling::fatalError(msgStream.str()); \
        } while (0)

// TODO change _COMMON_ASSERT to not run in debug
#define _COMMON_ASSERT(predicate, msg) if(!(bool(predicate))) _COMMON_FATAL(msg);

} // of namespace

#endif	//_COMMON_UTILS_HPP_
