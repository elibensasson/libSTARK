#include "prover.hpp"
#include "common/Utils/TaskReporting.hpp"
#include "algebraLib/SubspacePolynomial.hpp"

namespace libstark{
namespace Protocols{
namespace Fri{
namespace Prover{

using common::subproofLocation_t;
using common::state_t;
using common::rawQueries_t;
using common::rawResults_t;
using common::getL0Basis;
using common::getL1Basis;
using common::getBasisLIndex_byL0L1indices;
using common::getColumnBasis;
using common::verifierRequest_t;
using common::proverResponce_t;
using CryptoCommitment::hashDigest_t;
using CryptoCommitment::SparceMerkleTree;
using Infrastructure::Log2;
using Infrastructure::POW2;
using Algebra::PolynomialDegree;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::zero;
using Algebra::FieldElement;
using Algebra::getSpaceElementByIndex;
using std::unique_ptr;
using std::vector;
using std::max;
using std::ceil;

namespace{

struct queryInfo_t{
    const dataWithCommitment* proof;
    const rawQuery_t* rawQueries;
    vector<hashDigest_t>* results;
    size_t depth;

    queryInfo_t(
            const dataWithCommitment& proof_,
            const rawQuery_t& rawQueries_,
            vector<hashDigest_t>& results_,
            const size_t depth_):
            proof(&proof_),
            rawQueries(&rawQueries_),
            results(&results_),
            depth(depth_){};
};

vector<queryInfo_t> serializeQueries(
    const state_t<unique_ptr<dataWithCommitment>>& currState,
    const rawQueries_t& rawQueries,
    state_t<vector<hashDigest_t>>& rawResults,
    const size_t depth){

    vector<queryInfo_t> SerialQueries;

    //
    // Serialize local queries
    //
    {
        if(!rawQueries.localState.empty()){
            SerialQueries.push_back(queryInfo_t(*currState.localState, rawQueries.localState, rawResults.localState, depth));
        }
    }

    //
    // Serialize subproof queries
    //
    for(const auto& q : rawQueries.subproofs){
        const auto subproofQueries = 
        serializeQueries(currState.subproofs.at(q.first), rawQueries.subproofs.at(q.first), rawResults.subproofs[q.first], depth+1);
    
        SerialQueries.insert(SerialQueries.end(),subproofQueries.begin(),subproofQueries.end());
    }

    return SerialQueries;
}

hashDigest_t addSubproof(
    state_t<unique_ptr<dataWithCommitment>>& currState,
    const vector<FieldElement>& evaluationBasis,
    const subproofLocation_t& pathToProof,
    const subproofLocation_t& pathFromRoot,
    const bool L0isMSB
    ){
   
    //
    // Base case of recursion
    //
    if(pathToProof.size() == 0){
        return currState.localState->getCommitment();
    }
    
    //
    // Updating paths
    //
    const auto& currWay = pathToProof[0];
    const subproofLocation_t nextPathToProof(pathToProof.begin()+1, pathToProof.end());
    subproofLocation_t nextPathFromRoot(1,currWay);
    nextPathFromRoot.insert(nextPathFromRoot.end(),pathFromRoot.begin(),pathFromRoot.end());

    //
    // Basis of next evaluation
    //
    const vector<FieldElement> basisForColumnsProof(getColumnBasis(evaluationBasis,L0isMSB));

    //
    // Check if current univariate already evaluated, and evaluate if needed
    //
    if(currState.subproofs.count(currWay) == 0){
        const vector<FieldElement> BasisL0 = getL0Basis(evaluationBasis,L0isMSB);
        const vector<FieldElement> BasisL1 = getL1Basis(evaluationBasis,L0isMSB);
        
        const unsigned short logSigmentLen = getL0Basis(basisForColumnsProof,L0isMSB).size();
        const unsigned short logNumSigments = getL1Basis(basisForColumnsProof,L0isMSB).size(); 
        const unsigned short logSigmentsInBlock = std::min((unsigned short)10,logNumSigments);
        
        const size_t sigmentLen = POW2(logSigmentLen);
        const size_t sigmentsInBlock = POW2(logSigmentsInBlock);

        ///
        /// The following is a trick for faster evaluation
        ///
        /// We have : values of a polynomial over a space L_0
        /// We want : the polynomials value over another point x not in L_0
        ///
        /// the Lagrange polynomial for \alpha \in L_0 is:
        /// 
        /// l_\alpha (x) =
        /// \frac{ \prod_{\beta \ne \alpha \in L_0} (x - \beta) }{ \prod_{\beta \ne \alpha \in L_0} (\alpha - \beta) } =
        /// \frac{ Z_{L_0}(x) }{ (x-\alpha) \cdot \prod_{\beta \ne \alpha \in L_0} (\alpha - \beta) }
        ///
        /// We Define:
        ///
        /// C_\alpha := \prod_{\beta \ne \alpha \in L_0} (\alpha - \beta)
        ///
        /// Thus, given values p(\alpha) for any \alpha in L_0, the value over $x$ is:
        ///
        /// p(x) =
        /// \sum_{\alpha \in L_0} p(\alpha) \cdot l_\alpha (x) =
        /// Z_{L_0} (x) \cdot \sum_{\alpha \in L_0} \frac{ p(\alpha) }{(x-\alpha) \cdot C_\alpha}
        /// 
        /// In this formula many factors are independent of $x$ and can be precomputed, and this is what used bellow
        ///

        // global auxiliary values        
        const size_t L0_size = POW2(BasisL0.size());
        vector<FieldElement> spaceElements(L0_size);
        for(unsigned int i=0; i<L0_size; i++){
            spaceElements[i] = getSpaceElementByIndex(BasisL0,Algebra::zero(),i);
        }


        //compute Z_{L_0}
        const Algebra::SubspacePolynomial Z_L0(Algebra::elementsSet_t(BasisL0.begin(),BasisL0.end()));

        //compute C_\alpha vector
        vector<FieldElement> C_alpha(L0_size,Algebra::one());
        {

            for(unsigned int i=0; i<L0_size; i++){
                const FieldElement& alpha = spaceElements[i];
                for(unsigned int j=0; j<L0_size; j++){
                    if(i==j)continue;
                    const FieldElement& beta = spaceElements[j];
                    C_alpha[i] *= (alpha - beta);
                }
            }
        }

        
        const auto sigmentConstructor = [&](const size_t sigmentsBlockIdx, FieldElement* res){
            
            vector<FieldElement> vecToInveresePointwise(sigmentsInBlock*sigmentLen*L0_size);

            for(unsigned int localSigmentIdx = 0; localSigmentIdx < sigmentsInBlock; localSigmentIdx++){  
                const size_t sigmentIdx = sigmentsBlockIdx*sigmentsInBlock + localSigmentIdx;

                for(unsigned int i=0; i< sigmentLen; i++){
                    const size_t globalIndex = sigmentIdx * sigmentLen + i;
                    const FieldElement currOffset = getSpaceElementByIndex(BasisL1,zero(),globalIndex);
                    
                    for(size_t j=0; j< L0_size; j++){
                        const FieldElement alpha = spaceElements[j];
                        const size_t elementIndex = localSigmentIdx*sigmentLen*L0_size + i*L0_size + j;
                        vecToInveresePointwise[elementIndex] = ((currWay+currOffset)-alpha)*C_alpha[j];
                    }
                }
            }

            const vector<FieldElement> denuminators = Algebra::invertPointwise(vecToInveresePointwise);

            for(unsigned int localSigmentIdx = 0; localSigmentIdx < sigmentsInBlock; localSigmentIdx++){  
                const size_t sigmentIdx = sigmentsBlockIdx*sigmentsInBlock + localSigmentIdx;
                FieldElement* currSigRes = res + localSigmentIdx*sigmentLen;

                for(unsigned int i=0; i< sigmentLen; i++){
                    const size_t globalIndex = sigmentIdx * sigmentLen + i;
                    const FieldElement currOffset = getSpaceElementByIndex(BasisL1,zero(),globalIndex);
                    
                    currSigRes[i] = Algebra::zero();
                    for(size_t j=0; j< L0_size; j++){
                        const size_t currElemIdx = getBasisLIndex_byL0L1indices(evaluationBasis,j,globalIndex,L0isMSB);
                        const FieldElement alpha = spaceElements[j];
                        const FieldElement currVal = currState.localState->getElement(currElemIdx);
                        
                        const size_t elementIndex = localSigmentIdx*sigmentLen*L0_size + i*L0_size + j;
                        currSigRes[i] += currVal * denuminators[elementIndex];
                    }
                    currSigRes[i] *= Z_L0.eval(currWay+currOffset);
                }
            }
        };

        currState.subproofs[currWay].localState = 
            unique_ptr<dataWithCommitment>(
                new dataWithCommitment(
                        logSigmentLen + logSigmentsInBlock,
                        logNumSigments - logSigmentsInBlock,
                        sigmentConstructor
                    )
                );
    }

    
    //
    // Continue recursively
    //
    return addSubproof(currState.subproofs[currWay], basisForColumnsProof, nextPathToProof, nextPathFromRoot,L0isMSB);
    }
}

prover_t::prover_t(const std::vector<FieldElement>& evaluationBasis, std::vector<FieldElement>&& evaluation, const bool L0isMSB):
    evaluationBasis_(evaluationBasis),
    L0isMSB_(L0isMSB)
    {

    //
    //construct the root evaluation
    //
    {
        TASK("Constructing proof of proximity root");
        state_.localState = unique_ptr<dataWithCommitment>(new dataWithCommitment(std::move(evaluation), getL0Basis(evaluationBasis,L0isMSB_).size()));
    }
}

void prover_t::receiveMessage(const TranscriptMessage& msg){
    const verifierRequest_t& vMsgCast = dynamic_cast<const verifierRequest_t&>(msg);

    //retrieve commitments
    {
        nextResponce_.proofConstructionComitments = constructProofs(vMsgCast.proofConstructionQueries);
    }
    //retrieve data
    {
        nextResponce_.dataResults = responceToDataQueries(vMsgCast.dataQueries);
    }
}

msg_ptr_t prover_t::sendMessage(){
    return msg_ptr_t(new proverResponce_t(nextResponce_));
}


void prover_t::deleteProof(){
#if __GNUG__
    state_ = common::state_t<std::unique_ptr<dataWithCommitment>>();
#endif
}

vector<hashDigest_t> prover_t::constructProofs(const vector<subproofLocation_t>& proofsList){

    //
    // Start by sorting requests by layers
    //
    map<size_t,vector<size_t>> requestsByLayer;
    {
        for(unsigned int i=0; i < proofsList.size(); i++){
            const size_t currPathLenght = proofsList[i].size();
            requestsByLayer[currPathLenght].push_back(i);
        }
    }

    //initialize the results vector
    vector<hashDigest_t> result(proofsList.size());

    //
    // First commitment for root, if was requested
    //
    {
        if(requestsByLayer.count(0) > 0){
            const auto rootCommitment = state_.localState->getCommitment();
            for(const size_t resIdx : requestsByLayer.at(0)){
                result[resIdx] = rootCommitment;
            }
        }
    }

    //
    // Now continue to the rest
    //
    for(const auto& layer : requestsByLayer){
        if(layer.first == 0){
            continue;
        }
        
        TASK("Constructing layer #" + std::to_string(layer.first) + " of proof of proximity");
        
        for(unsigned int i=0; i < layer.second.size(); i++){
            const auto& reqIdx = layer.second[i];
            result[reqIdx] = addSubproof(state_, evaluationBasis_, proofsList[reqIdx], subproofLocation_t(), L0isMSB_);
        }
    }

    return result;
}

rawResults_t prover_t::responceToDataQueries(const rawQueries_t& queries)const{
    TASK("Answering data queries from proof of proximity");
    
    rawResults_t results;
   
    { 
        auto serialQueries = serializeQueries(state_, queries, results, 0);

#pragma omp parallel for
        for(ploopinttype i=0; i<serialQueries.size(); i++){
            auto currQ = serialQueries[i];
            
            const SparceMerkleTree currTree = currQ.proof->answerQueries(*currQ.rawQueries);
            *currQ.results = currTree.toVector();
        }
    }
   
    return results;
}


} //namespace Prover
} //nmasepace Fri
} //namespace Protocols
} //namespace libstark
