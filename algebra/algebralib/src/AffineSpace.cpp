#include "algebraLib/AffineSpace.hpp"

namespace Algebra{

using std::vector;

AffineSpace::AffineSpace(const orderedBasis_t& basis, const FieldElement& affineShift):
   basis_(basis), shift_(affineShift){}; 

const AffineSpace::orderedBasis_t& AffineSpace::getBasis()const{
    return basis_;
}

const FieldElement& AffineSpace::getAffineShift()const{
    return shift_;
}

unsigned long long AffineSpace::size()const{
    return 1ULL<<(getBasis().size());
}

FieldElement AffineSpace::getElementByIndex(const unsigned long long idx)const{
    return getSpaceElementByIndex(basis_,shift_,idx);
}
} // namespace Algebra
