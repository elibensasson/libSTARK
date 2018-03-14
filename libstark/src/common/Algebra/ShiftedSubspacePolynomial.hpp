/**
* A class for special affine polynomials - that we here call `shifted subspace polynomials'.
These have the property that the linear part is a subspace polynomial - (see explanation in the class
SubspacePolynomial)
The only difference between this class and SubspacePolynomial is that the constantFactor_ member
is not required to be zero.
(and therefore, the constructor of this class receives the value of constantFactor_  as a parameter)
*/

#ifndef _COMMON_ALGEBRA_SHIFTEDSUBSPACEPOLYNOMIAL_HPP__
#define _COMMON_ALGEBRA_SHIFTEDSUBSPACEPOLYNOMIAL_HPP__

#include "algebraLib/AffinePolynomial.hpp"
namespace Algebra
{

	class ShiftedSubspacePolynomial : public AffinePolynomial{
	public:
		ShiftedSubspacePolynomial(const elementsSet_t& spanSet, const FieldElement& constantFactor);

	private:
		static std::vector<FieldElement> computeVanishingPoly(const elementsSet_t& spanSet);
	};
}// namespace Algebra

#endif // !_COMMON_ALGEBRA_SHIFTEDSUBSPACEPOLYNOMIAL_HPP__

