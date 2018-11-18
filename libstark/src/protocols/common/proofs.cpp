#include "proofs.hpp"
#include "common/Utils/TaskReporting.hpp"
#include "common/Utils/ErrorHandling.hpp"

namespace libstark{
namespace Protocols{

using CryptoCommitment::hashDigest_t;
using CryptoCommitment::path_t;
using CryptoCommitment::constructMerkleTree;
using CryptoCommitment::logBytesPerHash;
using Infrastructure::Log2;
using Infrastructure::POW2;
using Algebra::FieldElement;
using Algebra::UnivariatePolynomialInterface;
using Algebra::zero;
using std::vector;

MerkleTree::MerkleTree(FieldElement const*const src, short src_log_size): logSizeBytes_(src_log_size + Log2(sizeof(FieldElement))){
    
    tree_ = new hashDigest_t[POW2(logSizeBytes_ - Log2(sizeof(CryptoCommitment::hashDigest_t)))];
    root_ = constructMerkleTree(src,logSizeBytes_,tree_);
}

MerkleTree::MerkleTree(const vector<hashDigest_t>& src): logSizeBytes_(Log2(src.size()) + Log2(sizeof(CryptoCommitment::hashDigest_t))){
    tree_ = new hashDigest_t[src.size()];
    root_ = constructMerkleTree(&src[0],logSizeBytes_,tree_);
}

MerkleTree::MerkleTree(short src_log_size): logSizeBytes_(src_log_size+Log2(sizeof(FieldElement))){
    
    tree_ = new hashDigest_t[POW2(logSizeBytes_ - Log2(sizeof(CryptoCommitment::hashDigest_t)))];
}

void MerkleTree::constructSubtree(FieldElement const*const src, const size_t sigment_logLen, const size_t sigment_index){
    constructMerkleSubTree(src,logSizeBytes_,sigment_logLen + Log2(sizeof(FieldElement)),sigment_index,tree_);
}

void MerkleTree::finishTreeAfterSegments(const size_t sigment_logLen){
    short currSrcLogLen = logSizeBytes_ - (sigment_logLen + Log2(sizeof(FieldElement)));
    if(currSrcLogLen == 0){
        root_ = tree_[1];
        return;
    }
    root_ = constructMerkleTree(tree_ + POW2(currSrcLogLen), currSrcLogLen + logBytesPerHash, tree_);
}

const hashDigest_t& MerkleTree::getRoot()const{
    return root_;
}

path_t MerkleTree::getPathToBlock(const size_t blockIndex)const{
    return CryptoCommitment::getPathToBlock(tree_,logSizeBytes_,blockIndex);
}

bool MerkleTree::verifyPathToBlock(const path_t& path, FieldElement const*const blockData, const size_t blockIndex)const{
    return CryptoCommitment::verifyPathToBlock(blockData, root_, path, blockIndex);
}

short MerkleTree::logSizeBytes()const{
    return logSizeBytes_;
}

MerkleTree::~MerkleTree(){
    delete[] tree_;
}

namespace{
    
    void getFinalHash(
        const dataWithCommitment::sigmentConstructor_t& sigmentConstructor,
        const unsigned short logNumSigments,
        const unsigned short logSigmentLength,
        vector<FieldElement>& data,
        MerkleTree& commitment,
        const bool multyThreading){
        
        const size_t numSigments = POW2(logNumSigments);
        const size_t sigmentLength = POW2(logSigmentLength);

        //
        //Calculate buffers
        //
        if(multyThreading){
#pragma omp parallel for
            for(plooplongtype currSigmentIdx = 0; currSigmentIdx < numSigments; currSigmentIdx++){
                sigmentConstructor(currSigmentIdx,&data[currSigmentIdx*sigmentLength]);
                commitment.constructSubtree(&data[0], logSigmentLength, currSigmentIdx);
            }
        }
        else{
            for(unsigned long long currSigmentIdx = 0; currSigmentIdx < numSigments; currSigmentIdx++){
                TASK("Constructing 1 sigment of total " + std::to_string(numSigments));
                
                {
                    TASK("Constructing Data sigment");
                    sigmentConstructor(currSigmentIdx,&data[currSigmentIdx*sigmentLength]);
                }
                {
                    TASK("Constructing Merkle tree");
                    commitment.constructSubtree(&data[0], logSigmentLength, currSigmentIdx);
                }
            }
        }

        commitment.finishTreeAfterSegments(logSigmentLength);
    }

    void getUniPolySigment(const UnivariatePolynomialInterface& srcPoly,
            const vector<FieldElement>& evaluationBasis, const FieldElement& evaluationShift,
            const unsigned short degBoundLog, const size_t sigmentIdx,
            FieldElement* res){
        //
        //Initialize global data relevant to fractions of space for queries
        //
        const size_t buffDim = degBoundLog;
        const vector<FieldElement> buffEvalBasis(evaluationBasis.begin(), evaluationBasis.begin()+buffDim);
        const vector<FieldElement> buffOffsetBasis(evaluationBasis.begin()+buffDim, evaluationBasis.end());
        
        //
        // Local info for current segment
        //

        const FieldElement currOffset = Algebra::getSpaceElementByIndex(buffOffsetBasis,evaluationShift,sigmentIdx);
        //TO DO : direct write to res
        const auto sigRes = srcPoly.eval(buffEvalBasis,currOffset);
        memcpy(res,&sigRes[0],sigRes.size()*sizeof(FieldElement));
    }
}
 
UniEvalWithCommitment::UniEvalWithCommitment(const UnivariatePolynomialInterface& srcPoly,
const vector<FieldElement>& evaluationBasis, const FieldElement& evaluationShift,
const unsigned short degBoundLog):
    dataWithCommitment(
        degBoundLog,
        evaluationBasis.size() - degBoundLog,
        [&](const size_t sigmentIdx, FieldElement* res){
                getUniPolySigment(srcPoly,evaluationBasis,evaluationShift,degBoundLog,sigmentIdx,res);
                }),
    poly_(srcPoly){};

const UnivariatePolynomialInterface& UniEvalWithCommitment::poly()const{return poly_;}


dataWithCommitment::dataWithCommitment(
    const unsigned short logSigmentLength,
    const unsigned short logNumSigments,
    const sigmentConstructor_t& sigmentConstructor,
    const bool multyThreading):
                                logSigmentLength_(logSigmentLength),
                                logNumSigments_(logNumSigments),
                                data_(POW2(logNumSigments+logSigmentLength)),
                                commitment_(logNumSigments+logSigmentLength){
                                    getFinalHash(sigmentConstructor, logNumSigments, logSigmentLength,data_,commitment_,multyThreading);
                                }

dataWithCommitment::dataWithCommitment(const vector<FieldElement>&& data ,const unsigned short logSigmentLenght):
                                logSigmentLength_(logSigmentLenght),
                                logNumSigments_(Log2(data.size())-logSigmentLenght),
                                data_(data),
                                commitment_(&data_[0],Log2(data_.size())){};
    
dataWithCommitment::~dataWithCommitment(){ }

vector<FieldElement> dataWithCommitment::getSigment(size_t sigmentId)const{
    const size_t sigBegin = sigmentId*POW2(logSigmentLength_);
    const size_t sigEnd = sigBegin + POW2(logSigmentLength_);
    vector<FieldElement> result(data_.begin() + sigBegin, data_.begin() + sigEnd);

    return result;
}

const FieldElement& dataWithCommitment::getElement(const size_t index)const{
    _COMMON_ASSERT(index < data_.size(),"Index out of range");
    return data_[index];
}

CryptoCommitment::SparceMerkleTree dataWithCommitment::answerQueries(const rawQuery_t& queries)const{

    CryptoCommitment::SparceMerkleTree resultsTree(logSigmentLength_ + logNumSigments_ + Log2(sizeof(FieldElement)));

    for(const size_t& blockPairIndex : queries){
        const size_t blockIndex = blockPairIndex<<1;
        const size_t FElemIndex = CryptoCommitment::getElementIndex(blockIndex); 

        std::array<hashDigest_t,2> data;
        {
            FieldElement* dataAsFE((FieldElement*)&data[0]);
            for(size_t i=0; i< CryptoCommitment::getDualBlockSize(); i++){
                dataAsFE[i] = data_[FElemIndex + i];
            }
        }

        const auto currPath =  commitment_.getPathToBlock(CryptoCommitment::getBlockIndex(FElemIndex));

#pragma omp critical
        { 
            resultsTree.addPath(data, currPath, blockPairIndex);
        }
    }

    return resultsTree;
}

CryptoCommitment::hashDigest_t dataWithCommitment::getCommitment()const{
    return commitment_.getRoot();
}

} //namespace Protocols
} //namespace libstark

