/**
 *       @file  AcspWitness.hpp
 *      @brief  Header file for Acsp witness
 *
 *     @author  Michael Riabzev, RiabzevMichael@gmail.com
 * =====================================================================================
 */
#pragma once // This is not needed, here just to get rid of an annoying VS2010 warning.
#ifndef  __Acsp_WITNESS_HPP
#define  __Acsp_WITNESS_HPP

#include <algebraLib/PolynomialInterface.hpp>

#include <memory>

namespace libstark {

/**
 * @class AcspWitness
 * @brief class for Acsp witness
 *
 * This class describes a witness for AcspFullInstance.
 * A witness is a polynomial \f$ A \in \mathbb{F}[x] \f$,
 * and named Assignment Polynomial.
 * Such a polynomial shows that\n
 * \f$(\mathbb{F},H,\vec{N},P,witnessDegreeBound,B)\f$ is a satisfiable AcspInstance
 * if and only if
 * \f{eqnarray*}{
 * \forall z \in H: (P \circ (x \vert A \circ \vec{N}))(z) = 0 \\
 * \deg A <= witnessDegreeBound \\
 * \lambda \cdot \deg(P \circ (x \vert A \circ \vec{N})) \le |\mathbb{F}| \\
 * \forall (x,y) \in B : A(x)=y \\
 * \f}
 *
 * In the code we name the assignment polynomial \f$A\f$
 * simply 'assignmentPoly'
 *
 * Methods:
 * Witness class contains only getters, 
 * an empty constructor and a destructor.
 * The only possible way to change its data members
 * is using the reduction class from EGCP or inside a UTEST.
 */
class AcspWitness {
public:
	typedef Algebra::UnivariatePolynomialInterface polynomial;
    typedef std::unique_ptr<const polynomial> poly_ptr;
    typedef std::vector<poly_ptr> poly_vec;

	AcspWitness(poly_ptr&& assignmentPoly):assignmentPolys_(1){
        assignmentPolys_[0] = std::move(assignmentPoly);
    }

	AcspWitness(poly_vec&& assignmentPolys):assignmentPolys_(std::move(assignmentPolys)){};
	
	AcspWitness(AcspWitness&& src) = default;
	AcspWitness(const AcspWitness& src) = delete;
	
    inline const polynomial& assignmentPoly() const {
		return *(assignmentPolys_[0]);
	}
	
    inline const poly_vec& assignmentPolys() const {
		return assignmentPolys_;
	}

private:

	poly_vec assignmentPolys_;
};

} //namespace libstark

#endif //__Acsp_WITNESS_HPP
