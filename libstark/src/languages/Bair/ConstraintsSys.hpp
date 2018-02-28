#include <algebraLib/PolynomialInterface.hpp>
#include <algebraLib/MappingsSys.hpp>

#include <memory>
#include <set>
#include <vector>

#ifndef CONSTRAINTS_SYS_HPP__
#define CONSTRAINTS_SYS_HPP__

namespace libstark{

/** 
 * @class ConstraintSys
 * @brief An interface for representation of system of constraints.
 *
 * A constraint system \f$ \mathcal{C} \f$ is a set of multivariate polynomials over
 * some variables set \f$ \mathcal{U} \f$ .
 * 
 * We say that an assignment \f$ \alpha:\mathcal{U} \to \mathbb{F} \f$ satisfies
 * \f$ \mathcal{C} \f$ if and only if \f$ \forall p \in \mathcal{C} : p\left(\alpha\right)=0 \f$.
 * 
 * For a given constraints system \f$ \mathcal{C} \f$ and a variables set \f$ \mathcal{V} \f$
 * we define \f$ \mathcal{V}\left(\mathcal{C}\right)\subset\mathcal{V} \f$ as the
 * set of all variables \f$ v \f$ that are used in \f$ \mathcal{C} \f$ (it will be useful
 * later for performance, when we notice some constraint systems use
 * only a fraction of the variables set).
 */

class ConstraintSys : public Algebra::MappingsSys{
public:
	typedef std::unique_ptr<Algebra::PolynomialInterface> polyPtr_t;
	typedef std::vector<polyPtr_t> polySet_t;
	
	virtual ~ConstraintSys(){};
	
    /// the amount of inputs each polynomial expects
    virtual size_t numVars() const = 0;

    size_t numMappings()const;
    virtual std::vector<Algebra::FieldElement> eval(const std::vector<Algebra::FieldElement>& assignment)const;
    ConstraintSys* clone()const = 0;

    Algebra::PolynomialInterface* getLinearComb(const std::vector<Algebra::FieldElement>& coeffs)const;

    /// the constraint polynomials
	virtual const polySet_t& constraints() const = 0;

    /// return false only if the variable with given ID does not affect any constraint polynomial
	virtual bool varUsed(const size_t varId) const;

    /// verifies if the constraint system is satisfied by given assignment
	bool verify(const std::vector<Algebra::FieldElement>& assignment)const;

    /// get maximal degree of a polynomial in the system
    Algebra::PolynomialDegree getMaximalDegree()const;

}; // class ConstraintSys

} // namespace libstark

#endif // CONSTRAINTS_SYS_HPP__
