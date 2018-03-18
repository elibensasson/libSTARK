#include "prover.hpp"

#include "common/Utils/ErrorHandling.hpp"
#include "common/Utils/TaskReporting.hpp"
#include "protocols/Fri/prover.hpp"
#include "reductions/BairToAcsp/BairToAcsp.hpp"

#include <set>
#if __GNUG__
#include <sys/sysinfo.h>
#endif

namespace libstark{
namespace Protocols{
namespace Ali{
namespace Prover{

using Infrastructure::Log2;
using Infrastructure::POW2;
using CryptoCommitment::hashDigest_t;
using CryptoCommitment::SparceMerkleTree;
using CryptoCommitment::getMerkleCommitmentInplace;
using Ali::details::randomCoeffsSet_t;
using Ali::details::rawQueries_t;
using Ali::details::rawResults_t;
using Ali::details::PCP_common::basisWithShift_t;
using Ali::details::PCP_common::composition_div_ZH_degreeBound;
using Ali::details::PCP_common::boundaryPolysMatrix_logWidth;
using Ali::details::PCP_common::boundaryPolysMatrix_logNumElements;
using Algebra::FieldElement;
using Algebra::PolynomialDegree;
using Algebra::UnivariatePolynomialInterface;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::novelFFT;
using std::unique_ptr;
using std::set;
using std::vector;
//
// Proof constructions auxiliary functions
//
namespace{
    typedef vector<FieldElement> coeffs_t;
    
    void getWitnessesCoefficients(const AcspInstance& acspInstance, const AcspWitness& acspWitness, coeffs_t* result){
        using Algebra::UnivariatePolynomialGeneral;
        using Algebra::zero;
        
        const size_t numOfWitnesses = acspWitness.assignmentPolys().size();
        const size_t degBound = PolynomialDegree::integral_t(acspInstance.witnessDegreeBound()[0]);
        for(const auto& d: acspInstance.witnessDegreeBound()){
            _COMMON_ASSERT(degBound == (size_t)PolynomialDegree::integral_t(d), "Degrees of witness columns are expected to be equal");
        }
        
        const unsigned short witnessDegLog = ceil(Log2(degBound));
        const size_t witnessPow2Deg = POW2(witnessDegLog);

#pragma omp parallel for
        for(unsigned int wIndex = 0; wIndex < numOfWitnesses; wIndex++){

            const Algebra::UnivariatePolynomialInterface& witnessPoly = *(acspWitness.assignmentPolys()[wIndex]);

            if(acspInstance.boundaryConstraints()[wIndex].empty()){
                result[wIndex] = witnessPoly.getCoefficients();
                if (result[wIndex].size() < witnessPow2Deg){
                    result[wIndex].resize(witnessPow2Deg,zero());
                }

                continue;
            }

            unique_ptr<Algebra::UnivariatePolynomialInterface> polyToTest;
            {
                ///Compute the evaluation f:Sm->{0,1} such that forall i=1..|x| f(alpha_i)=x_i
                ///where alpha_i is the i'th element of Sm and x_i is the i'th bit of the instance x.
                ///And writing the input location to the vector S_x
                Algebra::elementsSet_t S_x;
                for (const auto& p : acspInstance.boundaryConstraints()[wIndex]) {
                    S_x.insert(p.first);
                }

                UnivariatePolynomialGeneral Z_X(S_x);

                const auto deltaPoly = UnivariatePolynomialGeneral(acspInstance.boundaryConstraints()[wIndex]) - UnivariatePolynomialGeneral(witnessPoly);
                polyToTest = Z_X.divideByMe(deltaPoly);	
            }

            //New block of witnesses
            {
                result[wIndex] = polyToTest->getCoefficients();
                if (result[wIndex].size() < witnessPow2Deg){
                    result[wIndex].resize(witnessPow2Deg,zero());
                }
            }
        }
    }

    void initWitness_ZkMask_coeffs(FieldElement* res, const size_t numCoeffs){
#pragma omp parallel for
        for(unsigned long long i =0; i< numCoeffs; i++){
            res[i] = Algebra::generateRandom();
        }
    }

    unique_ptr<dataWithCommitment> boundaryPolysEvaluation(const AcspInstance& acspInstance, const AcspWitness& acspWitness, unique_ptr<novelFFT>& fftInstance,bool& entireWitnessKept){
        using std::vector;
        using Algebra::FieldElement;

        /**************************************/
        /** Step 1 - Parameter Instantiation: */
        /**************************************/

        const size_t numOfWitnesses = acspWitness.assignmentPolys().size();
        const size_t numOfWitnessesWithZkMask = numOfWitnesses+1;
        const size_t degBound = PolynomialDegree::integral_t(acspInstance.witnessDegreeBound()[0]);
        for(const auto& d: acspInstance.witnessDegreeBound()){
            _COMMON_ASSERT(degBound == (size_t)PolynomialDegree::integral_t(d), "Degrees of witness columns are expected to be equal");
        }
        const unsigned short witnessDegLog = ceil(Log2(degBound));
        const size_t witnessPow2Deg = POW2(witnessDegLog);

        //
        //Construct ordered basis for proofs
        //
        const auto basisPCPP = Ali::details::PCP_common::basisForWitness(acspInstance);

        //
        //Constructing evaluation
        //
        TASK("Constructing boundary constraints proof : evaluating " 
                + std::to_string(numOfWitnesses) +" witnesses and 1 ZK MASK polynomial on spaces of dimension " + std::to_string(basisPCPP.basis.size()));

        vector<coeffs_t> witnessCoeffs(numOfWitnessesWithZkMask);
        {
            TASK("Computing witness coefficients");
            getWitnessesCoefficients(acspInstance,acspWitness,&witnessCoeffs[0]);
        }
        {
            TASK("Generating Witnesses ZK Mask polynomial coefficients");
            const size_t zkIdx = witnessCoeffs.size()-1;
            vector<FieldElement>& zkMask = witnessCoeffs[zkIdx];
            
            zkMask.resize(witnessPow2Deg);
            initWitness_ZkMask_coeffs(&zkMask[0], witnessPow2Deg);
        }

        {
            TASK("Computing witness evaluations + transpose + Merkle");

            // some space constants
            const vector<FieldElement> cosetBasis(basisPCPP.basis.begin(),basisPCPP.basis.begin()+witnessDegLog);
            const vector<FieldElement> shiftsBasis(basisPCPP.basis.begin()+witnessDegLog, basisPCPP.basis.end());
            const size_t numShifts = POW2(shiftsBasis.size());
            const size_t cosetSize = POW2(cosetBasis.size());
            
            const unsigned short widthLog = boundaryPolysMatrix_logWidth(acspInstance);
            const size_t width = POW2(widthLog);
            
            fftInstance = unique_ptr<novelFFT>(new novelFFT(cosetBasis,std::move(witnessCoeffs),width,Algebra::zero()));
            
            unsigned short logNumCosetsInParallel;
            {
                unsigned short logRAM;
                unsigned short logVM;
                //compute RAM amount on current machine
                {
#if __GNUG__
                    struct sysinfo info;
                    sysinfo(&info);
                    logRAM = std::floor(Log2(info.totalram));
                    logVM = std::floor(Log2(info.totalswap + info.totalram));
                    logVM = std::min(logVM, logRAM); //it seems to be giving best performance
#else
                    logRAM = 33;
                    logVM = 33;
#endif
                }
                const unsigned short logCosetSize = widthLog + cosetBasis.size() + Log2(sizeof(FieldElement));
                entireWitnessKept = (logVM > shiftsBasis.size() + logCosetSize);
                const int logMerkleOverhead = (entireWitnessKept? 1 : 0);
                logNumCosetsInParallel = std::min(shiftsBasis.size(), (size_t)std::max(0,int(logRAM-(logCosetSize+logMerkleOverhead))));
            }
            const size_t numCosetsInParallel = POW2(logNumCosetsInParallel);

            //
            // Handling the case where the proof fits entirely into RAM
            //
            if(entireWitnessKept == true){
                const bool proofFitsInRAM = (logNumCosetsInParallel >= shiftsBasis.size());

                if(proofFitsInRAM){
                    //Evaluate all cosets in parallel
                    TASK("Evaluating all witnesses, will keep them in RAM");
                    return unique_ptr<dataWithCommitment>(new 
                            dataWithCommitment(widthLog + cosetBasis.size(), shiftsBasis.size(),
                                [&](const size_t shiftIdx, FieldElement* res){
                                const FieldElement affineShift = Algebra::getSpaceElementByIndex(shiftsBasis, basisPCPP.shift, shiftIdx);
                                fftInstance->FFT({affineShift}, res, cosetSize*width);
                                }
                                ));
                }
                else{
                    //Evaluate cosets in blocks
                    TASK("Evaluating witnesses in blocks of " + std::to_string(numCosetsInParallel) + " cosets in block, will keep them in RAM");
                    return unique_ptr<dataWithCommitment>(new 
                            dataWithCommitment(widthLog + cosetBasis.size() + logNumCosetsInParallel, shiftsBasis.size() - logNumCosetsInParallel,
                                [&](const size_t globalShiftIdx, FieldElement* res){

                                TASK("Constructing " + std::to_string(numCosetsInParallel) + " cosets of total " + std::to_string(numShifts));

                                vector<FieldElement> affineShifts(numCosetsInParallel);

                                const size_t shiftsStart = globalShiftIdx*numCosetsInParallel;
                                for(size_t shiftId=0; shiftId < numCosetsInParallel; shiftId++){
                                affineShifts[shiftId] = Algebra::getSpaceElementByIndex(shiftsBasis, basisPCPP.shift, shiftsStart + shiftId);
                                
                                }
                                fftInstance->FFT(affineShifts, res, cosetSize*width);
                                },
                                false
                                ));
                }
            }

            vector<FieldElement> merklePeakData(numShifts*sizeof(hashDigest_t)/sizeof(FieldElement));
            hashDigest_t* cosetsHash = (hashDigest_t*)&merklePeakData[0];
            {
                vector<FieldElement> cosetEval(width*cosetSize*numCosetsInParallel);
                for(size_t i=0; i< numShifts/numCosetsInParallel; i++){
                    TASK("Evaluating " + std::to_string(numCosetsInParallel) + " coset of " + std::to_string(numShifts));
                    
                    //init offsets for cosets
                    vector<FieldElement> shifts(numCosetsInParallel);
                    for(size_t shiftIdx=0; shiftIdx < numCosetsInParallel; shiftIdx++){
                        shifts[shiftIdx] = Algebra::getSpaceElementByIndex(shiftsBasis, basisPCPP.shift, i*numCosetsInParallel + shiftIdx);
                    }
                    
                    //evaluate and collect hashes
                    
                    const unsigned int max_threads_machine = omp_get_max_threads();
                    if(numCosetsInParallel < max_threads_machine){
                        
                        //evaluate
                        fftInstance->FFT(shifts, &cosetEval[0], cosetSize*width);

                        for(size_t shiftIdx=0; shiftIdx < numCosetsInParallel; shiftIdx++){
                            FieldElement* currCoset = &cosetEval[shiftIdx*cosetSize*width];

                            //get coset Merkle root
                            cosetsHash[numCosetsInParallel*i + shiftIdx] = getMerkleCommitmentInplace(currCoset,Log2(sizeof(FieldElement)) + widthLog + cosetBasis.size());
                        }

                    }
                    else{
#pragma omp parallel for
                        for(unsigned long long shiftIdx=0; shiftIdx < numCosetsInParallel; shiftIdx++){
                            FieldElement* currCoset = &cosetEval[shiftIdx*cosetSize*width];

                            //evaluate
                            fftInstance->FFT({shifts[shiftIdx]}, currCoset, cosetSize*width);

                            //get coset Merkle root
                            cosetsHash[numCosetsInParallel*i + shiftIdx] = getMerkleCommitmentInplace(currCoset,Log2(sizeof(FieldElement)) + widthLog + cosetBasis.size());
                        }
                    }
                }
            }

            return unique_ptr<dataWithCommitment>(new dataWithCommitment(std::move(merklePeakData),0));
        }
    }
    
    namespace{
        class cachedPoly_t : public Algebra::UnivariatePolynomialInterface{
            public:
                cachedPoly_t(
                        const dataWithCommitment& model,
                        const size_t numColumns,
                        const size_t columnIdx,
                        const UnivariatePolynomialInterface& realModel,
                        const basisWithShift_t& basisPCPP,
                        const UnivariatePolynomialGeneral& vanishingPoly,
                        const UnivariatePolynomialGeneral& boundValsPoly,
                        const bool hasBoundary
                        ): 
                    model_(model),
                    realModel_(realModel),
                    numColumns_(numColumns),
                    columnIdx_(columnIdx),
                    basisPCPP_(basisPCPP),
                    vanishingPoly_(vanishingPoly),
                    boundValsPoly_(boundValsPoly),
                    hasBoundary_(hasBoundary){
                    
                        //verify basisPCPP is of the expected form
                        {
                            for(unsigned int i=0; i+1<basisPCPP_.basis.size(); i++){
                                const FieldElement expected = Algebra::mapIntegerToFieldElement(i,1,1);
                                _COMMON_ASSERT(expected == basisPCPP_.basis[i],"proximity test space basis is not of expected form");
                            }
                            {
                                const size_t lastElemIndex = basisPCPP_.basis.size()-1;
                                const FieldElement expected = Algebra::mapIntegerToFieldElement(lastElemIndex,2,3);
                                _COMMON_ASSERT(expected == basisPCPP_.basis[lastElemIndex],"proximity test space basis is not of expected form");
                            }
                        }

                        //verify the basisPCPP shift is as expected
                        {
                            const short bitShift = basisPCPP_.basis.size()-1;
                            const FieldElement expectedShift = Algebra::mapIntegerToFieldElement(bitShift,1,1);
                            _COMMON_ASSERT(expectedShift == basisPCPP_.shift,"proximity test space shift is not as expected");
                        }
                    }

                FieldElement eval(const FieldElement& x)const{
                    const FieldElement shifted_x = x-basisPCPP_.shift;
                    const size_t index = mapFieldElementToInteger(0,basisPCPP_.basis.size(),shifted_x);
                    const size_t inModelIndex = index*numColumns_ + columnIdx_;

                    if(!hasBoundary_){
                        return model_.getElement(inModelIndex);
                    }
                    
                    const FieldElement res = (model_.getElement(inModelIndex)* vanishingPoly_.eval(x)) + boundValsPoly_.eval(x);
                    return res;
                }

                FieldElement getCoefficient(const unsigned int index)const{
                    _COMMON_FATAL("NOT implemented");
                }

                PolynomialDegree getDegree()const{
                    return realModel_.getDegree();
                }

                unique_ptr<PolynomialInterface> clone()const{
                    _COMMON_FATAL("NOT implemented");
                }

                vector<FieldElement> eval(const vector<FieldElement>& orderedBasis, const FieldElement& shift)const{
                    _COMMON_FATAL("NOT implemented");
                }


            private:
                const dataWithCommitment& model_;
                const UnivariatePolynomialInterface& realModel_;
                const size_t numColumns_;
                const size_t columnIdx_;
                const basisWithShift_t basisPCPP_;
                const UnivariatePolynomialGeneral vanishingPoly_;
                const UnivariatePolynomialGeneral boundValsPoly_;
                const bool hasBoundary_;
        };

        AcspWitness getCachedWitness(const AcspInstance& instance, const AcspWitness& witness, const dataWithCommitment& boundaryPolysMatrix){
            const auto PCPP_space = Ali::details::PCP_common::basisForWitness(instance);

            vector<unique_ptr<const UnivariatePolynomialInterface>> res(witness.assignmentPolys().size());
            for(size_t i=0; i< witness.assignmentPolys().size(); i++){

                //get the neighbors values for the consistency test
                UnivariatePolynomialGeneral vanishingPoly(Algebra::one());
                UnivariatePolynomialGeneral boundValsPoly(Algebra::zero());
                const auto& currBoundaryConstraints = instance.boundaryConstraints()[i];

                bool hasBoundary = false;
                if(!currBoundaryConstraints.empty()){
                    hasBoundary = true;
                    Algebra::elementsSet_t S_x;
                    for (const auto& p : currBoundaryConstraints) {
                        S_x.insert(p.first);
                    }
                    
                    vanishingPoly = UnivariatePolynomialGeneral(S_x);
                    boundValsPoly = UnivariatePolynomialGeneral(currBoundaryConstraints);
                }

                res[i] = unique_ptr<const UnivariatePolynomialInterface>(
                        new cachedPoly_t(boundaryPolysMatrix,POW2(boundaryPolysMatrix_logWidth(instance)),i,*(witness.assignmentPolys()[i]),PCPP_space,vanishingPoly,boundValsPoly,hasBoundary));
            }

            return AcspWitness(std::move(res));
        }
    }

     vector<FieldElement> compositionPolysEvaluation(const AcspInstance& acspInstance, const AcspWitness& acspWitness, const vector<FieldElement>& basis, const FieldElement& shift, const dataWithCommitment& boundaryPolysMatrix, const bool entireWitnessKept){

        using Algebra::zero;
        using std::unique_ptr;

        /**************************************/
        /** Step 1 - Parameter Instantiation: */
        /**************************************/

        /** Filling in the parameters values according to the given parameter t */

        //
        //Construct ordered bases for proofs
        //
        const auto  basisPCPP = Ali::details::PCP_common::basisForConsistency(acspInstance);

        /***************************************************************/
        /** Step 3 - Proof that A satisfies the constraint polynomial: */
        /***************************************************************/
        TASK("Computing B and p1. Size of p0 = " + std::to_string(POW2(basisPCPP.basis.size())));

        vector<FieldElement> polyEval;
        if(entireWitnessKept){
            const auto cachedWitness = getCachedWitness(acspInstance,acspWitness,boundaryPolysMatrix);
            polyEval = acspInstance.composeWithWitness_and_divideByVanishingSpacePoly(cachedWitness,basis,shift,true);
        }
        else{
            polyEval = acspInstance.composeWithWitness_and_divideByVanishingSpacePoly(acspWitness,basis,shift);
        }

        return polyEval;
    }

    unique_ptr<UniEvalWithCommitment> ZK_Composition_PolyEvaluation(const AcspInstance& acspInstance){
        TASK("Generating mask poly for Composition RS proximity ZK");
        const size_t degBound = PolynomialDegree::integral_t(Ali::details::PCP_common::composition_div_ZH_degreeBound(acspInstance));
        UnivariatePolynomialGeneral ZK_poly;
        for(size_t i=0; i <= degBound; i++){
            ZK_poly.setCoefficient(i, Algebra::generateRandom());
        }

        const auto  basisPCPP = Ali::details::PCP_common::basisForConsistency(acspInstance);
        return unique_ptr<UniEvalWithCommitment>(new UniEvalWithCommitment(ZK_poly,basisPCPP.basis,basisPCPP.shift,ceil(Log2(degBound+1))));
    }

    vector<FieldElement> computeUnivariateForPCPP_Witness(const AcspInstance& acspInstance,
            const novelFFT& fftInstance,
            const randomCoeffsSet_t& randCoeffs){
        
        TASK("Constructing univariate polynomial for Witness PCPP proof");

        //
        //Construct ordered bases for proofs
        //
        const auto basisPCPP = Ali::details::PCP_common::basisForWitness(acspInstance);
        const size_t numColumns = POW2(ceil(Log2(1 + acspInstance.witnessDegreeBound().size()))); //+1 for ZK mask poly
        
        const size_t degBound = PolynomialDegree::integral_t(acspInstance.witnessDegreeBound()[0]);
        const unsigned short witnessDegLog = ceil(Log2(degBound));
        const vector<FieldElement> cosetBasis(basisPCPP.basis.begin(),basisPCPP.basis.begin()+witnessDegLog);
        const size_t cosetSize = POW2(cosetBasis.size());

        vector<FieldElement> evaluation(cosetSize*numColumns);
        fftInstance.FFT({Algebra::zero()},&evaluation[0],0);

        vector<FieldElement> cosetVals(cosetSize);
        const size_t zkMaskIdx = acspInstance.boundaryConstraints().size();

        const unsigned short logBlockLen = std::min(10,int(cosetBasis.size()));
        const size_t blockLen = POW2(logBlockLen);
        const size_t numBlocks = POW2(cosetBasis.size() - logBlockLen);
#pragma omp parallel for
        for (unsigned long long blockIdx =0; blockIdx < numBlocks; blockIdx++){
            for (unsigned long long inBlockIdx = 0; inBlockIdx < blockLen; inBlockIdx++){

                const size_t xIdx = blockIdx*blockLen + inBlockIdx;
                const FieldElement x = getSpaceElementByIndex(cosetBasis,Algebra::zero(),xIdx);

                //the ZK mask poly
                cosetVals[xIdx] = evaluation[xIdx*numColumns + zkMaskIdx];

                //witnesses
                for(size_t wIndex =0; wIndex <acspInstance.boundaryConstraints().size(); wIndex++){
                    cosetVals[xIdx] += evaluation[xIdx*numColumns +wIndex]*
                        (
                         randCoeffs.boundary[wIndex].coeffUnshifted
                         +
                         randCoeffs.boundary[wIndex].coeffShifted
                         *
                         power(x,randCoeffs.boundary[wIndex].degShift)
                        );
                }
            }
        }

        {
            TASK("Doing LDE");
            const novelFFT fftCol(cosetBasis, std::move(cosetVals));
            vector<FieldElement> cosetShiftsBasis(basisPCPP.basis.begin()+witnessDegLog, basisPCPP.basis.end());
            vector<FieldElement> cosetShifts(POW2(cosetShiftsBasis.size()));
            for(size_t i=0; i< cosetShifts.size(); i++){
                cosetShifts[i] = getSpaceElementByIndex(cosetShiftsBasis,basisPCPP.shift, i);
            }
            
            vector<FieldElement> res(POW2(basisPCPP.basis.size()));
            fftCol.FFT(cosetShifts,&res[0],cosetSize);
            return res;
        }
    }

    vector<FieldElement> computeUnivariateForPCPP_Composition(const AcspInstance& acspInstance, const AcspWitness& acspWitness,
            const uniEvalsSet_t& uniEvals,
            const randomCoeffsSet_t& randCoeffs,
            const bool entireWitnessKept){
        
        TASK("Constructing univariate polynomial for Composition PCPP proof");

        //
        //Construct ordered basises for proofs
        //
        const auto basisPCPP = Ali::details::PCP_common::basisForConsistency(acspInstance);
        const size_t spaceSize = POW2(basisPCPP.basis.size());

        vector<FieldElement> evaluation = compositionPolysEvaluation(acspInstance,acspWitness, basisPCPP.basis, basisPCPP.shift, *uniEvals.boundaryPolysMatrix, entireWitnessKept);
#pragma omp parallel for
		  for (unsigned long long xIdx = 0; xIdx < spaceSize; xIdx++){
            
            //the ZK mask poly
            evaluation[xIdx] *= randCoeffs.ZK_mask_composition.coeffUnshifted;
            evaluation[xIdx] += uniEvals.ZK_mask_composition->getElement(xIdx);

        }

        return evaluation;
    }
}

//
// Results retrieving auxiliary functions
//
namespace{
    vector<hashDigest_t> fillBoundaryResults(const novelFFT& fftInstance,
            const dataWithCommitment& merkleTop,
            const AcspInstance& instance,
            const rawQueries_t& rawQueries){

        if(rawQueries.boundaryPolysMatrix.empty())return vector<hashDigest_t>(0);

    
        //
        //Construct ordered basis for proofs
        //
        const auto basisPCPP = Ali::details::PCP_common::basisForWitness(instance);

        const size_t degBound = PolynomialDegree::integral_t(instance.witnessDegreeBound()[0]);
        const unsigned short witnessDegLog = ceil(Log2(degBound));
        const vector<FieldElement> cosetBasis(basisPCPP.basis.begin(),basisPCPP.basis.begin()+witnessDegLog);
        const vector<FieldElement> shiftsBasis(basisPCPP.basis.begin()+witnessDegLog, basisPCPP.basis.end());
        const size_t numShifts = POW2(shiftsBasis.size());
        const size_t cosetSize = POW2(cosetBasis.size());
        const unsigned short widthLog = boundaryPolysMatrix_logWidth(instance);
        const unsigned short heightLog = basisPCPP.basis.size();
        const size_t width = POW2(widthLog);

        const unsigned short cosetShiftSize = cosetBasis.size()+widthLog;
        const size_t cosetMask = (1UL<<cosetShiftSize) - 1UL;

        vector<vector<size_t>> queriesByCoset(numShifts);
        vector<size_t> cosetsWithQueries;
        {
            set<size_t> cosetsWithQueriesSet;
            for(const size_t& blockPairIndex : rawQueries.boundaryPolysMatrix){
                const size_t blockIndex = blockPairIndex<<1;
                const size_t FElemIndex = CryptoCommitment::getElementIndex(blockIndex);
                
                const size_t FElemCosetIndex = FElemIndex>>cosetShiftSize;
                const size_t FElemLocalIndex = FElemIndex & cosetMask;
                const size_t blockLocalIndex = CryptoCommitment::getBlockIndex(FElemLocalIndex);
                const size_t blockLocalPairIndex = blockLocalIndex>>1;
                
                queriesByCoset[FElemCosetIndex].push_back(blockLocalPairIndex);
                cosetsWithQueriesSet.insert(FElemCosetIndex);
            }
            cosetsWithQueries.insert(cosetsWithQueries.end(),cosetsWithQueriesSet.begin(), cosetsWithQueriesSet.end());
        }
        const size_t numCosetsToEvaluate = cosetsWithQueries.size();
        
        //    
        //Calculate number of cosets in parallel for performance
        //
        size_t NumCosetsInParallel;
        {
            unsigned short logRAM;
            //compute RAM amount on current machine
            {
#if __GNUG__
                struct sysinfo info;
                sysinfo(&info);
                logRAM = Log2(std::round(info.totalram));
#else
                logRAM = 33;
#endif
            }
            logRAM = std::max(logRAM, (unsigned short)(widthLog + cosetBasis.size() + Log2(sizeof(FieldElement)) + 1));
            NumCosetsInParallel = std::min(numCosetsToEvaluate, (size_t)POW2(size_t(logRAM-(widthLog + cosetBasis.size() + Log2(sizeof(FieldElement))))));
        }
    
        //
        // Results tree
        //
        CryptoCommitment::SparceMerkleTree resultsTree(widthLog + heightLog + Log2(sizeof(FieldElement)));
        vector<FieldElement> cosetsEval(NumCosetsInParallel*cosetSize*width);
        
        for(size_t blockIdx=0; blockIdx < numCosetsToEvaluate; blockIdx+= NumCosetsInParallel){
            const size_t currBlockSize = std::min(numCosetsToEvaluate-blockIdx,NumCosetsInParallel);
            TASK("Evaluating " + std::to_string(currBlockSize) + " cosets of total " + std::to_string(numCosetsToEvaluate));

            //construct affine shifts
            vector<FieldElement> shifts(currBlockSize);
            for(size_t i=0; i<currBlockSize; i++){
                shifts[i] = Algebra::getSpaceElementByIndex(shiftsBasis, basisPCPP.shift, cosetsWithQueries[blockIdx+i]);
            }

            //evaluate the cosets
            
            const unsigned int max_threads_machine = omp_get_max_threads();
            //answer to queries of each coset seperatly
            if(currBlockSize < max_threads_machine){
                fftInstance.FFT(shifts, &cosetsEval[0], cosetSize*width);
                
                for(unsigned long long i=0; i<currBlockSize; i++){
                    FieldElement* cosetEval = &cosetsEval[i*cosetSize*width];
                    const size_t shiftIdx = cosetsWithQueries[blockIdx+i];
                    const auto& queries = queriesByCoset[shiftIdx];

                    //
                    // The common top path common to all indices of this coset
                    //
                    auto merkleTopPath = merkleTop.getMerkleTree().getPathToBlock(shiftIdx);
                    //add at the begining of the path the root of the twin coset
                    {
                        hashDigest_t rootOfPairCoset;
                        FieldElement* asElemVec = (FieldElement*)&rootOfPairCoset;
                        size_t start = CryptoCommitment::getElementIndex(shiftIdx^1UL);
                        size_t end = CryptoCommitment::getElementIndex(1UL+(shiftIdx^1UL));
                        size_t len = end-start;
                        for(size_t i=0; i<len; i++){
                            asElemVec[i] = merkleTop.getElement(start+i);
                        }

                        merkleTopPath.insert(merkleTopPath.begin(),rootOfPairCoset);
                    }

                    //collect data for queries
                    typedef std::pair<std::array<hashDigest_t,2>,CryptoCommitment::path_t> pathPair_t;
                    const size_t numQueries = queries.size();
                    vector<pathPair_t> paths(numQueries);
                    {
                        for(unsigned int q=0; q< numQueries; q++){
                            const size_t blockLocalPairIndex = queries[q];
                            const size_t blockLocalIndex = blockLocalPairIndex<<1;
                            const size_t FElemLocalIndex = CryptoCommitment::getElementIndex(blockLocalIndex);

                            memcpy(&paths[q].first[0],  &cosetEval[FElemLocalIndex], 2*sizeof(hashDigest_t));
                        }
                    }

                    //answer queries
                    const unsigned short treeLogLen = Log2(cosetSize*width*sizeof(FieldElement));
                    const auto queryPaths = CryptoCommitment::getPathToBlocksInPlace(&cosetEval[0], treeLogLen, queries);
                    for(unsigned int q=0; q< numQueries; q++){
                        const size_t blockLocalPairIndex = queries[q];
                        const size_t blockLocalIndex = blockLocalPairIndex<<1;
                        const size_t FElemLocalIndex = CryptoCommitment::getElementIndex(blockLocalIndex);

                        const size_t FElemIndex = FElemLocalIndex | (shiftIdx<<cosetShiftSize);
                        const size_t blockIndex = CryptoCommitment::getBlockIndex(FElemIndex);
                        const size_t blockPairIndex = blockIndex>>1;

                        paths[q].second = queryPaths[q];

                        paths[q].second.insert(paths[q].second.end(),merkleTopPath.begin(),merkleTopPath.end());

                        {
                            resultsTree.addPath(paths[q].first, paths[q].second, blockPairIndex);
                        }
                    }
                }
            }
            else{
#pragma omp parallel for
                for(unsigned long long i=0; i<currBlockSize; i++){
                    FieldElement* cosetEval = &cosetsEval[i*cosetSize*width];
                    const size_t shiftIdx = cosetsWithQueries[blockIdx+i];
                    {
                        fftInstance.FFT({shifts[i]}, &cosetEval[0], cosetSize*width);
                    }
                    const auto& queries = queriesByCoset[shiftIdx];

                    //
                    // The common top path common to all indices of this coset
                    //
                    auto merkleTopPath = merkleTop.getMerkleTree().getPathToBlock(shiftIdx);
                    //add at the begining of the path the root of the twin coset
                    {
                        hashDigest_t rootOfPairCoset;
                        FieldElement* asElemVec = (FieldElement*)&rootOfPairCoset;
                        size_t start = CryptoCommitment::getElementIndex(shiftIdx^1UL);
                        size_t end = CryptoCommitment::getElementIndex(1UL+(shiftIdx^1UL));
                        size_t len = end-start;
                        for(size_t i=0; i<len; i++){
                            asElemVec[i] = merkleTop.getElement(start+i);
                        }

                        merkleTopPath.insert(merkleTopPath.begin(),rootOfPairCoset);
                    }

                    //collect data for queries
                    typedef std::pair<std::array<hashDigest_t,2>,CryptoCommitment::path_t> pathPair_t;
                    const size_t numQueries = queries.size();
                    vector<pathPair_t> paths(numQueries);
                    {
                        for(unsigned int q=0; q< numQueries; q++){
                            const size_t blockLocalPairIndex = queries[q];
                            const size_t blockLocalIndex = blockLocalPairIndex<<1;
                            const size_t FElemLocalIndex = CryptoCommitment::getElementIndex(blockLocalIndex);

                            memcpy(&paths[q].first[0],  &cosetEval[FElemLocalIndex], 2*sizeof(hashDigest_t));
                        }
                    }

                    //answer queries
                    const unsigned short treeLogLen = Log2(cosetSize*width*sizeof(FieldElement));
                    const auto queryPaths = CryptoCommitment::getPathToBlocksInPlace(&cosetEval[0], treeLogLen, queries);
                    for(unsigned int q=0; q< numQueries; q++){
                        const size_t blockLocalPairIndex = queries[q];
                        const size_t blockLocalIndex = blockLocalPairIndex<<1;
                        const size_t FElemLocalIndex = CryptoCommitment::getElementIndex(blockLocalIndex);

                        const size_t FElemIndex = FElemLocalIndex | (shiftIdx<<cosetShiftSize);
                        const size_t blockIndex = CryptoCommitment::getBlockIndex(FElemIndex);
                        const size_t blockPairIndex = blockIndex>>1;

                        paths[q].second = queryPaths[q];

                        paths[q].second.insert(paths[q].second.end(),merkleTopPath.begin(),merkleTopPath.end());

#pragma omp critical
                        {
                            resultsTree.addPath(paths[q].first, paths[q].second, blockPairIndex);
                        }
                    }
                }
            }
        }
        
        return resultsTree.toVector();
    }

    rawResults_t fillResults(const uniEvalsSet_t& uniEvals,const novelFFT& fftInstance,
            const AcspInstance& instance,
            const rawQueries_t& rawQueries,
            const bool entireWitnessKept){

        rawResults_t results;

        {
            {
                TASK("ZK Composition Mask polynomial");
                results.ZK_mask_composition = uniEvals.ZK_mask_composition->answerQueries(rawQueries.ZK_mask_composition).toVector();
            }

            {
                TASK("Boundary polynomials");
                if(entireWitnessKept){
                    results.boundaryPolysMatrix = uniEvals.boundaryPolysMatrix->answerQueries(rawQueries.boundaryPolysMatrix).toVector();
                }
                else{
                    results.boundaryPolysMatrix = fillBoundaryResults(fftInstance, *uniEvals.boundaryPolysMatrix ,instance,rawQueries);
                }
            }
        }

        //return paths
        return results;
    }
}

prover_t::prover_t(const BairInstance& bairInstance, const AcspWitness& witness, const RS_proverFactory_t& RS_proverFactory):
    bairInstance_(bairInstance),
    witness_(witness),
    RS_proverFactory_(RS_proverFactory),
    phase_(Ali::details::phase_t::START_PROTOCOL){
        
        vector<FieldElement> coeffsPi(bairInstance.constraintsPermutation().numMappings());
        vector<FieldElement> coeffsChi(bairInstance.constraintsAssignment().numMappings());
        instance_ = CBairToAcsp::reduceInstance(bairInstance,coeffsPi,coeffsChi);
    }
    
void prover_t::receiveMessage(const TranscriptMessage& msg){
    const Ali::details::verifierMsg& vMsg = dynamic_cast<const Ali::details::verifierMsg&>(msg);
    
    switch(phase_){

    case(Ali::details::phase_t::START_PROTOCOL):
    {
        TASK("Received start protocol request)");
        evaluateBoundryPolys();
        evaluateZK_Composition_mask();
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    case(Ali::details::phase_t::VERIFIER_RANDOMNESS):
    {
        TASK("Receiving randomness from verifier");
        
        instance_ = CBairToAcsp::reduceInstance(bairInstance_,vMsg.coeffsPi,vMsg.coeffsChi);

        RS_prover_witness_ = RS_proverFactory_(
                Ali::details::PCP_common::basisForWitness(*instance_).basis, 
                computeUnivariateForRS_Proximity_Witness(vMsg.randomCoefficients),
                true
            );
        
        RS_prover_composition_ = RS_proverFactory_(
                Ali::details::PCP_common::basisForConsistency(*instance_).basis, 
                computeUnivariateForRS_Proximity_Composition(vMsg.randomCoefficients),
                false
            );
        
        phase_ = Ali::details::advancePhase(phase_);
    }
    
    default:
    if(vMsg.RS_verifier_witness_msg){
        TASK("Received message from Witness RS proximity verifier");
        RS_prover_witness_->receiveMessage(*vMsg.RS_verifier_witness_msg);
    }
    if(vMsg.RS_verifier_composition_msg){
        TASK("Received message from Composition RS proximity verifier");
        RS_prover_composition_->receiveMessage(*vMsg.RS_verifier_composition_msg);
    }
    {
        //Assume queries sent only at the end
        //the proof is destructed after answering to queries
        if(!vMsg.queries.boundaryPolysMatrix.empty()){
            {
                TASK("Deleting IOPP proofs");
                dynamic_cast<Fri::Prover::prover_t*>(RS_prover_witness_.get())->deleteProof();
                dynamic_cast<Fri::Prover::prover_t*>(RS_prover_composition_.get())->deleteProof();
            }
            {
                TASK("Answering queries");
                nextResults_ = answerQueries(vMsg.queries);
            }
        }
    }
    }

}

msg_ptr_t prover_t::sendMessage(){
    
    msg_ptr_t pMsgPtr(new Ali::details::proverMsg());
    auto& pMsg = dynamic_cast<Ali::details::proverMsg&>(*pMsgPtr);

    switch(phase_){
    
    case(Ali::details::phase_t::UNIVARIATE_COMMITMENTS):
    {
        TASK("Sending commitments");
        
        pMsg.commitments.push_back(state_.ZK_mask_composition->getCommitment());
        pMsg.commitments.push_back(state_.boundaryPolysMatrix->getCommitment());
        
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    default:
        TASK("Sending communication from RS proximity prover and queries results");
        
        pMsg.RS_prover_witness_msg = RS_prover_witness_->sendMessage();
        pMsg.RS_prover_composition_msg = RS_prover_composition_->sendMessage();
        pMsg.results = nextResults_;
    }

    return pMsgPtr;
}

void prover_t::evaluateBoundryPolys(){
    state_.boundaryPolysMatrix = boundaryPolysEvaluation(*instance_,witness_,fftInstance_,entireWitnessKept_);
}

void prover_t::evaluateZK_Composition_mask(){
    state_.ZK_mask_composition = ZK_Composition_PolyEvaluation(*instance_);
}

vector<FieldElement> prover_t::computeUnivariateForRS_Proximity_Witness(const Ali::details::randomCoeffsSet_t& randCoeffs)const{
    return computeUnivariateForPCPP_Witness(*instance_, *fftInstance_, randCoeffs);
}

vector<FieldElement> prover_t::computeUnivariateForRS_Proximity_Composition(const Ali::details::randomCoeffsSet_t& randCoeffs)const{
    return computeUnivariateForPCPP_Composition(*instance_, witness_, state_, randCoeffs,entireWitnessKept_);
}

Ali::details::rawResults_t prover_t::answerQueries(const Ali::details::rawQueries_t& queries){
    return fillResults(state_, *fftInstance_, *instance_, queries, entireWitnessKept_);
}


} // namespace Prover
} // namespace Ali
} // namespace Protocols
} // namespace libstark
