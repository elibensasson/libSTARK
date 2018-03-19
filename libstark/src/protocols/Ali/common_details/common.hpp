#ifndef __PARTYSTATE_HPP__
#define __PARTYSTATE_HPP__

#include "protocols/common/CryptoCommitment/MerkleCommitment.hpp"
#include "protocols/protocol.hpp"
#include "protocols/common/queries.hpp"
#include "languages/Acsp/AcspInstance.hpp"
#include <algebraLib/FieldElement.hpp>
#include <algebraLib/PolynomialDegree.hpp>
#include <vector>

namespace libstark{
namespace Protocols{
namespace Ali{
namespace details{

template<typename T_univariate>
class partyState{
public:
    virtual ~partyState(){};
    std::vector<T_univariate> boundary;
    T_univariate boundaryPolysMatrix;
    T_univariate ZK_mask_boundary;
    std::vector<T_univariate> ZK_mask_composition;
};

struct randomCoeefs{
    size_t degShift;
    std::vector<Algebra::FieldElement> coeffUnshifted;
    std::vector<Algebra::FieldElement> coeffShifted;
    //multiplyes the polynomial by the polynomial:
    // <coeffUnshifted> + <coeffShifted>*x^<degShift>

    randomCoeefs():degShift(0){};
};

typedef partyState<randomCoeefs> randomCoeffsSet_t;
typedef partyState<rawQuery_t> rawQueries_t;
typedef partyState<std::vector<CryptoCommitment::hashDigest_t>> rawResults_t;

enum phase_t{START_PROTOCOL, UNIVARIATE_COMMITMENTS, VERIFIER_RANDOMNESS, RS_PROXIMITY, QUERY, RESULTS, DONE};

phase_t advancePhase(const phase_t& currPhase);

class verifierMsg : public TranscriptMessage{
public:
    virtual ~verifierMsg(){};
    unsigned int numRepetitions;
    randomCoeffsSet_t randomCoefficients;
    std::vector<std::vector<Algebra::FieldElement>> coeffsPi;
    std::vector<std::vector<Algebra::FieldElement>> coeffsChi;
    Ali::details::rawQueries_t queries;
    std::vector<std::unique_ptr<TranscriptMessage>> RS_verifier_witness_msg;
    std::vector<std::unique_ptr<TranscriptMessage>> RS_verifier_composition_msg;
};

class proverMsg : public TranscriptMessage{
public:
    virtual ~proverMsg(){};
    std::vector<CryptoCommitment::hashDigest_t> commitments;
    Ali::details::rawResults_t results;
    std::vector<std::unique_ptr<TranscriptMessage>> RS_prover_witness_msg;
    std::vector<std::unique_ptr<TranscriptMessage>> RS_prover_composition_msg;
};

namespace PCP_common {

Algebra::PolynomialDegree composition_div_ZH_degreeBound(const AcspInstance& src);

std::vector<Algebra::PolynomialDegree> witness_div_Z_Boundery_degreeBound(const AcspInstance& src);

/**
 * Returns the basis over which the low degree tests are done
 * (aka RS PCPP)
 */
typedef std::vector<Algebra::FieldElement> basis_t;
struct basisWithShift_t{
    basis_t basis;
    Algebra::FieldElement shift;

    basisWithShift_t(const basis_t& basis_, const Algebra::FieldElement& shift_): basis(basis_),shift(shift_){};
    basisWithShift_t(const basis_t& basis_): basis(basis_),shift(Algebra::zero()){};
    basisWithShift_t(): basis(0),shift(Algebra::zero()){};
};
basisWithShift_t basisForWitness(const AcspInstance& src);

Algebra::PolynomialDegree maximalPolyDegSupported_Witness(const AcspInstance& src);

/**
 * Returns the basis over which the consistency of the witness and the composition
 * polynomial is proved
 */
basisWithShift_t basisForConsistency(const AcspInstance& src);

unsigned short boundaryPolysMatrix_logWidth(const AcspInstance& src, const unsigned int numZkMasks);
unsigned short boundaryPolysMatrix_logNumElements(const AcspInstance& src, const unsigned int numZkMasks);

} // namespace PCP_common

namespace SoundnessParameters{

    const unsigned short Eta = 3;
    const unsigned int FieldExtensionDegree = 64;
    const unsigned int maxSoundnessPerComb = FieldExtensionDegree-1;

} // namespace SoundnessParameters

} // namespace details
} // namespace Ali
} // namespace Protocols
} // namespace libstark

#endif //#ifndef __PARTYSTATE_HPP__
