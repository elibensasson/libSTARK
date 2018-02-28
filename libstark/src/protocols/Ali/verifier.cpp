#include "verifier.hpp"
#include "common_details/common.hpp"
#include "common/Utils/ErrorHandling.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/TaskReporting.hpp"
#include <algebraLib/PolynomialDegree.hpp>

#include <string>

namespace libstark{
namespace Protocols{
namespace Ali{
namespace Verifier{

using Ali::details::randomCoeffsSet_t;
using Ali::details::rawQueries_t;
using Ali::details::rawResults_t;
using Infrastructure::Log2;
using Algebra::FieldElement;
using Algebra::PolynomialDegree;
using std::vector;

verifier_t::verifier_t(const AcspInstance& instance, const RS_verifierFactory_t& RS_verifierFactory) : 
    instance_(instance),
    phase_(Ali::details::phase_t::START_PROTOCOL)
    {
    
    TASK("Constructing verifier");

    {
        verifyParams(instance);
    }

    //Initialize
    {
    //Boundry - aka Witness
    {
        const short logSizeWitnesses = Ali::details::PCP_common::boundaryPolysMatrix_logNumElements(instance) + Log2(sizeof(FieldElement));

        //boundary
        state_.boundaryPolysMatrix = uniEvalView_t(logSizeWitnesses);
    }

    //composition
    {
        const size_t basisPCPP_size = Ali::details::PCP_common::basisForConsistency(instance).basis.size();
        const short logSizeBytes =  basisPCPP_size + Log2(sizeof(FieldElement));

        //ZK_Composition_mask
        state_.ZK_mask_composition = uniEvalView_t(logSizeBytes);
    }
    }

    //Generate random coefficients
    generateRandomCoefficients(const_cast<Ali::details::randomCoeffsSet_t&>(randCoeffs_));

    //Generate queries
    generateQueries(RS_verifierFactory);
}

void verifier_t::receiveMessage(const TranscriptMessage& msg){
    const Ali::details::proverMsg& pMsg = dynamic_cast<const Ali::details::proverMsg&>(msg);

    switch(phase_){
    
    case(Ali::details::phase_t::UNIVARIATE_COMMITMENTS):
    {
        TASK("Recieved commitments");
        
        keepZK_Composition_maskCommitment(pMsg.commitments[0]);
        keepWitnessCommitment(pMsg.commitments[1]);
       
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    case(Ali::details::phase_t::RS_PROXIMITY):
    {
        TASK("Recieved message for RS proximity prover");
        
        if(!RS_verifier_witness_->doneInteracting()){
            RS_verifier_witness_->receiveMessage(*pMsg.RS_prover_witness_msg);
        }
        
        if(!RS_verifier_composition_->doneInteracting()){
            RS_verifier_composition_->receiveMessage(*pMsg.RS_prover_composition_msg);
        }
        
        if(RS_verifier_witness_->doneInteracting() && RS_verifier_composition_->doneInteracting()){
            phase_ = Ali::details::advancePhase(phase_);
        }
    }
    break;
    
    case(Ali::details::phase_t::RESULTS):
    {
        TASK("Recieved results");
        digestResults(pMsg.results);
        fetchResults();
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    default:
        _COMMON_FATAL("Got into unexpected phase in the protocol");
    }
}

msg_ptr_t verifier_t::sendMessage(){
    msg_ptr_t vMsgPtr(new Ali::details::verifierMsg());
    auto& vMsg = dynamic_cast<Ali::details::verifierMsg&>(*vMsgPtr);
    switch(phase_){
    
    case(Ali::details::phase_t::START_PROTOCOL):
    {
        TASK("Sending start protocol request)");
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    case(Ali::details::phase_t::VERIFIER_RANDOMNESS):
    {
        TASK("Sending random coefficients for unified RS proximity proof (including ZK rho)");
        vMsg.randomCoefficients = randCoeffs_;
        phase_ = Ali::details::advancePhase(phase_);
    }
    
    case(Ali::details::phase_t::RS_PROXIMITY):
    {
            TASK("Sending message from RS proximity verifier");
            vMsg.RS_verifier_witness_msg = RS_verifier_witness_->sendMessage();
            vMsg.RS_verifier_composition_msg = RS_verifier_composition_->sendMessage();
    }
    break;
    
    case(Ali::details::phase_t::QUERY):
    {
        
        TASK("Sending queries");
        vMsg.queries = getRawQueries();
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    default:
        _COMMON_FATAL("Got into unexpected phase in the protocol");
    }

    return vMsgPtr;
}

void verifier_t::digestQueries(const queriesToInp_t& queriesToInput_witness, const queriesToInp_t& queriesToInput_composition){
    TASK("Verifier digests queries");

    const auto basisPCPP(Ali::details::PCP_common::basisForWitness(instance_));
    const size_t numWitnesses = instance_.witnessDegreeBound().size();
    const size_t numWitnessColumns = Infrastructure::POW2(Ali::details::PCP_common::boundaryPolysMatrix_logWidth(instance_));
    
    //
    // Digest queries made to univariate of Witness PCPP
    //
    {
        TASK("Digest queries to Witness (aka boundary) RS PCPP univariate");
       
        //calculate total number of queries

        linearComb_witness_.resize(queriesToInput_witness.size());
        size_t currCombIdx = 0;
        
        for(const auto& q : queriesToInput_witness){
            
            const size_t x = q.first;
            const auto& resultsAddressList = q.second;

            auto& currComb = linearComb_witness_[currCombIdx++];
            {
                currComb.initLocation(getSpaceElementByIndex(basisPCPP.basis,basisPCPP.shift,x));
                for(FieldElement* location : resultsAddressList){
                    currComb.addAnswerPtr(location);
                }
            }

            {
                {
                    const size_t zkMaskColumnIdx = numWitnesses; //last column, right after the witnesses
                    const size_t x_matrix = (x*numWitnessColumns) + zkMaskColumnIdx;
                    const size_t blockIndex_matrix = CryptoCommitment::getBlockIndex(x_matrix);
                    const short offsetInBlock_matrix = CryptoCommitment::getOffsetInBlock(x_matrix);
                    auto setValFunc = [&](const FieldElement& res){ currComb.ZK_mask_res = res; };
                    
                    state_.boundaryPolysMatrix.queries[blockIndex_matrix][offsetInBlock_matrix].push_back(setValFunc);
                }

                {
                    currComb.boundaryEval_res.resize(numWitnesses);
                    for(size_t wIdx = 0; wIdx < numWitnesses; wIdx++){
                        const size_t x_matrix = (x*numWitnessColumns) + wIdx;
                        const size_t blockIndex_matrix = CryptoCommitment::getBlockIndex(x_matrix);
                        const short offsetInBlock_matrix = CryptoCommitment::getOffsetInBlock(x_matrix);
                        
                        auto setValFunc = [=,&currComb](const FieldElement& res){ currComb.boundaryEval_res[wIdx] = res; };
                        state_.boundaryPolysMatrix.queries[blockIndex_matrix][offsetInBlock_matrix].push_back(setValFunc);
                    }
                }
            }
        }
    }
    
    //
    // Digest queries made to univariate of Composition PCPP
    //
    {
        TASK("Digest queries to Composition RS PCPP univariate");
       
        const auto consistencySpace(Ali::details::PCP_common::basisForConsistency(instance_));

        //calculate total number of queries
        polyEvaluation_composition_.resize(queriesToInput_composition.size());
        size_t currCombIdx = 0;
        
        for(const auto& q : queriesToInput_composition){
            const size_t x = q.first;
            const FieldElement alpha = getSpaceElementByIndex(consistencySpace.basis, consistencySpace.shift, x);
            const size_t blockIndex = CryptoCommitment::getBlockIndex(x);
            const short offsetInBlock = CryptoCommitment::getOffsetInBlock(x);
            const auto& resultsAddressList = q.second;

            auto& currComb = polyEvaluation_composition_[currCombIdx++];
            {
                currComb.init(instance_,alpha);
                for(FieldElement* location : resultsAddressList){
                    currComb.addAnswerPtr(location);
                }
            }

            {
                //ZK mask
                {
                    auto setValFunc = [&](const FieldElement& res){ currComb.ZK_mask_res = res; };
                    state_.ZK_mask_composition.queries[blockIndex][offsetInBlock].push_back(setValFunc);
                }
                
                //Witness polys
                {
                    
                    for (size_t wIndex = 0; wIndex < numWitnesses; wIndex++){

                        const auto& neighbours = instance_.neighborPolys()[wIndex];
                        size_t nsize = neighbours.size();

                        for (unsigned int affine_num = 0; affine_num < nsize; affine_num++){
                            const FieldElement currNeighborVal = neighbours[affine_num]->eval(alpha);

                            { 
                                const size_t neighborValIndex = mapFieldElementToInteger(0,basisPCPP.basis.size(), currNeighborVal + basisPCPP.shift);//getSpaceIndexOfElement(basisPCPP.basis,basisPCPP.shift,currNeighborVal);
                                const size_t x_matrix = (neighborValIndex*numWitnessColumns) + wIndex;
                                const size_t blockIndex_matrix = CryptoCommitment::getBlockIndex(x_matrix);
                                const short offsetInBlock_matrix = CryptoCommitment::getOffsetInBlock(x_matrix);

                                auto setValFunc = [=,&currComb](const FieldElement& res){ currComb.boundaryPoly_res[wIndex][affine_num] = res; };
                                state_.boundaryPolysMatrix.queries[blockIndex_matrix][offsetInBlock_matrix].push_back(setValFunc);
                            }
                        }
                    }
                }
            }
        }
    }
}

rawQueries_t verifier_t::getRawQueries()const{
    TASK("Verifier generates raw queries to be passed to prover");
    
    rawQueries_t res;
    
    //ZK Composition mask 
    {
        TASK("ZK Composition Mask polynomial");
        res.ZK_mask_composition = state_.ZK_mask_composition.getRawQuery();
    }
    
    //boundry 
    {
        TASK("Boundry polynomials");
        res.boundaryPolysMatrix = state_.boundaryPolysMatrix.getRawQuery();
    }
    
    return res;
}

void verifier_t::digestResults(const rawResults_t& rawResults){
    TASK("Verifier digests results");
   
    //ZK Composition mask 
    {
        TASK("ZK Composition Mask polynomial");
        state_.ZK_mask_composition.digestResults(rawResults.ZK_mask_composition);
    }
    
    //boundry 
    {
        TASK("Boundry polynomials");
        state_.boundaryPolysMatrix.digestResults(rawResults.boundaryPolysMatrix);
    }
}

bool verifier_t::verifyComitment()const{
    TASK("Verifying integrity of results with commitments");
    bool res = true;
    
    //ZK Composition mask
    {
        TASK("ZK Composition Mask polynomial");
        res &= state_.ZK_mask_composition.verifyComitment();
    }
    
    //boundry
    {
        TASK("Boundry polynomials");
        res &= state_.boundaryPolysMatrix.verifyComitment();
    }

    return res;
}


void verifier_t::fetchResults()const{
    TASK("Verifyer fetches results");
    
    //ZK Composition mask
    {
        TASK("ZK Composition Mask polynomial");
        state_.ZK_mask_composition.fetchResults();
    }
    
    //boundry
    {
        TASK("Boundry polynomials");
        state_.boundaryPolysMatrix.fetchResults();
    }

    //
    // Calculating linear combinations needed for Witness PCPP
    //
    {
        TASK("Calculating " + std::to_string(linearComb_witness_.size()) + " Witness RS PCPP result from queries to univariate");
        for(const auto& currComb : linearComb_witness_){
            currComb.calculate_witness(randCoeffs_);
        }
    }

    //
    // Calculating constraint poly evaluations and linear combinations needed for Composition PCPP
    //
    {
        TASK("Calculating " + std::to_string(polyEvaluation_composition_.size()) + " Composition RS PCPP result from queries to univariate");
        for(const auto& currComb : polyEvaluation_composition_){
            currComb.calculate(randCoeffs_);
        }
    }
}

void verifier_t::fillResultsAndCommitmentRandomly(){
    
    //random results for local proofs
    state_.ZK_mask_composition.fillResultsAndCommitmentRandomly();
    state_.boundaryPolysMatrix.fillResultsAndCommitmentRandomly();
    
    RS_verifier_composition_->fillResultsAndCommitmentRandomly();
    RS_verifier_witness_->fillResultsAndCommitmentRandomly();


    fetchResults();
}

void verifier_t::verifyParams(const AcspInstance& instance){
    const short PCPP_spaceDim = Ali::details::PCP_common::basisForWitness(instance).basis.size();
    const short ContextField_Dim = Algebra::ExtensionDegree;
    _COMMON_ASSERT((PCPP_spaceDim <= ContextField_Dim),
            std::string("Can't construct a PCP proof for Acsp instance, because the context field is too small\n") + 
            std::string("The context field is of dimension ") + std::to_string(ContextField_Dim) + std::string("\n") +
            std::string("The sub-space for PCPP proof must be at least of dimension ") + std::to_string(PCPP_spaceDim)
            );
}

void verifier_t::keepZK_Composition_maskCommitment(const CryptoCommitment::hashDigest_t& commitment){
    state_.ZK_mask_composition.commitment = commitment;
}

void verifier_t::keepWitnessCommitment(const CryptoCommitment::hashDigest_t& commitment){
    state_.boundaryPolysMatrix.commitment = commitment;
}

void verifier_t::generateQueries(const RS_verifierFactory_t& RS_verifierFactory){
    TASK("VERIFIER QUERIES GENERATION");

    {
        TASK("RS proximity of witness (aka boundary)");
        const auto PCPP_Basis(Ali::details::PCP_common::basisForWitness(instance_));
        const auto deg_composition = Algebra::PolynomialDegree::integral_t(Ali::details::PCP_common::maximalPolyDegSupported_Witness(instance_));
        const short deg_log_composition = ceil(Infrastructure::Log2(deg_composition));
        {
            specsPrinter witnessFriSpecs("FRI for witness (f) specifications");
            RS_verifier_witness_ = RS_verifierFactory(PCPP_Basis.basis, Ali::details::SoundnessParameters::SecurityParameter, deg_log_composition,true, witnessFriSpecs);
            witnessFriSpecs.print();
        }
    }
    
    {
        TASK("RS proximity of composition");
        const auto PCPP_Basis(Ali::details::PCP_common::basisForConsistency(instance_));
        const auto deg_composition = Algebra::PolynomialDegree::integral_t(Ali::details::PCP_common::composition_div_ZH_degreeBound(instance_));
        const short deg_log_composition = ceil(Infrastructure::Log2(deg_composition));
        {
            specsPrinter compFriSpecs("FRI for constraints (g) specifications");
            RS_verifier_composition_ = RS_verifierFactory(PCPP_Basis.basis, Ali::details::SoundnessParameters::SecurityParameter, deg_log_composition,false,compFriSpecs);
            compFriSpecs.print();
        }
    }

    digestQueries(RS_verifier_witness_->queriesToInput(),RS_verifier_composition_->queriesToInput());
}

void verifier_t::generateRandomCoefficients(Ali::details::randomCoeffsSet_t& randCoeffs){
    TASK("Verifier generates random coefficients");

    //a coefficient for the ZK masks
    randCoeffs.ZK_mask_boundary.coeffUnshifted = Algebra::generateRandom();
    randCoeffs.ZK_mask_composition.coeffUnshifted = Algebra::generateRandom();

    {
    //get degree bounds
    const PolynomialDegree::integral_t requiredDegreeBound = PolynomialDegree::integral_t(Ali::details::PCP_common::maximalPolyDegSupported_Witness(instance_));

    //boundary witnesses
    const auto& degBounds = Ali::details::PCP_common::witness_div_Z_Boundery_degreeBound(instance_);
    for(size_t wIndex =0 ; wIndex < instance_.witnessDegreeBound().size(); wIndex++){

        Protocols::Ali::details::randomCoeefs currCoeffs;

        //calculate expected degree bound
        const PolynomialDegree::integral_t bounderyPoly_bound = PolynomialDegree::integral_t(degBounds[wIndex]);

        //init degree shifts to fit to the bounds
        currCoeffs.degShift = requiredDegreeBound - bounderyPoly_bound;

        //fill random coefficients
        currCoeffs.coeffUnshifted = Algebra::generateRandom();
        currCoeffs.coeffShifted = Algebra::generateRandom();

        //add random coefficient to list
        randCoeffs.boundary.push_back(currCoeffs);
    }
    }
}

bool verifier_t::verify()const{
    bool res = verifyComitment();
    
    {
        TASK("Calling Witness RS Proximity verifier");
        res &= RS_verifier_witness_->verify();
    }
    
    {
        TASK("Calling Composition RS Proximity verifier");
        res &= RS_verifier_composition_->verify();
    }

    return res;
}

bool verifier_t::doneInteracting()const{
    return phase_ == Ali::details::phase_t::DONE;
}

size_t verifier_t::expectedCommitedProofBytes()const{
    const size_t basisPCPP_size = Ali::details::PCP_common::basisForWitness(instance_).basis.size();
    const size_t evaluationBytes = Infrastructure::POW2(basisPCPP_size) * sizeof(FieldElement);
    const size_t evaluationWithCommitmentBytes = 2UL * evaluationBytes;
    
    // *2 for merkle 
    const size_t witnessMatrixSize = 2UL*Infrastructure::POW2(Ali::details::PCP_common::boundaryPolysMatrix_logNumElements(instance_)) * sizeof(FieldElement);
    
    //Witness matrix + witness ZK mask
    const size_t localProofsBytes = evaluationWithCommitmentBytes + witnessMatrixSize;
    const size_t RS_proximityProofBytes_witness = RS_verifier_witness_->expectedCommitedProofBytes();
    const size_t RS_proximityProofBytes_composition = RS_verifier_composition_->expectedCommitedProofBytes();
    
    const size_t evaluationWithCommitmentBytes_composition = 2UL * evaluationBytes;
    
    const size_t localProofsBytes_composition = evaluationWithCommitmentBytes_composition * 2UL; //1 for composition and 1 for ZK mask

    return localProofsBytes + localProofsBytes_composition + RS_proximityProofBytes_witness + RS_proximityProofBytes_composition;
}

size_t verifier_t::expectedSentProofBytes()const{

    size_t numExpectedHashes = 0;

    {
        //Adding 1 for the commitments
        numExpectedHashes += 1UL + state_.ZK_mask_composition.expectedResultsLenght();
        numExpectedHashes += 1UL + state_.boundaryPolysMatrix.expectedResultsLenght();
    }

    const size_t localBytes = numExpectedHashes * sizeof(CryptoCommitment::hashDigest_t);
    const size_t RS_proximityBytes_witness = RS_verifier_witness_->expectedSentProofBytes();
    const size_t RS_proximityBytes_composition = RS_verifier_composition_->expectedSentProofBytes();
    
    return localBytes + RS_proximityBytes_witness + RS_proximityBytes_composition;
}

size_t verifier_t::expectedQueriedDataBytes()const{

    size_t numExpectedHashes = 0;

    {
        numExpectedHashes += state_.ZK_mask_composition.expectedQueriedFieldElementsNum();
        numExpectedHashes += state_.boundaryPolysMatrix.expectedQueriedFieldElementsNum();
    }
    
    const size_t localBytes = numExpectedHashes * sizeof(FieldElement);
    const size_t RS_proximityBytes_witness = RS_verifier_witness_->expectedQueriedDataBytes();
    const size_t RS_proximityBytes_composition = RS_verifier_composition_->expectedQueriedDataBytes();
    
    return localBytes + RS_proximityBytes_witness + RS_proximityBytes_composition;
}

} // namespace Verifier
} // namespace Ali
} // namespace Protocols
} // namespace libstark
