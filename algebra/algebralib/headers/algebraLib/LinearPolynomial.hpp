#include "PolynomialInterface.hpp"

#ifndef LINEAR_POLYNOMIAL_HPP__
#define LINEAR_POLYNOMIAL_HPP__

namespace Algebra{

class LinearPolynomial : public UnivariatePolynomialInterface {

public:
    
    LinearPolynomial(const FieldElement& coeff0, const FieldElement& coeff1) : coefficient0_(coeff0),coefficient1_(coeff1){};
    
    /**
     * @brief return a clone of the current polynomial
     * @return a unique_ptr of PolynomialInterface,
     * representing a polynomial equivalent to current
     */
    std::unique_ptr<PolynomialInterface> clone()const;
	
    /**
	 * @brief   Evaluates the polynomial instance using specific element
	 * @param   x the assignment
	 * @return  P(x)
	 */
	FieldElement eval(const FieldElement& x)const{
        return coefficient0_ + x*coefficient1_;
    }
	
	/**
	 * @brief   returns the value of the coefficient of \f$x^\text{index}\f$
	 * @param   index the power of \f$x\f$ the coefficient multiplies 
	 * @return   cefficient the value of the coefficient
	 */
	FieldElement getCoefficient(const unsigned int index)const;
	 
    /**
     * @brief   returns the degree of the polynomial
     * @return   polynomial degree
     */
    PolynomialDegree getDegree()const;

	/**
	 * @brief   computes the composition polynomial
	 * @param   p input linear polynomial
	 * @return  The composition \f$this \circ p\f$ such that
	 * \f$(this \circ p)(x) = this(p(x))\f$
	 */
    LinearPolynomial compose(const LinearPolynomial& p)const;

private:
    const FieldElement coefficient0_;
    const FieldElement coefficient1_;

};

bool operator==(const LinearPolynomial& p1, const LinearPolynomial& p2);
LinearPolynomial operator+(const LinearPolynomial& p, const FieldElement& c);
    
} //namespace Algebra

#endif // LINEAR_POLYNOMIAL_HPP__
