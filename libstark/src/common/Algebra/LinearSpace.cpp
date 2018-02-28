#include "LinearSpace.hpp"
#include "algebraLib/UnivariatePolynomialGeneral.hpp"
#include "algebraLib/SubspacePolynomial.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include <omp.h>

namespace Algebra{

LinearSpace::LinearSpace(const std::vector<FieldElement>& orderedBasis, const FieldElement affineShift) : 
    orderedBasis_(orderedBasis),
    affineShift_(affineShift),
    subspacePoly_(elementsSet_t(orderedBasis.begin(),orderedBasis.end()))
    {};

bool LinearSpace::exist(const std::unique_ptr<const FieldElementPredicate>& pred)const { 

    const size_t spaceSize = Infrastructure::POW2(orderedBasis_.size());
    bool isFound = false;
#ifndef _DEBUG
#pragma omp parallel
#endif
    {
        const size_t numThreads = omp_get_num_threads();
        const size_t currThreadId = omp_get_thread_num();
        for (size_t i = 0; ((i+currThreadId)< spaceSize) && (!isFound); i+=numThreads){
            const auto currLocation = i+currThreadId;
            FieldElement e = getSpaceElementByIndex(orderedBasis_,affineShift_, currLocation);
            if (pred->test(e) == true){
                isFound=true;
            }
            
#ifdef PRINT_PROGRESS
	    if (currLocation%100 == 0){
                std::cout<<currLocation<<"/"<<spaceSize<<",";
                std::fflush(stdout);
            }
#endif //PRINT_PROGRESS
        }
    }
    return isFound;
}

size_t LinearSpace::size()const {
    return Infrastructure::POW2(orderedBasis_.size());
}

const UnivariatePolynomialInterface& LinearSpace::vanishingPoly()const{
    return subspacePoly_;
}


bool LinearSpace::contains(const FieldElement& e)const{
    // "e" is in the linear space
    // if and only if it is a root of the
    // minimal polynomial that vanishes over the linear space
    return zero() == subspacePoly_.eval(e + affineShift_);
}

} // namespace Algebra
