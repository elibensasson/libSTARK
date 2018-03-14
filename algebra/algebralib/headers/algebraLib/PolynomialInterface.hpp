#ifndef __POLYNOMIAL_INTERFACE_HPP
#define __POLYNOMIAL_INTERFACE_HPP

#include "FieldElement.hpp"
#include "PolynomialDegree.hpp"
#include "FFT.hpp"
#include "MappingsSys.hpp"
#include "AffineSpace.hpp"
#include <algebraLib/variable.hpp>
#include <list>
#include <cassert>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

namespace Algebra {

class PolynomialInterface : public multivarFunc{
public:
	//number of variables polynomial expects
    virtual size_t numVars() const = 0;   

    //evaluates a polynomial on a given assignment
    virtual FieldElement eval(const std::vector<FieldElement>& x)const = 0;
    
	//evaluates a polynomial on a given set (represented as vector) of assignments
    virtual std::vector<FieldElement> evalOnSet(const std::vector<std::vector<FieldElement>>& x_set)const{
        const size_t numEvals = x_set.size();
        std::vector<FieldElement> res(numEvals);
        for(size_t i=0; i< numEvals; i++){
            res[i] = eval(x_set[i]);
        }

        return res;
    }
    
    virtual bool isRoot(const std::vector<FieldElement>& x)const {return eval(x) == zero();}
	virtual ~PolynomialInterface(){};
	
	/**
	 * @brief   Returns an upper bound for degree of this polynomial
	 * when composed with univariate polynomials (as its variables)
	 * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
	 * @return  a degree \f$D\f$ such that
	 * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
	 * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
	 * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
	 */
	virtual PolynomialDegree getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees)const = 0;

    /**
     * @brief return a clone of the current polynomial
     * @return a unique_ptr of PolynomialInterface,
     * representing a polynomial equivalent to current
     */
    virtual std::unique_ptr<PolynomialInterface> clone()const = 0;

    /**
     * @brief returns false only if for any assignment \f$ a \f$
     * and for any change of the value of \f$ a_{varId} \f$,
     * the result of the evaluation does not change
     */
    virtual bool isEffectiveInput(const size_t varId)const{return true;}
};

class UnivariatePolynomialInterface : public PolynomialInterface {
public:
    size_t numVars() const{
        return 1;
    }
	
    FieldElement eval(const std::vector<FieldElement>& x)const {
		assert(x.size() == 1);
		return eval(x[0]);
	}
	
	PolynomialDegree getDegreeBound(const std::vector<PolynomialDegree>& inputDegreesBound)const {
		//check if the current polynomial is constant the degree does not change
        if(getDegree().isConstantPolyDeg()){
            return getDegree();
        }
        assert(inputDegreesBound.size() == 1);
        //if the input degree is of a constant polynomial the degree is bounded to 0
        //it is exactly 0 if the p(0) != 0 for p as current polynomial,
        //and otherwise it is exactly the degree of the input.
        if(inputDegreesBound[0].isConstantPolyDeg()){
            if (getCoefficient(0) != zero()){
                return PolynomialDegree(0);
            }
            else{
                return inputDegreesBound[0];
            }
        }
		
        //otherwise, both the degrees are non-negative, and the composition degree
        //is the product of those degrees
        return PolynomialDegree(PolynomialDegree::integral_t(getDegree())*PolynomialDegree::integral_t(inputDegreesBound[0]));
	}

    /**
     * Return the degree bound for a single input
     */
    virtual PolynomialDegree getDegreeBound(const PolynomialDegree inputDeg)const{
        return getDegreeBound(std::vector<PolynomialDegree>({inputDeg}));
    }
	
	/**
	 * @brief   Evaluates the polynomial instance using specific element
	 * @param   x the assignment
	 * @return  P(x)
	 */
	virtual FieldElement eval(const FieldElement& x)const = 0;

    /**
     * @brief evaluates a polynomial over a given space (represented by a ordered basis and affine shift)
     * @return the evaluation as a vector, element number \f$ \sum_{i=0}^n b_i \cdot 2^i \f$ represents
     *          the value at the point \f$ shift + \sum_{i=0}^n (basis element)_i \cdot 2^i \f$
     */
    virtual std::vector<FieldElement> eval(const std::vector<FieldElement>& orderedBasis, const FieldElement& shift)const{

        return FFT(getCoefficients(),orderedBasis,shift);
    }

    virtual std::vector<FieldElement> eval(const AffineSpace& evaluationSpace)const{
        return eval(evaluationSpace.getBasis(), evaluationSpace.getAffineShift());
    }
	
	/**
	 * @brief   returns the value of the coefficient of \f$x^\text{index}\f$
	 * @param   index the power of \f$x\f$ the coefficient multiplies 
	 * @return   cefficient the value of the coefficient
	 */
	virtual FieldElement getCoefficient(const unsigned int index)const = 0;
	
    //returns all coefficients in a vector
    virtual const std::vector<FieldElement> getCoefficients()const{
        using std::vector;

        //get coefficients
        const size_t numCoeffs = std::max(PolynomialDegree::integral_t(0),1+PolynomialDegree::integral_t(getDegree()));
        vector<FieldElement> coeffs(numCoeffs);
        for (size_t i=0; i< numCoeffs; i++){
            coeffs[i] = getCoefficient(i);
        }

        return coeffs;
    }

	 /**
	 * @brief   returns the degree of the polynomial
	 * @return   polynomial degree
	 */
    virtual PolynomialDegree getDegree()const = 0;
};

class DivisorPolynomial : public UnivariatePolynomialInterface {
	public:

	/**
	 * @brief Let this polynomial be \f$a \in \mathbb{F}[x]\f$, and the parameter be \f$b \in \mathbb{F}[x]\f$
	 * than by the devision theorem there exists \f$ q,r \in \mathbb{F}[x] \f$ such that \f$ \deg r < \deg a \f$
	 * and \f$ b = aq + r \f$.
	 * This method return \f$q\f$
	 */
	virtual std::unique_ptr<UnivariatePolynomialInterface> divideByMe(const UnivariatePolynomialInterface& dividend)const = 0;
};


////an interface with some extra properties used in the cs2BREX reduction. Currently seems needed because of some circular class relations:
//// Polynomial Node holds SelectorSum field, and SelectorSum used to hold CircuitPolynomial vector, before this interface was written
//class TinyRAMPolynomialInterface : public PolynomialInterface{
//
//	virtual std::string asString() const;
//	virtual Variable::set getUsedVariables() const;
//	virtual void setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New);
//
//
//};

} //namespace Algebra

#endif // __POLYNOMIAL_INTERFACE_HPP
