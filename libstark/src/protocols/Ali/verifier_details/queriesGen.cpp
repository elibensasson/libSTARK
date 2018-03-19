#include "queriesGen.hpp"
#include <algebraLib/SubspacePolynomial.hpp>
#include <algebraLib/UnivariatePolynomialGeneral.hpp>

namespace libstark{
namespace Protocols{
namespace Ali{
namespace Verifier{
namespace details{

using std::pair;
using std::max;
using std::map;
using std::vector;
using Algebra::FieldElement;
using Algebra::zero;
using Infrastructure::POW2;

void linearCombinationValue::initLocation(const FieldElement& x, const unsigned int combId){
    x_ = x;
    combId_ = combId;
}

void linearCombinationValue::addAnswerPtr(FieldElement* answerPtr){
    result_.addAnswerPtr(answerPtr);
}

void linearCombinationValue::calculate_witness(const Protocols::Ali::details::randomCoeffsSet_t& coeffs)const{
    FieldElement res = ZK_mask_res;

    for(size_t i=0; i< coeffs.boundary.size(); i++){
        res += (coeffs.boundary[i].coeffUnshifted[combId_] + (power(x_, coeffs.boundary[i].degShift) * coeffs.boundary[i].coeffShifted[combId_])) * boundaryEval_res[i];
    }

    result_.answer(res);
}

void compositionWithZK_Value::addAnswerPtr(FieldElement* answerPtr){
    result_.addAnswerPtr(answerPtr);
}

void compositionWithZK_Value::calculate(const Protocols::Ali::details::randomCoeffsSet_t& coeffs)const{
    FieldElement res = coeffs.ZK_mask_composition[combId_].coeffUnshifted[0] * calculateCompositionValue() + ZK_mask_res;

    result_.answer(res);
}

void compositionWithZK_Value::init(const AcspInstance& instance, const FieldElement& consistencyPoint, const unsigned int combId){
    instance_ = &instance;
    consistencyPoint_ = consistencyPoint;
    combId_ = combId;
   
    const size_t numWitnesses = instance.neighborPolys().size();

    boundaryPoly_res.resize(numWitnesses);
    boundaryVanishingVals_.resize(numWitnesses);
    boundaryPolyVals_.resize(numWitnesses);
    
    for(size_t wIdx=0; wIdx < numWitnesses ; wIdx++){
        
        auto& currNeighbors = instance_->neighborPolys()[wIdx];
        auto& currResults = boundaryPoly_res[wIdx];
        auto& currVanishingVals = boundaryVanishingVals_[wIdx];
        auto& currPolyVals = boundaryPolyVals_[wIdx];
        
        currResults.resize(currNeighbors.size());
        currVanishingVals.resize(currNeighbors.size());
        currPolyVals.resize(currNeighbors.size());

        const auto& currBoundaryConstraints = instance_->boundaryConstraints()[wIdx];


        //get the neighbors values for the consistency test
        Algebra::UnivariatePolynomialGeneral Ax(currBoundaryConstraints);
        
        Algebra::elementsSet_t S_x;
        for (const auto& p : currBoundaryConstraints) {
            S_x.insert(p.first);
        }
        
        Algebra::UnivariatePolynomialGeneral Z_X(S_x);

        for (unsigned int i=0; i < currNeighbors.size(); i++) {
            const FieldElement currNeighborVal = currNeighbors[i]->eval(consistencyPoint_);
            currVanishingVals[i] = Z_X.eval(currNeighborVal);
            currPolyVals[i] = Ax.eval(currNeighborVal);
        }
    }
    
    
    //get the vanishing space polynomial value for consistency test (aka Z_H)
    {
        vanishingSpacePolyVal_ = instance_->vanishingSet().vanishingPoly().eval(consistencyPoint_);
    }
}

FieldElement compositionWithZK_Value::calculateCompositionValue()const{
    vector<FieldElement> inputValuesVector;
    inputValuesVector.push_back(consistencyPoint_);

    const size_t numWitnesses = instance_->neighborPolys().size();
    
    for(size_t wIdx=0; wIdx < numWitnesses ; wIdx++){
        
        auto& currNeighbors = instance_->neighborPolys()[wIdx];
        auto& currResults = boundaryPoly_res[wIdx];
        auto& currVanishingVals = boundaryVanishingVals_[wIdx];
        auto& currPolyVals = boundaryPolyVals_[wIdx];

        for (unsigned int i=0; i < currNeighbors.size(); i++) {
            const FieldElement inputValue(currResults[i]);
            inputValuesVector.push_back(inputValue*currVanishingVals[i] + currPolyVals[i]);
        }
    }
    
    const auto& acspPoly = instance_->constraintPoly();
    const FieldElement res = acspPoly.eval(inputValuesVector)/vanishingSpacePolyVal_;

    return res;
}

} // namespace details
} // namespace Verifier
} // namespace Ali
} // namespace Protocols
} // namespace libstark
