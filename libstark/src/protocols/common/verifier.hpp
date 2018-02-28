#ifndef PROTOCOLS_COMMON_VERIFIER_HPP__
#define PROTOCOLS_COMMON_VERIFIER_HPP__

#include "protocols/protocol.hpp"
#include "protocols/common/queries.hpp"
#include "CryptoCommitment/MerkleCommitment.hpp"
#include <algebraLib/FieldElement.hpp>
#include <map>
#include <vector>
#include <functional>

namespace libstark{
namespace Protocols{

typedef std::function<void(const Algebra::FieldElement&)> setVal_t;
// (blockIdx, offsetInBlock) -> locations to keep results
typedef std::map<size_t , std::map<short, std::vector<setVal_t> > > expliciteQueries_t;

class uniEvalView_t{
public:
    
    uniEvalView_t(const short logSizeBytes_);
    uniEvalView_t();
    CryptoCommitment::SparceMerkleTree view;
    CryptoCommitment::hashDigest_t commitment;
    
    expliciteQueries_t queries;

    void fetchResults()const;
    rawQuery_t getRawQuery()const;
    void fillResultsAndCommitmentRandomly();
    void digestResults(const rawResult_t& results);
    bool verifyComitment()const;
    size_t expectedResultsLenght()const;
    size_t expectedQueriedFieldElementsNum()const;
    short getLogSizeBytes()const{return logSizeBytes;}

private:
    short logSizeBytes;
};

class ResultLocation{
public:
    void addAnswerPtr(Algebra::FieldElement* ptr);
    void answer(const Algebra::FieldElement& res)const;

private:
    std::vector<Algebra::FieldElement*> answerLocations_;
};

} //namespace Protocols
} //namespace libstark

#endif //#ifndef PROTOCOLS_COMMON_VERIFIER_HPP__
