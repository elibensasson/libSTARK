#include "lightCircuit.hpp"
#include <algebraLib/PolynomialInterface.hpp>
#include <algebraLib/UnivariatePolynomialGeneral.hpp>
//#include <gadgetlib2/variable.hpp>

#ifndef  __lightCircPoly_HPP
#define  __lightCircPoly_HPP

namespace libstark {

class lightCircPoly : public Algebra::PolynomialInterface {
public:
	typedef Algebra::FieldElement elementT;
	typedef Algebra::UnivariatePolynomialGeneral univariateT;

	/**
	 * @brief   A constructor to convert from UnivariatePolynomialGeneral
	 * Generates an equivalent polynomial (same mapping)
	 */
	explicit lightCircPoly(const univariateT&);

	/**
	 * @brief Construction from a circuit
	 * @param circ must have a single output
	 */
	explicit lightCircPoly(const LightCircLib::lightCircuit& circ);
	
    lightCircPoly(const lightCircPoly& poly);

    size_t numVars()const;
    
    /**
     * @brief return a clone of the current polynomial
     * @return a unique_ptr of PolynomialInterface,
     * representing a polynomial equivalent to current
     */
    std::unique_ptr<Algebra::PolynomialInterface> clone()const;
	
    /**
	 * @brief   A constructor to that expand the amount of inputs of a givven
     * lightCircPoly
     *
     * It requires a mapping from the original inputs indexes to new inputs indexes
	 */
	lightCircPoly(const lightCircPoly&, const size_t totalInputs ,const std::vector<size_t>& originalInputsLocation);

	/**
	 * @brief   Evaluation the polynomial
	 * @param   x vector of inputs \f$\vec{x} = (x_1,x_2,\dots,x_n)\f$
	 * @return  The evaluation \f$P(x_1,x_2,\dots,x_n)\f$
	 */
	elementT eval(const std::vector<elementT>& x)const;
	
	/**
	 * @brief   Evaluates the polynomial, as a polynomial over \f$ \mathbb{F}[x] \f$
	 * @param   x vector of input univariate polynomials \f$\vec{x} = (p_1,p_2,\dots,p_n)\f$
	 * @return  The evaluation (composition) \f$P(p_1,p_2,\dots,p_n)\f$ such that
	 * \f$(P(p_1,p_2,\dots,p_n))(x) = P(p_1(x),p_2(x),\dots,p_n(x))\f$
	 */
	univariateT eval(const std::vector<univariateT>& x)const;
	
	/**
	 * @brief   Evaluates the polynomial, as a polynomial over \f$ \mathbb{F}[x] \f$
	 * @param   x vector of pointer to input univariate polynomials \f$\vec{x} = (p_1,p_2,\dots,p_n)\f$
	 * @return  The evaluation (composition) \f$P(p_1,p_2,\dots,p_n)\f$ such that
	 * \f$(P(p_1,p_2,\dots,p_n))(x) = P(p_1(x),p_2(x),\dots,p_n(x))\f$
	 */
	Algebra::UnivariatePolynomialInterface* eval(const std::vector<Algebra::UnivariatePolynomialInterface const* >& x)const;


	/**
	 * @brief   Multiplies this polynomial with other polynomial
	 * The variables of the other polynomial are handled as
	 * distinct to the variables of this polynomial,
	 * and their ordered after the variables of this polynomial.
	 * Mathematically, if current polynomial is
	 * \f$p(x_1,x_2,\dots,x_n)\f$ and the other polynomial
	 * is \f$q(x_1,x_2,\dots,x_m)\f$, the result is
	 * that this polynomial becomes
	 * \f$f(x_1,x_2,\dots,x_n,y_1,y_2,\dots,y_m) = p(x_1,x_2,\dots,x_n) \cdot q(y_1,y_2,\dots,y_m)\f$
	 * @param   other the polynomial to multiply with
	 */
	void multiplyDistinct(const lightCircPoly& other);

	/**
	 * @brief   Adds this polynomial to other polynomial
	 * The variables of the other polynomial are handled as
	 * distinct to the variables of this polynomial,
	 * and their ordered after the variables of this polynomial.
	 * Mathematically, if current polynomial is
	 * \f$p(x_1,x_2,\dots,x_n)\f$ and the other polynomial
	 * is \f$q(x_1,x_2,\dots,x_m)\f$, the result is
	 * that this polynomial becomes
	 * \f$f(x_1,x_2,\dots,x_n,y_1,y_2,\dots,y_m) = p(x_1,x_2,\dots,x_n) + q(y_1,y_2,\dots,y_m)\f$
	 * @param   other the polynomial to multiply with
	 */
	void addDistinct(const lightCircPoly& other);

	/**
	 * @brief   Adds this polynomial to other polynomial
	 * Mathematically, if current polynomial is
	 * \f$p(x_1,x_2,\dots,x_n)\f$ and the other polynomial
	 * is \f$q(x_1,x_2,\dots,x_m)\f$, the result is
	 * that this polynomial becomes
	 * \f$f(x_1,x_2,\dots,x_{max(n,m)}) = p(x_1,x_2,\dots,x_n) + q(x_1,x_2,\dots,x_m)\f$
	 * @param   other the polynomial to multiply with
	 */
	void addSameInputs(const lightCircPoly& other);

	/**
	 * @brief   Returns an upper bound for degree of this polynomial
	 * when composed with univariate polynomials (as its variables)
	 * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
	 * @return  a degree \f$D\f$ such that
	 * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
	 * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
	 * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
	 */
	Algebra::PolynomialDegree getDegreeBound(const std::vector<Algebra::PolynomialDegree>& inputDegreesBound)const;
	
    /**
     * @brief Returs the polynomial degree
     */
    Algebra::PolynomialDegree getDegree()const;

	/**
	 * returns the model of this instance
	 */
	virtual const LightCircLib::lightCircuit& getCircuit()const{return *model_;}

    /**
     * return whether a specified input is effective:
     * it returns false only if the result
     * can not be affected by the specified input
     */
    virtual bool isEffectiveInput(const size_t inputIndex)const;

protected:
	std::unique_ptr<LightCircLib::lightCircuit> model_;

	/**
	 * @brief   merging current polynomial with other polynomial using some
	 * arithmetic operation defined by the 'merging gate'.
	 * For example if current polynomial is \f$ p(x) \f$ and
	 * other polynomial is \f$ q(x) \f$, and the merging gate is addition gate,
	 * the result is \f$ r(x,y) = p(x) + q(y) \f$.
	 * If for the same case the merging gate is multiplying gate (aka TIMES)
	 * the result is \f$ p(x) \cdot \q(y) \f$.
	 * @param  other the other polynomial
	 * @param  mergingGate a pointer to the merging gate. After it is passed to
	 * this function, its management responsibility is passed to this lightCircPoly
	 * (it is added to the circuits gates set, and will be released only by it)
	 */
	void mergeDistinctInputs(const lightCircPoly& other, LightCircLib::lightCircGate* mergingGate);
	
    /**
	 * @brief   merging current polynomial with other polynomial using some
	 * arithmetic operation defined by the 'merging gate'.
	 * For example if current polynomial is \f$ p(x) \f$ and
	 * other polynomial is \f$ q(x) \f$, and the merging gate is addition gate,
	 * the result is \f$ r(x) = p(x) + q(x) \f$.
	 * If for the same case the merging gate is multiplying gate (aka TIMES)
	 * the result is \f$ p(x) \cdot \q(x) \f$.
     * It is allowed to have the amount of inputs not equal. If the other polynomial has less inputs, it
     * uses as many inputs as it needs from the vector beginning. 
     * If the other has more inputs then this then additional inputs are added to this.
     * are added ().
	 * @param  other the other polynomial
	 * @param  mergingGate a pointer to the merging gate. After it is passed to
	 * this function, its management responsibility is passed to this lightCircPoly
	 * (it is added to the circuits gates set, and will be released only by it)
	 */
	void mergeSameInputs(const lightCircPoly& other, LightCircLib::lightCircGate* mergingGate);
};


} // namespace libstark 

#endif   // __lightCircPoly_HPP
