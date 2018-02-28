#include <vector>
#include "FieldElement.hpp"

#ifndef __AFFINE_SPACE_HPP__
#define __AFFINE_SPACE_HPP__

namespace Algebra{

class AffineSpace{
public:
    typedef std::vector<FieldElement> orderedBasis_t;
    AffineSpace(const orderedBasis_t& basis, const FieldElement& affineShift = zero());
    const orderedBasis_t& getBasis()const;
    const FieldElement& getAffineShift()const;
    unsigned long long size()const;
    FieldElement getElementByIndex(const unsigned long long idx)const;
private:
    const std::vector<FieldElement> basis_;
    const FieldElement shift_;
};

} // namespace Algebra

#endif //#ifndef __AFFINE_SPACE_HPP__
