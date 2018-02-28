#ifndef RS_IOPP_BIASED_PROVER_HPP__
#define RS_IOPP_BIASED_PROVER_HPP__

#include "common/common.hpp"
#include "protocols/common/proofs.hpp"

#include <algebraLib/PolynomialInterface.hpp>

namespace libstark{
namespace Protocols{
namespace Fri{
namespace Prover{

class prover_t : public PartieInterface{
public:
    prover_t(const std::vector<Algebra::FieldElement>& evaluationBasis, std::vector<Algebra::FieldElement>&& evaluation, const bool L0isMSB);
    void receiveMessage(const TranscriptMessage& msg);
    msg_ptr_t sendMessage();
    void deleteProof();
private:
    const std::vector<Algebra::FieldElement> evaluationBasis_;
    common::state_t<std::unique_ptr<dataWithCommitment>> state_;
    common::proverResponce_t nextResponce_;
    const bool L0isMSB_;
   
    //methods
    std::vector<CryptoCommitment::hashDigest_t> constructProofs(const std::vector<common::subproofLocation_t>& proofsList);
    common::rawResults_t responceToDataQueries(const common::rawQueries_t& queries)const;
};

} //namespace Prover
} //nmasepace Fri
} //namespace Protocols
} //namespace libstark

#endif // #ifndef RS_IOPP_BIASED_PROVER_HPP__
