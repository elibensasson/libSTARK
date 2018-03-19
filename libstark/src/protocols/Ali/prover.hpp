#ifndef Ali_PROVER_HPP__
#define Ali_PROVER_HPP__

#include "protocols/protocol.hpp"
#include "common_details/common.hpp"

#include "languages/Acsp/AcspInstance.hpp"
#include "languages/Acsp/AcspWitness.hpp"

#include "languages/Bair/BairInstance.hpp"

#include "protocols/common/proofs.hpp"
#include "protocols/Fri/prover.hpp"
#include "common/Utils/TaskReporting.hpp"

#include <algebraLib/novelFFT.hpp>
#include <functional>

namespace libstark{
namespace Protocols{
namespace Ali{
namespace Prover{

typedef std::function<std::unique_ptr<PartieInterface>(const std::vector<Algebra::FieldElement>& evaluationBasis, std::vector<Algebra::FieldElement>&& evaluation, const bool L0isMSB)> RS_proverFactory_t;

const RS_proverFactory_t Biased_prover = [](const std::vector<Algebra::FieldElement>& evaluationBasis, std::vector<Algebra::FieldElement>&& evaluation, const bool L0isMSB){
    TASK("Generating prover for RS proximity the 'BIASED' version");

    return std::unique_ptr<PartieInterface>(new Protocols::Fri::Prover::prover_t(evaluationBasis, std::move(evaluation), L0isMSB));
};


typedef Ali::details::partyState<std::unique_ptr<dataWithCommitment>> uniEvalsSet_t;

class prover_t : public PartieInterface{
public:
    prover_t(const BairInstance& bairInstance, const AcspWitness& witness, const RS_proverFactory_t& RS_proverFactory);
    void receiveMessage(const TranscriptMessage& msg);
    msg_ptr_t sendMessage(); 

private:
    const BairInstance& bairInstance_;
    std::vector<std::unique_ptr<AcspInstance>> instance_;
    const AcspWitness& witness_;
    const RS_proverFactory_t RS_proverFactory_;
    std::unique_ptr<Algebra::novelFFT> fftInstance_;
    unsigned int numRS_repetitions_;
    
    uniEvalsSet_t state_;
    bool entireWitnessKept_;
    std::vector<std::unique_ptr<PartieInterface>> RS_prover_witness_;
    std::vector<std::unique_ptr<PartieInterface>> RS_prover_composition_;
    Ali::details::phase_t phase_;
    Ali::details::rawResults_t nextResults_;

    //
    // methods
    //
    void evaluateBoundryPolys(const unsigned int numRepetitions);
    void evaluateZK_Witness_mask();
    void evaluateZK_Composition_mask();
    std::vector<Algebra::FieldElement> computeUnivariateForRS_Proximity_Witness(const Ali::details::randomCoeffsSet_t& randCoeffs, const unsigned int rsCombId)const;
    std::vector<Algebra::FieldElement> computeUnivariateForRS_Proximity_Composition(const Ali::details::randomCoeffsSet_t& randCoeffs, const unsigned int rsCombId)const;
    Ali::details::rawResults_t answerQueries(const Ali::details::rawQueries_t& queries);
};


} // namespace Prover
} // namespace Ali
} // namespace Protocols
} // namespace libstark

#endif //#ifndef Ali_PROVER_HPP__
