#ifndef RS_IOPP_BIASED_VERIFIER_HPP__
#define RS_IOPP_BIASED_VERIFIER_HPP__

#include "verifier_details/queryGenerator.hpp"
#include "common/common.hpp"
#include "protocols/common/verifier.hpp"
#include "common/Utils/specsPrint.hpp"

namespace libstark{
namespace Protocols{
namespace Fri{
namespace Verifier{

class verifier_t : public IOPP_verifierInterface{
public:
    verifier_t(const std::vector<Algebra::FieldElement> evaluationBasis, const unsigned short securityLevel, const unsigned short logDegBound, const bool L0isMSB, specsPrinter& specsCollector);
    void receiveMessage(const TranscriptMessage& msg);
    msg_ptr_t sendMessage(); 
    bool doneInteracting()const;
    const queriesToInp_t& queriesToInput()const;
    bool verify()const;
    
    size_t expectedCommitedProofBytes()const;
    size_t expectedSentProofBytes()const;
    size_t expectedQueriedDataBytes()const;
    
    void fillResultsAndCommitmentRandomly();

private:
    const std::vector<Algebra::FieldElement> evaluationBasis_;
    const queriesToInp_t queriesToInput_;
    const size_t depth_;
    const bool L0isMSB_;
    
    const details::RS_queries queries_;
    std::vector<details::RS_result> RS_results_;
    std::vector<details::Consistency_path_t> Consistency_path_results_;
    
    bool recievedResults_;
    
    typedef std::pair<common::state_t<uniEvalView_t>*, common::subproofLocation_t> proofRequest_t;
    std::vector<proofRequest_t> currProofRequests_;
    common::state_t<uniEvalView_t> state_;
    
    //methods
    std::vector<common::subproofLocation_t> getCurrProofConstructionRequest()const;
    void recieveProofConstructionComitments(const std::vector<CryptoCommitment::hashDigest_t>& comitments);
    bool doneConstructingProof()const;
    common::rawQueries_t getRawQueries()const;
    void digestResults(const common::rawResults_t& results);
    void fetchResults();
    bool verifyComitment()const;
    bool verifyLowDegree()const;
    bool verifyConsistencyPaths()const;
};

} //namespace Verifier
} //nmasepace RS_IOPP
} //namespace Protocols
} //namespace libstark

#endif //#ifndef RS_IOPP_BIASED_VERIFIER_HPP__
