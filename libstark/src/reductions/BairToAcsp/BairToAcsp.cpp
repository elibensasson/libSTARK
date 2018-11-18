/**************************************** BairToAcsp.cpp *****************************************/
/**
 * @file.
 *
 * Implements the BairToAcsp reduction class.
 */
  /***********************************************************************************************/

#include "BairToAcsp.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Algebra/LinearSpace.hpp"
#include "common/Utils/TaskReporting.hpp"
#include "Details/witnessReduction.hpp"
#include "Details/constraints.hpp"
#include "Details/common.hpp"
#include "Details/instanceMappings.hpp"
#include "Details/neighborsConstructor.hpp"
#include "Details/spaces.hpp"
#include "Details/boundaryConstraints.hpp"
#include "Details/AcspSummandsPolynomial.hpp"

#include <algebraLib/UnivariatePolynomialGeneral.hpp>
#include <algebraLib/novelFFT.hpp>

#include <queue>
#include <array>

namespace libstark {

using Algebra::FieldElement;
using Algebra::LinearSpace;
using Algebra::UnivariatePolynomialInterface;
using libstark::BairToAcsp::instanceMappings;
using libstark::BairToAcsp::common;
using libstark::BairToAcsp::AcspNeighbors;
using libstark::BairToAcsp::CS_testLocations;
using libstark::BairToAcsp::spaces;
using libstark::BairToAcsp::constraints;
using libstark::BairToAcsp::reduceBoundaryConstraints;
using Infrastructure::POW2;

using std::unique_ptr;
using std::move;
using std::array;
using std::vector;
using std::pair;

unique_ptr<AcspWitness> CBairToAcsp::reduceWitness( const BairInstance& instance, const BairWitness& witness){

    return BairToAcsp::witnessReduction::reduceWitness(instance,witness);
}


/**********************************************
 *             Instance Reduction
 *********************************************/

namespace{
    //get vanishing space properties
    vector<FieldElement> getVanishingSpace(const spaces& spacesGenerator){

        const auto basis = spacesGenerator.getVanishingSpaceBasis();
        const vector<FieldElement> result(basis.begin(),basis.end());

        //return the result
        return result;
    }
	

vector<FieldElement> compositionAlg(const AcspInstance& instance, const AcspWitness& witness, const vector<FieldElement>& basisPCPP, const FieldElement& shiftPCPP, const bool witnessIsEvaluation){
    using std::vector;
    using std::min;
    using std::max;
    using std::unique_ptr;
    using Algebra::FieldElement;
    using Algebra::PolynomialDegree;
    using Algebra::zero;
    using Algebra::UnivariatePolynomialGeneral;
    using Algebra::mapIntegerToFieldElement;
    using Algebra::novelFFT;
    using Infrastructure::Log2;
    using BairToAcsp::AcspSummandsPolynomial;

    //the constraints poly
    const AcspSummandsPolynomial& constraintsPoly = *(dynamic_cast<const AcspSummandsPolynomial*>(&instance.constraintPoly()));
    
    //
    //Get the composition degree bound
    //
    vector<PolynomialDegree> constraintsInputDegrees;

    // first input is "x" which has degree 1
    constraintsInputDegrees.push_back(PolynomialDegree(1));

    // rest are composition of neighbor with witness
    for(size_t wIndex = 0 ; wIndex < witness.assignmentPolys().size(); wIndex++){
        const auto witnessDegree = witness.assignmentPolys()[wIndex]->getDegree();
        for (const auto& n : instance.neighborPolys()[wIndex]){
            constraintsInputDegrees.push_back(n->getDegreeBound(witnessDegree));
        }
    }

    // get the composition degree bound
    const PolynomialDegree degBound = constraintsPoly.getDegreeBound_DividedByZH(constraintsInputDegrees);
    const size_t degBoundLog = ceil(Log2(max(PolynomialDegree::integral_t(degBound)+1,PolynomialDegree::integral_t(2))));

    //
    //construct composition evaluation basis 
    //
    vector<FieldElement> compositionBasis(degBoundLog);
    if(witnessIsEvaluation){
        compositionBasis = vector<FieldElement>(basisPCPP.begin(),basisPCPP.begin()+degBoundLog);
    }
    else{
        for(size_t i=0; i< degBoundLog; i++){
            compositionBasis[i] = mapIntegerToFieldElement(i,1,1);
            _COMMON_ASSERT(compositionBasis[i] == basisPCPP[i], "Unsupported basis");
        }
    }
    
    const FieldElement evaluationAffineShift = (witnessIsEvaluation? shiftPCPP : mapIntegerToFieldElement(degBoundLog+1,1,1));

    vector<FieldElement> evaluation(Infrastructure::POW2(compositionBasis.size()));
    
    vector<size_t> neighborsIndexOffset;
    size_t numNeighbors = 0;
    for(size_t wIndex = 0 ; wIndex < witness.assignmentPolys().size(); wIndex++){
        neighborsIndexOffset.push_back(numNeighbors);
        for(size_t n=0; n < instance.neighborPolys()[wIndex].size(); n++){
            numNeighbors++;
        }
    }

    const size_t wDegBound = PolynomialDegree::integral_t(instance.witnessDegreeBound()[0])+1;
    for(const auto& d: instance.witnessDegreeBound()){
        _COMMON_ASSERT((long long)wDegBound-1 == PolynomialDegree::integral_t(d), "Degrees of witness columns are expected to be equal");
    }
    const size_t wDim = Log2(wDegBound);
    const vector<FieldElement> wEvalBasis(compositionBasis.begin(), compositionBasis.begin()+wDim);
    const vector<FieldElement> wOffsetBasis(compositionBasis.begin()+wDim, compositionBasis.end());
    const size_t numEvalOffsets = evaluation.size() >> wDim;
    const size_t numWitnessOffsets = Infrastructure::POW2(wOffsetBasis.size());
    
    vector<vector<FieldElement>> witnessOffsets;
    {
        witnessOffsets.resize(numWitnessOffsets);
        const FieldElement carryOut = mapIntegerToFieldElement(wDim,1,1);
        for(size_t i=0; i<numWitnessOffsets; i++){
            auto& currOffsets = witnessOffsets[i];
            const FieldElement genElem = getSpaceElementByIndex(wOffsetBasis,Algebra::zero(),i) + evaluationAffineShift;
            
            currOffsets.push_back(genElem);
            currOffsets.push_back(genElem*Algebra::xFE());
            currOffsets.push_back(genElem*Algebra::xFE() + carryOut);
        }
    }
    
    const size_t numWitnesses = witness.assignmentPolys().size();
    const size_t cosetSize = POW2(wEvalBasis.size());
    const size_t numCosetsPerEval = 3;
    const size_t witnessEvalBlockSize = numWitnesses*cosetSize*numCosetsPerEval;

    std::unique_ptr<novelFFT> fftInstance;
    if(!witnessIsEvaluation){
        typedef vector<FieldElement> polyCoeffs_t;
        vector<polyCoeffs_t> polys(numWitnesses);
        for(unsigned int wIndex = 0 ; wIndex < numWitnesses; wIndex++){
            polys[wIndex] = witness.assignmentPolys()[wIndex]->getCoefficients();
        }

        fftInstance = std::unique_ptr<novelFFT>(new novelFFT(wEvalBasis,std::move(polys),numWitnesses,Algebra::zero()));
    }
    
    //construct evaluation
    {
        TASK("Evaluate the composition polynomial divided by Z_H over a space of degree " + std::to_string(compositionBasis.size()));
        
        vector<FieldElement> witnessEval;
        if(!witnessIsEvaluation){
            witnessEval.resize(witnessEvalBlockSize);
        }

        for(unsigned long long currOffset = 0; currOffset < numEvalOffsets; currOffset++){
            TASK("Evaluate 1 offset of total " + std::to_string(numEvalOffsets));
            
            if(!witnessIsEvaluation){
                //Evaluate witnesses
                fftInstance->FFT(witnessOffsets[currOffset],&witnessEval[0],numWitnesses*cosetSize);
            }

#pragma omp parallel for schedule(guided)
            for(plooplongtype i=0; i< wDegBound; i++){

                const long currLocation = i + (currOffset<<wDim);

                //iteration constants
                const FieldElement x = getSpaceElementByIndex(compositionBasis,evaluationAffineShift,currLocation);

                //construct the assignment for the constraints poly
                vector<vector<FieldElement>> assignment(1);
                assignment[0].resize(1+numNeighbors);
                assignment[0][0] = x;

                for(size_t wIndex = 0 ; wIndex < witness.assignmentPolys().size(); wIndex++){
                    for(size_t n=0; n < instance.neighborPolys()[wIndex].size(); n++){

                        const FieldElement neighborRes = instance.neighborPolys()[wIndex][n]->eval(x);
                        FieldElement witnessRes;
                        
                        if(!witnessIsEvaluation){
                            const size_t nInCosetIdx = mapFieldElementToInteger(0,wDim,neighborRes);
                            size_t nOffsetIdx = mapFieldElementToInteger(degBoundLog+2,1,neighborRes) + 2*mapFieldElementToInteger(wDim,1,neighborRes);
                            if (nOffsetIdx == 2){nOffsetIdx = 0;}
                            if (nOffsetIdx == 3){nOffsetIdx = 2;}
                            const size_t nIdx = nInCosetIdx + (nOffsetIdx<<wDim);
                            const size_t dataIdx =  wIndex + numWitnesses*nIdx;
                            witnessRes = witnessEval[dataIdx];
                        }
                        else{
                            //the witness is expected to return an evaluation value efficiently
                            witnessRes = witness.assignmentPolys()[wIndex]->eval(neighborRes);
                        }

                        assignment[0][1 + neighborsIndexOffset[wIndex] + n] = witnessRes;
                    }
                }
                //evaluate and return
                evaluation[currLocation]  = constraintsPoly.evalDividedByZH(assignment)[0];
            }
    }
    }
    
    {
        TASK("Doing LDE");
        const novelFFT fftCol(compositionBasis, std::move(evaluation));
        vector<FieldElement> cosetShiftsBasis(basisPCPP.begin()+degBoundLog, basisPCPP.end());
        vector<FieldElement> cosetShifts(POW2(cosetShiftsBasis.size()));
        for(size_t i=0; i< cosetShifts.size(); i++){
            cosetShifts[i] = getSpaceElementByIndex(cosetShiftsBasis,shiftPCPP - evaluationAffineShift, i);
        }

        vector<FieldElement> res(POW2(basisPCPP.size()));
        fftCol.FFT(cosetShifts,&res[0],POW2(compositionBasis.size()));
        return res;
    }
}

class evaluatedUniPoly : public Algebra::UnivariatePolynomialGeneral{
    public:
    evaluatedUniPoly(const AcspInstance& instance, const UnivariatePolynomialInterface& witness):
        Algebra::UnivariatePolynomialGeneral(witness){
            using std::unique_ptr;
            using std::max;
            using Algebra::zero;
            using Algebra::PolynomialDegree;
            using Infrastructure::Log2;

            //
            //define interpolation space
            //
            const size_t vanishingSpaceDim = Log2(instance.vanishingSet().size());
            const size_t witnessDegLog = ceil(Log2(PolynomialDegree::integral_t(witness.getDegree())));
            const size_t witnessEvalBasisSize = max(vanishingSpaceDim + 1, witnessDegLog);
            evaluationBasis_ = Algebra::getStandartOrderedBasis(witnessEvalBasisSize);
            evaluationOffset_ = zero();

            //
            //Evaluate witness on relevant spaces for faster composition
            //
            {
                TASK("Evaluate the witness polynomial over the closure of the vanishing set to the neighbors");
                witnessEval_ = witness.eval(evaluationBasis_,evaluationOffset_);
            }
            
        }

    Algebra::FieldElement eval(const FieldElement& x)const{
        const size_t x_indx = Algebra::mapFieldElementToInteger(0,64,x-evaluationOffset_);
        if(x_indx < Infrastructure::POW2(evaluationBasis_.size())){
                return witnessEval_[x_indx];
        }
        return UnivariatePolynomialGeneral::eval(x);
    }

    private:
    FieldElement evaluationOffset_;
    vector<FieldElement> evaluationBasis_;
    vector<FieldElement> witnessEval_;
};

vector<unique_ptr<UnivariatePolynomialInterface>> witnessCheckerHelper(const AcspInstance& instance, const AcspWitness& witness){
    vector<unique_ptr<UnivariatePolynomialInterface>> res;
    for(size_t wIndex=0; wIndex < witness.assignmentPolys().size(); wIndex++){
        res.push_back(unique_ptr<UnivariatePolynomialInterface>(new evaluatedUniPoly(instance,*witness.assignmentPolys()[wIndex])));
    }

    return res;
}


}
/*
 * Construct an Acsp partial instance from a Bair full instance (top-level arithmetization function for partial instances)
 */
unique_ptr<AcspInstance> CBairToAcsp::reduceInstance(const BairInstance&  instance, const vector<FieldElement>& coeffsPi, const vector<FieldElement>& coeffsChi){
    // get common definition
    const common commonDef(instance, coeffsPi, coeffsChi);

    // fetch more parameters
    const instanceMappings instanceMapping(commonDef);
    const spaces spacesGenerator(commonDef);
    const CS_testLocations testLocations(commonDef);
    const AcspNeighbors neighborsWithInterpretation(instance, commonDef, instanceMapping,testLocations);
    
    // construct the vanishing set
    unique_ptr<AcspInstance::set> vanishingSet(new LinearSpace(getVanishingSpace(spacesGenerator)));
   
    // construct the neighbors vector
    vector<AcspInstance::polynomialsVec> neighbors = neighborsWithInterpretation.getNeighborPolynomials();
    
    //construct the constraints polynomial
    unique_ptr<const AcspInstance::polynomial> constraintsPoly(constraints::getConstraintsPoly(instance,neighborsWithInterpretation,commonDef,instanceMapping, spacesGenerator, testLocations));
    
    //construct the boundary constraints
    std::vector<AcspInstance::boundaryConstraints_t> boundaryConstraints = reduceBoundaryConstraints(instance.boundaryConstraints(),commonDef);

    //construct and return
    unique_ptr<AcspInstance> res(new AcspInstance(move(vanishingSet), move(neighbors), move(constraintsPoly), commonDef.witnessDegreeBound(), boundaryConstraints, compositionAlg, witnessCheckerHelper));

    return res;
}

} // namespace libstark

