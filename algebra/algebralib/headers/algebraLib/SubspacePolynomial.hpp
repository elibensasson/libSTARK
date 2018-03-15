/**
* A class for special linearized polynomials - subspace polynomials - that split into distinct roots
The main addition in this class is a constructor that takes as input a set of elements,
and returns the subspace poly whose roots are exactly the linear span of this set
*/

#ifndef SUBSPACE_POLYNOMIAL_HPP__
#define SUBSPACE_POLYNOMIAL_HPP__

#include "LinearizedPolynomial.hpp"
namespace Algebra
{

	class SubspacePolynomial : public LinearizedPolynomial{
	public:
		SubspacePolynomial(const elementsSet_t& spanSet);

	private:
		static std::vector<FieldElement> computeVanishingPoly(const elementsSet_t& spanSet);
	};
}// namespace Algebra

#endif // !SUBSPACE_POLYNOMIAL_HPP__

