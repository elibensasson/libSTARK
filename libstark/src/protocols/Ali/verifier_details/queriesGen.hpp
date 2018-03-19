#ifndef ACP_IOP_VERIFIER_QUERIES_GEN_HPP__
#define ACP_IOP_VERIFIER_QUERIES_GEN_HPP__

#include "../common_details/common.hpp"
#include "protocols/common/verifier.hpp"
#include "languages/Acsp/AcspInstance.hpp"
#include <algebraLib/FieldElement.hpp>

#include <vector>

namespace libstark{
namespace Protocols{
namespace Ali{
namespace Verifier{
namespace details{    

//
// Queries to input of inner RS proximity verifier
//

class linearCombinationValue{
public:
    mutable std::vector<Algebra::FieldElement> boundaryEval_res;
    mutable Algebra::FieldElement compositionEval_res;
    mutable Algebra::FieldElement ZK_mask_res;

    void initLocation(const Algebra::FieldElement& x, const unsigned int combId);
    void calculate_witness(const Protocols::Ali::details::randomCoeffsSet_t& coeffs)const;
    void addAnswerPtr(Algebra::FieldElement* answerPtr);
private:
    Algebra::FieldElement x_;
    unsigned int combId_;
    ResultLocation result_;
};

class compositionWithZK_Value{
public:
    mutable std::vector<std::vector<Algebra::FieldElement>> boundaryPoly_res;
    mutable Algebra::FieldElement ZK_mask_res;
    
    void init(const AcspInstance& instance, const Algebra::FieldElement& consistencyPoint, const unsigned int combId);
    void calculate(const Protocols::Ali::details::randomCoeffsSet_t& coeffs)const;
    void addAnswerPtr(Algebra::FieldElement* answerPtr);

private:
    const AcspInstance* instance_;
    Algebra::FieldElement consistencyPoint_;
    unsigned int combId_;
    std::vector<std::vector<Algebra::FieldElement>> boundaryVanishingVals_;
    std::vector<std::vector<Algebra::FieldElement>> boundaryPolyVals_;
    Algebra::FieldElement vanishingSpacePolyVal_;
    ResultLocation result_;

    //methods
    Algebra::FieldElement calculateCompositionValue()const;
};

} // namespace details
} // namespace Verifier
} // namespace Ali
} // namespace Protocols
} // namespace libstark

#endif // ACP_IOP_VERIFIER_QUERIES_GEN_HPP__
