#ifndef  __Infrastructure_HPP
#define  __Infrastructure_HPP

#include <cstdint>
#include <cmath>

namespace Infrastructure {

/********************************************************/
/****************** Basic Math **************************/
/********************************************************/

//Calculates ::Infrastructure::Log2 of a number.
double Log2(double n);

//Calculates  upper bound of Log2 of a number (number of bits needed to represent value)
unsigned int Log2ceil(uint64_t i);

//Returns true iff the given number is a power of 2.
bool IsPower2(const long x);

// Returns 2^exponent
/*constexpr*/ inline unsigned long POW2(int exponent) {
    //assert(exponent>=0);
    return (1UL) << exponent;
}

//Returns the ceiling of a when a is of type double.
/*constexpr*/ inline int64_t CEIL(double a) {
    return (int64_t)ceil(a);
}
//#define CEIL(a)  ((int64_t)ceil((double)(a)))

} // namespace Infrastructure 

#endif   // __Infrastructure_HPP
