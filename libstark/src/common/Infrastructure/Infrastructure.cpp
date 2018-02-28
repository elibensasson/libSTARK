#include "Infrastructure.hpp"

namespace Infrastructure {

/*****************************************************************************/
/****************************  Basic Math  ***********************************/
/*****************************************************************************/

///Returns log in base 2
double Log2( double n )  {
    return log(n) / log((double)2);  // log(n)/log(2) is ::Infrastructure::Log2.
}

/// Returns an upper bound on log2(i). Namely, returns the number of binary digits needed to store
/// the value 'i'. When i == 0 returns 0.
unsigned int Log2ceil(uint64_t i) {
    int retval = i ? 1 : 0 ;
    while (i >>= 1) {++retval;}
    return retval;
}

///Returns true iff x is a power of 2
bool IsPower2(const long x)  {
    return ( (x > 0) && ((x & (x - 1)) == 0) );
}

} // namespace Infrastructure 

