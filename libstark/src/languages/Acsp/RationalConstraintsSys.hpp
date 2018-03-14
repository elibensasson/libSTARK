#include <algebraLib/MappingsSys.hpp>

#include <memory>
#include <set>
#include <vector>

#ifndef RAT_CONSTRAINTS_SYS_HPP__
#define RAT_CONSTRAINTS_SYS_HPP__

namespace libstark{

/** 
 * @class RationalConstraintSys
 * @brief An interface for representation of system of rational constraints.
 *
 * A rational constraint system \f$ \mathcal{C} \f$ is a pair \f$ \f$
 *
 * set of ordered pairs multivariate polynomials over
 * some variables set \f$ \mathcal{U} \f$ and a space \f$H\f$.
 * each pair contains one polynomial called the enumerator and marked by \f$p\f$,
 * and another called the denominator and marked by \f$q\f$.
 * 
 * We say that an assignment \f$ \alpha:\mathcal{U} \to \mathbb{F} \f$ satisfies
 * \f$ \mathcal{C} \f$ if and only if \f$ \forall (p,q) \in \mathcal{C} : q\left(\alpha\right)=0 \rightarrow p\left(\alpha\right)=0\f$.
 *
 * The evaluation of \f$ (p,q) \in \mathcal{C}\f$ is the evaluation of the rational function \f$ \frac{p}{q} \f$,
 * in case \f$ q\left(\alpha\right) \f$ the output is undefined and an exception is thrown.
 */

class RationalConstraintSys : public Algebra::MappingsSys{
public:
	virtual ~RationalConstraintSys(){};
	
    /// the amount of inputs each polynomial expects
    virtual size_t numVars() const = 0;

    size_t numMappings()const = 0;
    virtual std::vector<Algebra::FieldElement> eval(const std::vector<Algebra::FieldElement>& assignment)const = 0;
    RationalConstraintSys* clone()const = 0;

    /// verifies if the rational constraint system is satisfied by given assignment
	bool verify(const std::vector<Algebra::FieldElement>& assignment)const = 0;

}; // class ConstraintSys

} // namespace libstark

#endif // RAT_CONSTRAINTS_SYS_HPP__
