#ifndef ALGEBRA_LINEARSPACE_HPP__
#define ALGEBRA_LINEARSPACE_HPP__

#include "FiniteSetInterface.hpp"
#include "algebraLib/SubspacePolynomial.hpp"

namespace Algebra{

/**
 * @class LinearSpace
 * @brief represents a set which is a linear space
 */
class LinearSpace : public FiniteSetInterface {
public:
	LinearSpace(const std::vector<FieldElement>& orderedBasis, const FieldElement affineShift = zero());

	/**
	 * @brief   Checks if an elements that satisfies a predicate exist in the set
	 * @param   pred the predicate
	 * @return  True iff such an element exists
	 */
	bool exist(const std::unique_ptr<const FieldElementPredicate>& pred)const;
	size_t size()const;
	const UnivariatePolynomialInterface& vanishingPoly()const;

    /**
     * Returns whether a field element is a member of the set
     */
    bool contains(const FieldElement& e)const;

private:
    const std::vector<FieldElement> orderedBasis_;
    const FieldElement affineShift_;
    const SubspacePolynomial subspacePoly_;
};

} // namespace Algebra

#endif // ALGEBRA_LINEARSPACE_HPP__
