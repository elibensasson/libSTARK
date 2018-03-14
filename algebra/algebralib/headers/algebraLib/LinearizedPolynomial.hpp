/********************************************** LinearizedPolynomials.hpp ************************************************/
/**
* @file.
*
* A class of (strictly) linearized polynomials - i.e.,
univariate polynomials whose non-zero coefficients are only of monomials of the form  x^{2^i},
and whose constant coefficient is zero. 
*
*/
/************************************************************************************************************/
#ifndef LINEARIZED_POLYNOMIAL_HPP_
#define LINEARIZED_POLYNOMIAL_HPP_


#include "AffinePolynomial.hpp"
namespace Algebra {
class LinearizedPolynomial : public  AffinePolynomial{
	
	public:
		/** Class Constructor - Assigns the given coefficient array to the newly created polynomial.
		Assigns the constant factor of the poly to be 0.
		*/
		LinearizedPolynomial(std::vector<FieldElement> coefficients) :AffinePolynomial(coefficients, zero()){};
	};


}// of namespace Algebra

#endif
