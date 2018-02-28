/********************************************** AffinePolynomial.hpp ************************************************/
/**
* @file.
*

* A class of (affine) linearized polynomials - i.e.,
univariate polynomials whose non-zero coefficients are only of monomials of the form  x^{2^i},
and possibly, a non-zero constant coefficient.

written by Ariel Gabizon ariel.gabizon@gmail.com and Michal Riabzev

*
*
*/
/************************************************************************************************************/
#ifndef PCPCD_REFACTORING_ALGEBRA_AFFINEPOLYNOMIAL_HPP_
#define PCPCD_REFACTORING_ALGEBRA_AFFINEPOLYNOMIAL_HPP_

#include "algebraLib/FieldElement.hpp"
#include "algebraLib/PolynomialInterface.hpp"
#include <vector>
#include <array>

namespace Algebra {

	
	/**
	* As described above, a class for general Affine polynomials.
	*/
	class AffinePolynomial : public  UnivariatePolynomialInterface{
	
    public:

        /** Constructor **/
        AffinePolynomial(const std::vector<FieldElement>& coefficients, const FieldElement& constantFactor);

        /**
         * @brief return a clone of the current polynomial
         * @return a unique_ptr of PolynomialInterface,
         * representing a polynomial equivalent to current
         */
        std::unique_ptr<PolynomialInterface> clone()const;

		/** The function evaluates the Affine polynomial at a given point and returns the result. */
		FieldElement eval(const FieldElement& x)const;

		//return the i'th coefficient of this polynomial
		FieldElement getCoefficient(const unsigned int i)const; 

		PolynomialDegree getDegree() const;

        void multiplyByConstant(const FieldElement& factor);
		
        /** Class Destructor */
		virtual ~AffinePolynomial(){};

    private:
		std::vector<FieldElement> coefficients_;	//coefficients[i] = coefficient c of the monomial c*x^(2^i)
		FieldElement constantFactor_;  //The constant factor of the vanishing polynomial, relevant for affine subspaces and not linear.
        size_t constantFactor_intRep_;
		
        std::array<size_t,ExtensionDegree> polyMat_; //the matrix corresponding to evaluating this poly (not including adding constantFactor)
        /**updates the polyMat field to contain correct evaluation matrix*/
		void computeMat() ;

		
	};


}// of namespace Algebra

#endif //PCPCD_REFACTORING_ALGEBRA_AFFINEPOLYNOMIAL_HPP_
