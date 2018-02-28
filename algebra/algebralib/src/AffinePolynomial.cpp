/********************************************** AffinePolynomial.cpp ************************************************/
/**
* @file.
*
* A class of (affine) A polynomials - i.e.,
univariate polynomials whose non-zero coefficients_ are only of monomials of the form  x^{2^i},
and possibly, a non-zero constant coefficient.
*
*
*/
/************************************************************************************************************/

#include "algebraLib/AffinePolynomial.hpp"

#include <stdio.h>
#include <assert.h>
#include <math.h>

using namespace std;
namespace Algebra {
    
	/******************************************************************************************************/
	/***************************************** Helper functions *******************************************/
	/******************************************************************************************************/


	/* The function evaluates the linear part of the Affine polynomial at a given point and returns the result.
	It is used in the computeMat method to evaluate the linear part of the 
	poly on a basis in order to construct the matrix corresponding
	to the poly's operation.
	After the affine poly is intialized, this matrix will be used to evaluate it.
	Tests showed using the matrix, rather than regular univariate evaluation using coeffs,  can make the evaluation
	16 times faster!
	*/
	FieldElement evalLinearPart(const FieldElement& x, const vector<FieldElement>& coefficients)  {

		FieldElement res = zero();

		/*we take advantage of the fact squaring is very efficient in GF2E,
		and that x^{2^i} = (x^{2^{i-1})^2. Each iteration we square the current power
		of x- power_x, multiply it by the relevant coefficient, and add to the sum,
		resulting in sum_{i=0}^{size-1} coeffcieints[i]*x^{2^i} */

		FieldElement currPower, power_x = x;
		for (unsigned long i = 0; i<coefficients.size(); i++) {
			currPower = coefficients[i] * power_x;

			res += currPower;
			power_x *= power_x;
		}

		return res;
	}


    

	/******************************************************************************************************/
	/************************************ AffinePolynomial Class ******************************************/
	/******************************************************************************************************/
	
	/** Class Constructor - Assigns the given coefficient array to the newly created polynomial. */
	AffinePolynomial::AffinePolynomial(const vector<FieldElement>& coefficients, const FieldElement& constantFactor) {
		if (coefficients.size())
		{

			//
			//Initialize coefficients 
			//
			const FieldElement ZERO = zero();

			//find highest non zero index:
			int lastIndex = coefficients.size() - 1;
			while ((lastIndex >= 0) && (coefficients[lastIndex] == ZERO)){
				lastIndex--;
			}
			coefficients_ = vector<FieldElement>(coefficients.begin(), coefficients.begin() + lastIndex + 1);
		}
	
        constantFactor_ = constantFactor;
        constantFactor_intRep_ = mapFieldElementToInteger(0,ExtensionDegree,constantFactor);
		computeMat();
	}
    
    unique_ptr<PolynomialInterface> AffinePolynomial::clone()const{
        return unique_ptr<PolynomialInterface>(new AffinePolynomial(coefficients_, constantFactor_));
    }
		
    /** The function evaluates the Affine polynomial at a given point and returns the result. */
    FieldElement AffinePolynomial::eval(const Algebra::FieldElement& x)const{
         size_t res = constantFactor_intRep_;

         size_t x_asInt = mapFieldElementToInteger(0,ExtensionDegree,x);
         for(int i=0; x_asInt != 0; i++){
            if(x_asInt & 1){
                res ^= polyMat_[i];
            }
            x_asInt >>=1;
         }

         return mapIntegerToFieldElement(0,ExtensionDegree,res);
    }

	//return the i'th coefficient of this polynomial
	FieldElement AffinePolynomial::getCoefficient(const unsigned int i)const {
        if ( i == 0) return constantFactor_;
		
        //if "i" is not a power of 2, return zero
        if ((1UL<<int(log2(i))) != i) return zero();

        const size_t indexLog = log2(i);
        if (indexLog < coefficients_.size()) return coefficients_[indexLog];

		return zero();
    }
    

    PolynomialDegree AffinePolynomial::getDegree() const{
        const FieldElement ZERO = zero();
        
        if (coefficients_.size() > 0){
            return PolynomialDegree(1LL<<(coefficients_.size()-1));
        }
        
        if (constantFactor_ != ZERO){
            return PolynomialDegree(0);
        }

        //otherwise this is the zero polynomial, and its degree is (-infinity)
        return PolynomialDegree(-1);
    }
    
    void AffinePolynomial::multiplyByConstant(const FieldElement& factor){
        for(FieldElement& c : coefficients_){
            c *= factor;
        }
		computeMat();
    }


	void AffinePolynomial::computeMat() {
		const long dim = ExtensionDegree;

		if (coefficients_.size() == 0){
            polyMat_.fill(0);
            return;
		}
		

		for (long i = 0; i < dim; i++){
            const FieldElement x = mapIntegerToFieldElement(i, 1, 1);
			polyMat_[i] = mapFieldElementToInteger(0,ExtensionDegree,evalLinearPart(x,coefficients_));

		}
    }
	

	

} // of namespace Algebra
