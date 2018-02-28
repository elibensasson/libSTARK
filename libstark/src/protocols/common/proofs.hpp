#ifndef PROTOCOLS_COMMON_PROOFS_HPP__
#define PROTOCOLS_COMMON_PROOFS_HPP__

#include "CryptoCommitment/MerkleCommitment.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "queries.hpp"
#include <algebraLib/FieldElement.hpp>
#include <algebraLib/UnivariatePolynomialGeneral.hpp>
#include <vector>
#include <functional>

namespace libstark{
namespace Protocols{

class MerkleTreeInterface{
public:
    virtual const CryptoCommitment::hashDigest_t& getRoot()const = 0;
    virtual CryptoCommitment::path_t getPathToBlock(const size_t blockIndex)const = 0;
    virtual short logSizeBytes()const = 0;
};

class MerkleTree : public MerkleTreeInterface{
public:
    MerkleTree(Algebra::FieldElement const*const src, short src_log_size);
    MerkleTree(const std::vector<CryptoCommitment::hashDigest_t>& src);
    MerkleTree(short src_log_size);
    void constructSubtree(Algebra::FieldElement const*const src, const size_t sigment_logLen, const size_t sigment_index);
    void finishTreeAfterSegments(const size_t sigment_logLen);
    const CryptoCommitment::hashDigest_t& getRoot()const;
    CryptoCommitment::path_t getPathToBlock(const size_t blockIndex)const;
    bool verifyPathToBlock(const CryptoCommitment::path_t& path, Algebra::FieldElement const*const blockData, const size_t blockIndex)const;
    ~MerkleTree();
    CryptoCommitment::hashDigest_t* tree_;
    short logSizeBytes()const;
private:
    short logSizeBytes_;
    CryptoCommitment::hashDigest_t root_;
};

class dataWithCommitment{
public:
    
    dataWithCommitment(const std::vector<Algebra::FieldElement>&& data, const unsigned short logSigmentLenght);
    
    typedef std::function<void(const size_t, Algebra::FieldElement*)> sigmentConstructor_t;
    dataWithCommitment(const unsigned short logSigmentLength, const unsigned short logNumSigments, const sigmentConstructor_t& sigmentConstructor, const bool multyThreading = true);

    virtual ~dataWithCommitment();
    
    CryptoCommitment::hashDigest_t getCommitment()const;
    std::vector<Algebra::FieldElement> getSigment(size_t sigmentId)const;
    const Algebra::FieldElement& getElement(const size_t index)const;
    CryptoCommitment::SparceMerkleTree answerQueries(const rawQuery_t& queries)const;
    const MerkleTree& getMerkleTree()const{return commitment_;}

private:
    const unsigned short logSigmentLength_;
    const unsigned short logNumSigments_;
    std::vector<Algebra::FieldElement> data_;
    MerkleTree commitment_;
};

class UniEvalWithCommitment : public dataWithCommitment{
public:
    
    UniEvalWithCommitment(const Algebra::UnivariatePolynomialInterface& srcPoly,
    const std::vector<Algebra::FieldElement>& evaluationBasis,
    const Algebra::FieldElement& evaluationShift,
    const unsigned short degBoundLog);

    const Algebra::UnivariatePolynomialInterface& poly()const;

private:
    const Algebra::UnivariatePolynomialGeneral poly_;
};

} //namespace Protocols
} //namespace libstark

#endif // #ifndef PROTOCOLS_COMMON_PROOFS_HPP__
