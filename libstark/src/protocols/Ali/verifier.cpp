#include "verifier.hpp"
#include "common_details/common.hpp"
#include "common/Utils/ErrorHandling.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/TaskReporting.hpp"
#include "reductions/BairToAcsp/BairToAcsp.hpp"
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
using Algebra::one;
using std::vector;

vector<FieldElement> getRandVector(const unsigned int len){
    vector<FieldElement> res(len);
    for(auto& e : res){
        e = Algebra::generateRandom();
    }

    return res;
}

verifier_t::verifier_t(const BairInstance& bairInstance, const RS_verifierFactory_t& RS_verifierFactory, const unsigned short securityParameter) : 
    bairInstance_(bairInstance),
    phase_(Ali::details::phase_t::START_PROTOCOL)
    {
    
    TASK("Constructing verifier");

    //Soundness per random linear combination
    {
        unsigned int s = securityParameter;
        while (s>0){
            const unsigned soundnessGained = std::min(Ali::details::SoundnessParameters::maxSoundnessPerComb,s);
            combSoundness_.push_back(soundnessGained+1);
            s -= soundnessGained;
        }
    }

    //Random coefficients for constraints
    {
        for(unsigned int i=0; i<combSoundness_.size(); i++){
            coeffsPi_.push_back(getRandVector(bairInstance.constraintsPermutation().numMappings()));
            coeffsChi_.push_back(getRandVector(bairInstance.constraintsAssignment().numMappings()));
            instance_.push_back(CBairToAcsp::reduceInstance(bairInstance,coeffsPi_[i],coeffsChi_[i]));
        }
    }

    {
        verifyParams(*(instance_[0]));
    }

    //Initialize
    {
    //Boundary - aka Witness
    {
        const short logSizeWitnesses = Ali::details::PCP_common::boundaryPolysMatrix_logNumElements(*(instance_[0]), combSoundness_.size()) + Log2(sizeof(FieldElement));

        //boundary
        state_.boundaryPolysMatrix = uniEvalView_t(logSizeWitnesses);
    }

    //composition
    {
        const size_t basisPCPP_size = Ali::details::PCP_common::basisForConsistency(*(instance_[0])).basis.size();
        const short logSizeBytes =  basisPCPP_size + Log2(sizeof(FieldElement));

        //ZK_Composition_mask
        for(unsigned int i=0; i< combSoundness_.size(); i++){
            state_.ZK_mask_composition.push_back(uniEvalView_t(logSizeBytes));
        }
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
        TASK("Received commitments");
        
        keepWitnessCommitment(pMsg.commitments[0]);
        for(unsigned int i=1; i< pMsg.commitments.size(); i++){
            keepZK_Composition_maskCommitment(pMsg.commitments[i],i-1);
        }
       
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    case(Ali::details::phase_t::RS_PROXIMITY):
    {
        TASK("Received message for RS proximity prover");
        
        bool RS_Done = true;
        
        for(unsigned int i=0; i<RS_verifier_witness_.size(); i++){
            if(!RS_verifier_witness_[i]->doneInteracting()){
                RS_verifier_witness_[i]->receiveMessage(*pMsg.RS_prover_witness_msg[i]);
                RS_Done = false;
            }
        }
        
        for(unsigned int i=0; i<RS_verifier_composition_.size(); i++){
            if(!RS_verifier_composition_[i]->doneInteracting()){
                RS_verifier_composition_[i]->receiveMessage(*pMsg.RS_prover_composition_msg[i]);
                RS_Done = false;
            }
        }
        
        if(RS_Done){
            phase_ = Ali::details::advancePhase(phase_);
        }
    }
    break;
    
    case(Ali::details::phase_t::RESULTS):
    {
        TASK("Received results");
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
        vMsg.numRepetitions = combSoundness_.size();
        phase_ = Ali::details::advancePhase(phase_);
    }
    break;
    
    case(Ali::details::phase_t::VERIFIER_RANDOMNESS):
    {
        TASK("Sending random coefficients for unified RS proximity proof (including ZK rho)");
        vMsg.coeffsPi = coeffsPi_;
        vMsg.coeffsChi = coeffsChi_;
        vMsg.randomCoefficients = randCoeffs_;
        phase_ = Ali::details::advancePhase(phase_);
    }
    
    case(Ali::details::phase_t::RS_PROXIMITY):
    {
            TASK("Sending message from RS proximity verifier");
            for(auto& v : RS_verifier_witness_){
                vMsg.RS_verifier_witness_msg.push_back(v->sendMessage());
            }
            
            for(auto& v : RS_verifier_composition_){
                vMsg.RS_verifier_composition_msg.push_back(v->sendMessage());
            }
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

void verifier_t::digestQueries(const vector<const queriesToInp_t*>& queriesToInput_witness, const vector<const queriesToInp_t*>& queriesToInput_composition){
    TASK("Verifier digests queries");

    const auto basisPCPP(Ali::details::PCP_common::basisForWitness(*(instance_[0])));
    const size_t numWitnesses = instance_[0]->witnessDegreeBound().size();
    const size_t numWitnessColumns = Infrastructure::POW2(Ali::details::PCP_common::boundaryPolysMatrix_logWidth(*(instance_[0]), combSoundness_.size()));
    
    //
    // Digest queries made to univariate of Witness PCPP
    //
    {
        TASK("Digest queries to Witness (aka boundary) RS PCPP univariate");
      
        //calculate total number of queries
        {
            unsigned int numLinearCombinations = 0;
            for(const auto& qSet : queriesToInput_witness){
                numLinearCombinations += qSet->size();
            }
            linearComb_witness_.resize(numLinearCombinations);
        }

        unsigned int combIdx = 0;
        for(unsigned int ldeCombinationId =0; ldeCombinationId< queriesToInput_witness.size(); ldeCombinationId++){
            for(const auto& q : *(queriesToInput_witness[ldeCombinationId])){

                const size_t x = q.first;
                const auto& resultsAddressList = q.second;

                auto& currComb = linearComb_witness_[combIdx++];
                {
                    currComb.initLocation(getSpaceElementByIndex(basisPCPP.basis,basisPCPP.shift,x), ldeCombinationId);
                    for(FieldElement* location : resultsAddressList){
                        currComb.addAnswerPtr(location);
                    }
                }

                {
                    {
                        const size_t zkMaskColumnIdx = numWitnesses + ldeCombinationId; //The zk mask of the i'th LDE is the i'th poly after witness
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
    }

    
    //
    // Digest queries made to univariate of Composition PCPP
    //
    {
        TASK("Digest queries to Composition RS PCPP univariate");
       
        const auto consistencySpace(Ali::details::PCP_common::basisForConsistency(*(instance_[0])));

        //calculate total number of queries
        {
            unsigned int numLinearCombinations = 0;
            for(const auto& qSet : queriesToInput_composition){
                numLinearCombinations += qSet->size();
            }
            polyEvaluation_composition_.resize(numLinearCombinations);
        }
        size_t currCombIdx = 0;
        
        for(unsigned int ldeCombinationId =0; ldeCombinationId< queriesToInput_composition.size(); ldeCombinationId++){
            for(const auto& q : *(queriesToInput_composition[ldeCombinationId])){
                const size_t x = q.first;
                const FieldElement alpha = getSpaceElementByIndex(consistencySpace.basis, consistencySpace.shift, x);
                const size_t blockIndex = CryptoCommitment::getBlockIndex(x);
                const short offsetInBlock = CryptoCommitment::getOffsetInBlock(x);
                const auto& resultsAddressList = q.second;

                auto& currComb = polyEvaluation_composition_[currCombIdx++];
                {
                    currComb.init(*(instance_[ldeCombinationId]),alpha,ldeCombinationId);
                    for(FieldElement* location : resultsAddressList){
                        currComb.addAnswerPtr(location);
                    }
                }

                {
                    //ZK mask
                    {
                        auto setValFunc = [&](const FieldElement& res){ currComb.ZK_mask_res = res; };
                        state_.ZK_mask_composition[ldeCombinationId].queries[blockIndex][offsetInBlock].push_back(setValFunc);
                    }

                    //Witness polys
                    {

                        for (size_t wIndex = 0; wIndex < numWitnesses; wIndex++){

                            const auto& neighbours = instance_[0]->neighborPolys()[wIndex];
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
}

rawQueries_t verifier_t::getRawQueries()const{
    TASK("Verifier generates raw queries to be passed to prover");
    
    rawQueries_t res;
    
    //ZK Composition mask 
    {
        TASK("ZK Composition Mask polynomial");
        for (const auto& v : state_.ZK_mask_composition){
            res.ZK_mask_composition.push_back(v.getRawQuery());
        }
    }
    
    //boundry 
    {
        TASK("Boundary polynomials");
        res.boundaryPolysMatrix = state_.boundaryPolysMatrix.getRawQuery();
    }
    
    return res;
}

void verifier_t::digestResults(const rawResults_t& rawResults){
    TASK("Verifier digests results");
   
    //ZK Composition mask 
    {
        TASK("ZK Composition Mask polynomial");
        for(unsigned int i=0; i< state_.ZK_mask_composition.size(); i++){
            state_.ZK_mask_composition[i].digestResults(rawResults.ZK_mask_composition[i]);
        }
    }
    
    //boundary 
    {
        TASK("Boundary polynomials");
        state_.boundaryPolysMatrix.digestResults(rawResults.boundaryPolysMatrix);
    }
}

bool verifier_t::verifyComitment()const{
    TASK("Verifying integrity of results with commitments");
    bool res = true;
    
    //ZK Composition mask
    {
        TASK("ZK Composition Mask polynomial");
        for(const auto& v: state_.ZK_mask_composition){
            res &= v.verifyComitment();
        }
    }
    
    //boundary
    {
        TASK("Boundary polynomials");
        res &= state_.boundaryPolysMatrix.verifyComitment();
    }

    return res;
}


void verifier_t::fetchResults()const{
    TASK("Verifier fetches results");
    
    //ZK Composition mask
    {
        TASK("ZK Composition Mask polynomial");
        for (auto& v: state_.ZK_mask_composition){
            v.fetchResults();
        }
    }
   
    //boundary
    {
        TASK("Boundary polynomials");
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
    for(auto& v : state_.ZK_mask_composition){
        v.fillResultsAndCommitmentRandomly();
    }
    
    state_.boundaryPolysMatrix.fillResultsAndCommitmentRandomly();
    
    for(auto& v : RS_verifier_composition_){
        v->fillResultsAndCommitmentRandomly();
    }
    
    for(auto& v : RS_verifier_witness_){
        v->fillResultsAndCommitmentRandomly();
    }


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

void verifier_t::keepZK_Composition_maskCommitment(const CryptoCommitment::hashDigest_t& commitment, const unsigned int maskIdx){
    state_.ZK_mask_composition[maskIdx].commitment = commitment;
}

void verifier_t::keepWitnessCommitment(const CryptoCommitment::hashDigest_t& commitment){
    state_.boundaryPolysMatrix.commitment = commitment;
}

void verifier_t::generateQueries(const RS_verifierFactory_t& RS_verifierFactory){
    TASK("VERIFIER QUERIES GENERATION");

    {
        TASK("RS proximity of witness (aka boundary)");
        const auto PCPP_Basis(Ali::details::PCP_common::basisForWitness(*(instance_[0])));
        const auto deg_composition = Algebra::PolynomialDegree::integral_t(Ali::details::PCP_common::maximalPolyDegSupported_Witness(*(instance_[0])));
        const short deg_log_composition = ceil(Infrastructure::Log2(deg_composition));
        {
            for(const auto& s: combSoundness_){
                specsPrinter witnessFriSpecs("FRI for witness (f) specifications #" + std::to_string(RS_verifier_witness_.size()+1));
                RS_verifier_witness_.push_back(RS_verifierFactory(PCPP_Basis.basis, s, deg_log_composition,true, witnessFriSpecs));
                witnessFriSpecs.print();
            }
        }
    }
    
    {
        TASK("RS proximity of composition");
        const auto PCPP_Basis(Ali::details::PCP_common::basisForConsistency(*(instance_[0])));
        const auto deg_composition = Algebra::PolynomialDegree::integral_t(Ali::details::PCP_common::composition_div_ZH_degreeBound(*(instance_[0])));
        const short deg_log_composition = ceil(Infrastructure::Log2(deg_composition));
        {
            for(const auto& s: combSoundness_){
                specsPrinter compFriSpecs("FRI for constraints (g) specifications #" + std::to_string(RS_verifier_composition_.size()+1));
                RS_verifier_composition_.push_back(RS_verifierFactory(PCPP_Basis.basis, s, deg_log_composition,false,compFriSpecs));
                compFriSpecs.print();
            }
        }
    }

    {
        vector<const queriesToInp_t*> queriesWitness;
        vector<const queriesToInp_t*> queriesComposition;
        for (unsigned int i=0; i< combSoundness_.size(); i++){
            queriesWitness.push_back(&(RS_verifier_witness_[i]->queriesToInput()));
            queriesComposition.push_back(&(RS_verifier_composition_[i]->queriesToInput()));
        }
        digestQueries(queriesWitness,queriesComposition);
    }
}

void verifier_t::generateRandomCoefficients(Ali::details::randomCoeffsSet_t& randCoeffs){
    TASK("Verifier generates random coefficients");

    {
    //get degree bounds
    const PolynomialDegree::integral_t requiredDegreeBound = PolynomialDegree::integral_t(Ali::details::PCP_common::maximalPolyDegSupported_Witness(*(instance_[0])));

    //boundary witnesses
    const auto& degBounds = Ali::details::PCP_common::witness_div_Z_Boundery_degreeBound(*(instance_[0]));
    for(size_t wIndex =0 ; wIndex < instance_[0]->witnessDegreeBound().size(); wIndex++){

        Protocols::Ali::details::randomCoeefs currCoeffs;

        //calculate expected degree bound
        const PolynomialDegree::integral_t bounderyPoly_bound = PolynomialDegree::integral_t(degBounds[wIndex]);

        //init degree shifts to fit to the bounds
        currCoeffs.degShift = requiredDegreeBound - bounderyPoly_bound;

        currCoeffs.coeffUnshifted.resize(combSoundness_.size());
        currCoeffs.coeffShifted.resize(combSoundness_.size());

        //fill random coefficients
        for(size_t combId=0; combId < combSoundness_.size(); combId++){
            currCoeffs.coeffUnshifted[combId] = Algebra::generateRandom();
            currCoeffs.coeffShifted[combId] = Algebra::generateRandom();
        }

        //add random coefficient to list
        randCoeffs.boundary.push_back(currCoeffs);
    }
    }

    randCoeffs.ZK_mask_composition.resize(combSoundness_.size());
    for(auto& e : randCoeffs.ZK_mask_composition){
        e.coeffUnshifted.push_back(Algebra::generateRandom());
    }
}

bool verifier_t::verify()const{
    bool res = verifyComitment();
    
    {
        TASK("Calling Witness RS Proximity verifier");
        for (const auto& v : RS_verifier_witness_){
            res &= v->verify();
        }
    }
    
    {
        TASK("Calling Composition RS Proximity verifier");
        for (const auto& v : RS_verifier_composition_){
            res &= v->verify();
        }
    }

    return res;
}

bool verifier_t::doneInteracting()const{
    return phase_ == Ali::details::phase_t::DONE;
}

size_t verifier_t::expectedCommitedProofBytes()const{
    const size_t basisPCPP_size = Ali::details::PCP_common::basisForWitness(*(instance_[0])).basis.size();
    const size_t evaluationBytes = Infrastructure::POW2(basisPCPP_size) * sizeof(FieldElement);
    const size_t evaluationWithCommitmentBytes = 2UL * evaluationBytes;
    
    // *2 for merkle 
    const size_t witnessMatrixSize = 2UL*Infrastructure::POW2(Ali::details::PCP_common::boundaryPolysMatrix_logNumElements(*(instance_[0]), combSoundness_.size())) * sizeof(FieldElement);
    
    //Witness matrix + witness ZK mask
    const size_t localProofsBytes = evaluationWithCommitmentBytes + witnessMatrixSize;
    size_t RS_proximityProofBytes_witness = 0;
    for (const auto& v : RS_verifier_witness_){
        RS_proximityProofBytes_witness += v->expectedCommitedProofBytes();
    }
    
    size_t RS_proximityProofBytes_composition = 0;
    for (const auto& v : RS_verifier_composition_){
        RS_proximityProofBytes_composition += v->expectedCommitedProofBytes();
    }
    
    const size_t evaluationWithCommitmentBytes_composition = 2UL * evaluationBytes;
    
    const size_t localProofsBytes_composition = evaluationWithCommitmentBytes_composition * 2UL; //1 for composition and 1 for ZK mask

    return localProofsBytes + localProofsBytes_composition + RS_proximityProofBytes_witness + RS_proximityProofBytes_composition;
}

size_t verifier_t::expectedSentProofBytes()const{

    size_t numExpectedHashes = 0;

    {
        //Adding 1 for the commitments
        for(const auto& v : state_.ZK_mask_composition){
            numExpectedHashes += 1UL + v.expectedResultsLenght();
        }
        numExpectedHashes += 1UL + state_.boundaryPolysMatrix.expectedResultsLenght();
    }

    const size_t localBytes = numExpectedHashes * sizeof(CryptoCommitment::hashDigest_t);
    size_t RS_proximityBytes_witness = 0;
    for(const auto& v : RS_verifier_witness_){
        RS_proximityBytes_witness += v->expectedSentProofBytes();
    }
    
    size_t RS_proximityBytes_composition = 0;
    for(const auto& v : RS_verifier_composition_){
        RS_proximityBytes_composition += v->expectedSentProofBytes();
    }
    
    return localBytes + RS_proximityBytes_witness + RS_proximityBytes_composition;
}

size_t verifier_t::expectedQueriedDataBytes()const{

    size_t numExpectedHashes = 0;

    {
        for (const auto& v : state_.ZK_mask_composition){
            numExpectedHashes += v.expectedQueriedFieldElementsNum();
        }
        numExpectedHashes += state_.boundaryPolysMatrix.expectedQueriedFieldElementsNum();
    }
    
    const size_t localBytes = numExpectedHashes * sizeof(FieldElement);
    size_t RS_proximityBytes_witness =0;
    for(const auto& v : RS_verifier_witness_){
        RS_proximityBytes_witness += v->expectedQueriedDataBytes();
    }
    
    size_t RS_proximityBytes_composition =0;
    for(const auto& v : RS_verifier_composition_){
        RS_proximityBytes_composition += v->expectedQueriedDataBytes();
    }
    
    return localBytes + RS_proximityBytes_witness + RS_proximityBytes_composition;
}

} // namespace Verifier
} // namespace Ali
} // namespace Protocols
} // namespace libstark
