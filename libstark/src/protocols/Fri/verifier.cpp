#include "verifier.hpp"
#include "protocols/common/CryptoCommitment/MerkleCommitment.hpp"
#include "common/Utils/TaskReporting.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/ErrorHandling.hpp"

namespace libstark{
namespace Protocols{
namespace Fri{
namespace Verifier{

using std::vector;
using std::pair;
using std::unique_ptr;
using Algebra::FieldElement;
using CryptoCommitment::hashDigest_t;
using CryptoCommitment::SparceMerkleTree;
using details::RS_queriesTree;
using details::addRandomComitmentPath;
using details::addRandomQueryPath;

using common::dimOfColumn;

using common::state_t;
using common::rawQueries_t;
using common::rawResults_t;
using common::verifierRequest_t;
using common::proverResponce_t;
using common::subproofLocation_t;

using Infrastructure::Log2;

namespace{

    size_t expectedQueriedFieldElementsNum(const state_t<uniEvalView_t>& src){
        size_t res = src.localState.expectedQueriedFieldElementsNum();

        for(auto& subproof : src.subproofs){
            res += expectedQueriedFieldElementsNum(subproof.second);
        }

        return res;
    }

    size_t expectedHashesInResults(const state_t<uniEvalView_t>& src){
        size_t res = src.localState.expectedResultsLenght();

        for(auto& subproof : src.subproofs){
            res += expectedHashesInResults(subproof.second);
        }

        return res;
    }

    size_t expectedProofSize(const state_t<uniEvalView_t>& src){
        size_t res = Infrastructure::POW2(src.localState.getLogSizeBytes());

        for(auto& subproof : src.subproofs){
            res += expectedProofSize(subproof.second);
        }

        return res;
    }


    void fill_queries(
            const RS_queriesTree& srcQueries,
            state_t<uniEvalView_t>& dstState,
            const size_t dimOfL){

        //local queries
        dstState.localState.view = SparceMerkleTree(dimOfL + Log2(sizeof(FieldElement)));
        for(const auto& q : srcQueries.localState){

            const size_t x = q.first;
            const size_t blockIndex = CryptoCommitment::getBlockIndex(x);
            const short offsetInBlock = CryptoCommitment::getOffsetInBlock(x);

            auto setValFunc = [=,&q](const FieldElement& res){ q.second.answer(res); };
            dstState.localState.queries[blockIndex][offsetInBlock].push_back(setValFunc);
        }
        
        //child queries
        const size_t columnBasisSize = dimOfColumn(dimOfL);
        for(const auto& subproofQuerie : srcQueries.subproofs){
            fill_queries(subproofQuerie.second,dstState.subproofs[subproofQuerie.first],columnBasisSize);
        }
    }

    void getRawQuery_IOPP(rawQueries_t& dst ,const state_t<uniEvalView_t>& src){
        dst.localState = src.localState.getRawQuery();

        for(auto& subproof : src.subproofs){
            getRawQuery_IOPP(dst.subproofs[subproof.first], subproof.second);
        }
    }

    void digestResults_rec(state_t<uniEvalView_t>& dst, const rawResults_t& src){
        dst.localState.digestResults(src.localState);

        for(auto& subproof : dst.subproofs){
            digestResults_rec(subproof.second,src.subproofs.at(subproof.first));
        }
    }

    void fillResultsAndCommitmentsRandomly_rec(state_t<uniEvalView_t>& dst){
        dst.localState.fillResultsAndCommitmentRandomly();
        
        for(auto& subproof : dst.subproofs){
            fillResultsAndCommitmentsRandomly_rec(subproof.second);
        }
    }

    bool verifyComitment_rec(const state_t<uniEvalView_t>& currState){
        bool res = currState.localState.verifyComitment();

        for(const auto& subproof : currState.subproofs){
            res &= verifyComitment_rec(subproof.second);
        }

        return res;
    }

    void fetchResults_rec(const state_t<uniEvalView_t>& currState){

        currState.localState.fetchResults();

        for(const auto& subproof : currState.subproofs){
            fetchResults_rec(subproof.second);
        }
    }

}

const unsigned short calculateRecursionDepth(const unsigned short basisSize, const unsigned short dimReduction, const unsigned short logDegBound){
    //return biggest depth such that any evaluation is at least 256 bits
    
    const unsigned short depthPoly = floor(float(logDegBound)/float(dimReduction));
    
    const unsigned short minSpaceDim = 2+int(CryptoCommitment::logBytesPerHash - Log2(sizeof(FieldElement)));
    const unsigned short depthSpace = floor(float(basisSize-minSpaceDim)/float(dimReduction));
    
    const unsigned short depth = std::min(depthPoly,depthSpace);

    return depth;
}

const size_t getNumCommitmentPaths(const unsigned short basisSize, const unsigned short fieldExtensionDim, const unsigned short securityLevel){
    return std::ceil(securityLevel/double(fieldExtensionDim - (basisSize+1)));
}

const size_t getNumQueryPaths(const unsigned short basisSize, const unsigned short logDegBound, const unsigned short securityLevel){
    return std::ceil(double(securityLevel) / (basisSize-logDegBound));
}

verifier_t::verifier_t(const vector<FieldElement> evaluationBasis, const unsigned short securityLevel, const unsigned short logDegBound, const bool L0isMSB, specsPrinter& specsCollector):
    evaluationBasis_(evaluationBasis),
    depth_(calculateRecursionDepth(evaluationBasis.size(),common::SoundnessParameters::dimReduction, logDegBound)),
    L0isMSB_(L0isMSB),
    recievedResults_(false)
{
    
    const size_t numQueryPaths = getNumQueryPaths(evaluationBasis_.size(),logDegBound, securityLevel);
    const size_t numComitmentPaths = getNumCommitmentPaths(evaluationBasis_.size(),FFF::Element::ord,securityLevel);
   
    { 
        using std::to_string;

        specsCollector.addLine("field size (|F|)","2^64");
        specsCollector.addLine("RS code dimension","2^" + to_string(logDegBound));
        specsCollector.addLine("RS block-length","2^" + to_string(evaluationBasis.size()));
        specsCollector.addLine("RS code rate","2^-{" + to_string(evaluationBasis.size()-logDegBound) + "}");
        specsCollector.addLine("Soundness error","2^-{" + to_string(securityLevel) + "}");
        specsCollector.addLine("dim L_0 (eta)",to_string(common::SoundnessParameters::dimReduction));
        specsCollector.addLine("recursion depth",to_string(depth_));
        specsCollector.addLine("COMMIT repetitions",to_string(numComitmentPaths));
        specsCollector.addLine("number of tests (ell)",to_string(numQueryPaths));
    }

    TASK("Generating Reed Solomon Proximity proofs queries to reach security level of " +
            std::to_string(securityLevel) +
            " bits, for evaluation over a space of dimension " + 
            std::to_string(evaluationBasis.size()));

    //
    // Construct queries
    //
    {
        // construct commitment paths and link to results
        {
            TASK("Generating " + std::to_string(numComitmentPaths) + " commitment query paths of depth " + std::to_string(depth_));

            RS_results_.resize(numComitmentPaths);
            for(size_t i=0; i< numComitmentPaths; i++){
                addRandomComitmentPath(logDegBound, evaluationBasis_ , const_cast<details::RS_queries&>(queries_), RS_results_[i], depth_, L0isMSB_);
            }
        }

        // constract consistency query paths and link to results
        {
            TASK("Generating " + std::to_string(numQueryPaths) + " consistency paths queries");

            Consistency_path_results_.resize(numQueryPaths);
            for(size_t i=0; i< numQueryPaths; i++){
                addRandomQueryPath(const_cast<details::RS_queries&>(queries_), evaluationBasis_ , Consistency_path_results_[i], L0isMSB_);
            }
        }

        // fetch queries for univariate for external verifier
        {
            for(const auto& q : queries_.univariate){
                const size_t x = q.first;
                auto& currLocationsList = const_cast<queriesToInp_t&>(queriesToInput_)[x];
                currLocationsList.insert(currLocationsList.end(),q.second.begin(),q.second.end());
            }
        }

        // fetch queries for bivariates and store in verifier state
        {
            fill_queries(queries_.proof, state_, evaluationBasis_.size());
        }
    }

    //
    // Construct first layer
    //
    {
        //small hack - this line 'asks' first for the 0 layer (the linear combination) 
        currProofRequests_.push_back( proofRequest_t(&state_,subproofLocation_t(0)));

        //now asks for the next layer
        recieveProofConstructionComitments(vector<hashDigest_t>(1));
    }
}

rawQueries_t verifier_t::getRawQueries()const{
    rawQueries_t result;
    getRawQuery_IOPP(result , state_);
    return result;
}

void verifier_t::digestResults(const rawResults_t& results){
    digestResults_rec(state_,results);
}

void verifier_t::fillResultsAndCommitmentRandomly(){
    fillResultsAndCommitmentsRandomly_rec(state_);
}

void verifier_t::receiveMessage(const TranscriptMessage& msg){
    const proverResponce_t& responce = dynamic_cast<const proverResponce_t&>(msg);
    
    if(!doneConstructingProof()){
        //Still constructing the proof
        recieveProofConstructionComitments(responce.proofConstructionComitments);
    }
    else{
        //Those must be the data queries results
        digestResults(responce.dataResults);
        fetchResults();
        recievedResults_ = true;
    }
}

msg_ptr_t verifier_t::sendMessage(){
    verifierRequest_t* request(new verifierRequest_t());
    
    if(!doneConstructingProof()){
        //Still constructing proof
        request->proofConstructionQueries = getCurrProofConstructionRequest();
    }
    else{
        //The only thing left to do is to query th data
        request->dataQueries = getRawQueries();
    }

    return msg_ptr_t(request);
}

bool verifier_t::verifyComitment()const{
    TASK("Verifying integrity with commitment");
    return verifyComitment_rec(state_); 
}

void verifier_t::fetchResults(){
    fetchResults_rec(state_);
}

bool verifier_t::verifyLowDegree()const{
    bool res = true;
    
    {
        TASK("verifying  Reed-Solomon proximity  proof : " + std::to_string(RS_results_.size()) + " low degree explicit tests");
        bool localRes = true;
        for(const auto& res : RS_results_){
            localRes &= res.verify();
        }
        
        if(localRes == false){
            res =  false; 
        }
    }
    
    return res;
}

bool verifier_t::verifyConsistencyPaths()const{
    bool res = true;

    {
        TASK("Verifying Consistency Paths : " + std::to_string(Consistency_path_results_.size()) + " paths");
        for (const auto& path : Consistency_path_results_){
            for (const auto& pRes : path){
                res &= pRes.verify();
            }
        }
    }
    
    return res;
}

bool verifier_t::verify()const{
    return verifyComitment() & verifyLowDegree() & verifyConsistencyPaths();
}

const queriesToInp_t& verifier_t::queriesToInput()const{
    return queriesToInput_;
}
bool verifier_t::doneInteracting()const{
    return recievedResults_;
}

bool verifier_t::doneConstructingProof()const{
    return currProofRequests_.empty();
}

vector<subproofLocation_t> verifier_t::getCurrProofConstructionRequest()const{
    
    vector<subproofLocation_t> currReq;
    for(const auto& p : currProofRequests_){
        currReq.push_back(p.second);
    }

    return currReq;
}

void verifier_t::recieveProofConstructionComitments(const vector<hashDigest_t>& comitments){
    //
    // Keep the commitments
    //
    
    {
        _COMMON_ASSERT(currProofRequests_.size() == comitments.size(), "Number of commitments not as expected");

        for(size_t i=0; i< currProofRequests_.size(); i++){
            currProofRequests_[i].first->localState.commitment = comitments[i];
        }
    }
    
    //
    // prepare for next layer
    //
    {
        vector<proofRequest_t> nextRequests;
        for(const auto& p : currProofRequests_){
            const auto& currPath = p.second;

            for(auto& subproofNode : p.first->subproofs){
                auto newPath = currPath;
                newPath.push_back(subproofNode.first);
                nextRequests.push_back(proofRequest_t(&subproofNode.second, newPath));
            }
        }

        currProofRequests_ = nextRequests;
    }
}

size_t verifier_t::expectedCommitedProofBytes()const{
    return expectedProofSize(state_);
}

size_t verifier_t::expectedSentProofBytes()const{
    return expectedHashesInResults(state_) * sizeof(hashDigest_t);
}

size_t verifier_t::expectedQueriedDataBytes()const{
    return expectedQueriedFieldElementsNum(state_) * sizeof(FieldElement);
}

} //namespace Verifier
} //nmasepace Fri
} //namespace Protocols
} //namespace libstark
