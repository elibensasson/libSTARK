#include "MerkleCommitment.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/ErrorHandling.hpp"
#include <omp.h>
#include <string>
#include <algebraLib/FieldElement.hpp>
#include <iomanip>
#include <wmmintrin.h>

#ifdef WIN32
#include <emmintrin.h>
#endif	// #ifdef WIN32
#ifdef __GNUC__
#include <x86intrin.h>
#endif	// #ifdef __GNUC__

namespace libstark{
namespace Protocols{
namespace CryptoCommitment{

using std::vector;

//AES128 hash specific functions
namespace{
    //macros

#define AES_128_key_exp(k, rcon) aes_128_key_expansion(k, _mm_aeskeygenassist_si128(k, rcon))


static __m128i aes_128_key_expansion(__m128i key, __m128i keygened){
    keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3,3,3,3));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    return _mm_xor_si128(key, keygened);
}

//encrypts the data
inline __m128i aes128_enc(const __m128i& data, const __m128i& enc_key){
    __m128i m  = data;
    
    __m128i k0 = enc_key;
    m = _mm_xor_si128       (m, k0);
    
    __m128i k1  = AES_128_key_exp(k0, 0x01);
    m = _mm_aesenc_si128    (m, k1);
    
    __m128i k2  = AES_128_key_exp(k1, 0x02);
    m = _mm_aesenc_si128    (m, k2);
    
    __m128i k3  = AES_128_key_exp(k2, 0x04);
    m = _mm_aesenc_si128    (m, k3);
    
    __m128i k4  = AES_128_key_exp(k3, 0x08);
    m = _mm_aesenc_si128    (m, k4);
    
    __m128i k5  = AES_128_key_exp(k4, 0x10);
    m = _mm_aesenc_si128    (m, k5);
    
    __m128i k6  = AES_128_key_exp(k5, 0x20);
    m = _mm_aesenc_si128    (m, k6);
    
    __m128i k7  = AES_128_key_exp(k6, 0x40);
    m = _mm_aesenc_si128    (m, k7);
    
    __m128i k8  = AES_128_key_exp(k7, 0x80);
    m = _mm_aesenc_si128    (m, k8);
    
    __m128i k9  = AES_128_key_exp(k8, 0x1B);
    m = _mm_aesenc_si128    (m, k9);
    
    __m128i k10  = AES_128_key_exp(k9, 0x36);
    m = _mm_aesenclast_si128(m, k10);
    
    return m;
}
}

//hashes 64 bytes from src into 32 bytes in dst
void hash(void const* const src, void * const dst){
    
    //
    //Code for SHA-256
    //

    //SHA256_CTX sha256;
    //SHA256_Init(&sha256);
    //SHA256_Update(&sha256,src,hash_src_len);
    //SHA256_Final((unsigned char*)dst,&sha256);

    //
    //Code for AES-128 based hash
    //
    
    const __m128i key = _mm_loadu_si128((__m128i*)src);
    const __m128i plaintext = _mm_loadu_si128(((__m128i*)src)+1);
    const __m128i encRes = aes128_enc(plaintext, key);

    _mm_storeu_si128((__m128i*)dst, _mm_xor_si128(encRes,plaintext));
}

hashDigest_t hash(void const* const src){
    hashDigest_t res;
    hash(src,&res);
    return res;
}

std::string hashDigest_t::toString()const{
    std::stringstream stream;
    stream << std::hex;
    
    short* shortsBuff = (short*)&buffer;
    for(unsigned short i=0; i< sizeof(hashDigest_t)/sizeof(short); i++){
        stream << std::setfill('0')<< std::setw(sizeof(short)*2) << shortsBuff[i] <<" ";
    }

    return stream.str();
}

bool operator==(const hashDigest_t& a, const hashDigest_t& b){
    return 0 == std::memcmp(&a,&b,sizeof(hashDigest_t));
}

bool operator!=(const hashDigest_t& a, const hashDigest_t& b){
    return !(a==b);
}

bool operator<(const hashDigest_t& a, const hashDigest_t& b){
    return 0 < std::memcmp(&a,&b,sizeof(hashDigest_t));
}
unsigned short getBlockSize(){
    return (1U<<logBytesPerHash)/sizeof(Algebra::FieldElement);
}

unsigned short getDualBlockSize(){
    return 2*getBlockSize();
}

size_t getBlockIndex(const size_t elementIndex){
    return elementIndex/getBlockSize();
}

size_t getElementIndex(const size_t blockIndex){
    return blockIndex * getBlockSize();
}

unsigned short getOffsetInBlock(const size_t index){
    return index - getBlockIndex(index)*getBlockSize();
}

unsigned short getOffsetInDualBlock(const size_t index){
    return index - (getBlockIndex(index) & ~1UL)*getBlockSize();
}



//
// Constructs a Merkle tree for the src buffer (srcLen expected in bytes)
// The tree is written to dst, and its root is returned.
// It is expected src_logLen is in bytes.
// It is expected the size of dst in bytes is at least srcLen.
//
hashDigest_t constructMerkleTree(void const* const src, const short src_logLen, void * const dst){

    using Infrastructure::POW2;

    short curr_dst_logLen = src_logLen - logBytesPerHash - 1;
    hashDigest_t* curr_src = (hashDigest_t*)src;
    _COMMON_ASSERT(src_logLen > logBytesPerHash,"It is assumed the source length contains a power of 2 blocks of 256 bits, src_logLen = " + std::to_string(src_logLen));

    while (curr_dst_logLen >= 0){
        hashDigest_t* curr_dst = ((hashDigest_t*)dst) + (1UL<<curr_dst_logLen);
        const size_t curr_dst_len = POW2(curr_dst_logLen);
#pragma omp parallel for
        for(plooplongtype i=0; i<curr_dst_len; i++){
            hash(curr_src+(i<<1UL),curr_dst+i);
        }

        curr_src = curr_dst;
        curr_dst_logLen--;
    }

        //return tree root
        return ((hashDigest_t*)dst)[1];
}

hashDigest_t getMerkleCommitmentInplace(void * dataInp, const short src_logLen){

    using Infrastructure::POW2;
    _COMMON_ASSERT(src_logLen > logBytesPerHash,"It is assumed the source length contains a power of 2 blocks of 256 bits, src_logLen = " + std::to_string(src_logLen));


    const unsigned short logLen = src_logLen - logBytesPerHash;
    const size_t buffLen = POW2(logLen);
    hashDigest_t* data = (hashDigest_t*)dataInp;

    
    for (unsigned short currStepLog=1; currStepLog <= logLen; currStepLog++){
        const size_t currStep = POW2(currStepLog);
        const size_t prevStep = POW2(currStepLog-1);
#pragma omp parallel for
        for(plooplongtype i=0; i<buffLen; i+=currStep){
            data[i+1] = data[i+prevStep];
            hash(data+i,data+i);
        }
    }

        //return tree root
        return data[0];
}

//
// Constructs a Merkle sub-tree for a sigment in the src buffer (srcLen expected in bytes)
// The sub - tree is written to dst
// It is expected src_logLen is in bytes.
// It is expected the size of dst in bytes is at least srcLen.
//
void constructMerkleSubTree(void const* const src, const short src_logLen, const size_t sigment_logLen, const size_t sigment_index, void * const dst){

    using Infrastructure::POW2;

    short curr_dst_row_logLen = src_logLen - logBytesPerHash - 1;
    short curr_sigment_logLen = sigment_logLen - logBytesPerHash - 1;
    hashDigest_t* curr_src_shifted = (hashDigest_t*)src + POW2(sigment_logLen - logBytesPerHash)*sigment_index;
    
    _COMMON_ASSERT(sigment_logLen > logBytesPerHash,"It is assumed the source length contains a power of 2 blocks of 256 bits");

    while (curr_sigment_logLen >=0){
        hashDigest_t* curr_dst_row = ((hashDigest_t*)dst) + POW2(curr_dst_row_logLen);
        hashDigest_t* curr_dst_shifted = curr_dst_row + POW2(curr_sigment_logLen)*sigment_index;
        const size_t curr_sigment_len = POW2(curr_sigment_logLen);
#pragma omp parallel for
        for(plooplongtype i=0; i<curr_sigment_len; i++){
            hash(curr_src_shifted+(i<<1UL),curr_dst_shifted+i);
        }

        curr_src_shifted = curr_dst_shifted;
        curr_dst_row_logLen--;
        curr_sigment_logLen--;
    }
}

path_t getPathToBlock(void const*const tree, const short src_logLen, const size_t blockIndex){
    path_t result;
    hashDigest_t const*const tree_ = (hashDigest_t const*const)tree;
    const short firstRow_logLen = src_logLen - logBytesPerHash - 1;
    size_t curr_offset = (1UL<<firstRow_logLen) + (blockIndex>>1);
    
    while(curr_offset > 1UL){
        result.push_back(tree_[curr_offset ^ 1UL]);
        curr_offset >>= 1;
    }
    
    return result;
}

vector<path_t> getPathToBlocksInPlace(void * dataInp, const short src_logLen, const vector<size_t>& blockIndices){
    using Infrastructure::POW2;
    _COMMON_ASSERT(src_logLen > logBytesPerHash,"It is assumed the source length contains a power of 2 blocks of 256 bits, src_logLen = " + std::to_string(src_logLen));


    const unsigned short logLen = src_logLen - logBytesPerHash;
    const size_t buffLen = POW2(logLen);
    hashDigest_t* data = (hashDigest_t*)dataInp;

    vector<path_t> res(blockIndices.size());

    
    for (unsigned short currStepLog=1; currStepLog <= logLen; currStepLog++){
        const size_t currStep = POW2(currStepLog);
        const size_t prevStep = POW2(currStepLog-1);

        //construct next layer
#pragma omp parallel for
        for(plooplongtype i=0; i<buffLen; i+=currStep){
            data[i+1UL] = data[i+prevStep];
            hash(data+i,data+i);
        }

        //collect path data
        {
            if(currStepLog == logLen){
                //no need to collect the root
                continue;
            }
            
            for(unsigned int q=0; q<blockIndices.size(); q++){
                const size_t currLayerBlock = blockIndices[q]>>(currStepLog-1);
                const size_t currTwin = currLayerBlock ^ 1UL;
                const size_t TwinLocation = currTwin*currStep;
                res[q].push_back(data[TwinLocation]);
            }
        }
    }

    return res;
}

bool verifyPathToBlock(void const*const blockData, const hashDigest_t& root, const path_t& path, const size_t blockIndex){
    const short firstRow_logLen = path.size();
    size_t curr_offset = (1UL<<firstRow_logLen) + (blockIndex>>1);

    auto currHash = hash(blockData);

    for(size_t i=0; i < path.size(); i++){
        hashDigest_t hash_src[2];
        hash_src[(curr_offset&1UL) ^ 1UL] = path[i];
        hash_src[(curr_offset&1UL)] = currHash;

        currHash = hash(&hash_src);

        curr_offset >>= 1;
    }
    
    //verify root
    return currHash == root;
}

//
// An efficient representation of subtree containing only
// data needed to pass to show consistency of many queried elements
// with the commitment
//
bool SparceMerkleLayer::hasElement(const size_t idx)const{
    return (data_.find(idx) != data_.end());
}

void SparceMerkleLayer::addEntry(const size_t idx, const hashDigest_t& data){
    data_[idx] = data;
}

void SparceMerkleLayer::deleteEntry(const size_t idx){
    data_.erase(data_.find(idx));
}

const hashDigest_t& SparceMerkleLayer::readData(const size_t idx)const{
    return data_.at(idx);
}

hashDigest_t SparceMerkleLayer::hashPair(const size_t idx)const{
    hashDigest_t src[2];
    src[0] = readData(idx<<1);
    src[1] = readData((idx<<1)^1UL);
    return hash(src);
}

SparceMerkleLayer SparceMerkleLayer::calculateNextLayer(const SparceMerkleLayer& received)const{
    SparceMerkleLayer res = received;
    
    //as it is sufficient to scan only pairs
    //we can skip every second entry,
    //as it is handled already by its mate
    bool needToCalc = false;
    for(const auto& v: data_){
        needToCalc = !needToCalc;
        if(!needToCalc){
            continue;
        }
        const size_t currPairIdx = v.first>>1;
        res.addEntry(currPairIdx, hashPair(currPairIdx));
    }
    return res;
}

std::vector<hashDigest_t> SparceMerkleLayer::toVector()const{
    std::vector<hashDigest_t> res;
    for( const auto& e : data_){
        res.push_back(e.second);
    }
    return res;
}

std::set<size_t> SparceMerkleLayer::getIndices()const{
    std::set<size_t> res;
    for( const auto& e : data_){
        res.insert(e.first);
    }
    return res;
}
    
SparceMerkleTree::SparceMerkleTree(const short src_logLen){
    const short src_logLenInHashes = src_logLen - logBytesPerHash;
    layers_.resize(src_logLenInHashes);
}

//Serialization
std::vector<hashDigest_t> SparceMerkleTree::toVector()const{
    const auto serializationMapping = getSerializationMapping(layers_[0].getIndices());
    
    std::vector<hashDigest_t> res;
    for(const auto loc : serializationMapping){
        res.push_back(layers_[loc.first].readData(loc.second));
    }
    
    return res;
}

//De serialization
void SparceMerkleTree::DeSerialize(const std::set<size_t>& queriedIndices, const std::vector<hashDigest_t>& serializedSubtree){
    
    const auto serializationMapping = getSerializationMapping(queriedIndices);
    _COMMON_ASSERT(serializationMapping.size() == serializedSubtree.size(), 
        "The serialization length is not as expected : " + std::to_string(serializationMapping.size()) + " != " + std::to_string(serializedSubtree.size()));
    for(unsigned int i =0; i < serializationMapping.size(); i++){
        layers_[serializationMapping[i].first].addEntry(serializationMapping[i].second , serializedSubtree[i]);
    }
}

std::vector< std::pair<short,size_t> > SparceMerkleTree::getSerializationMapping(const std::set<size_t>& queriedIndices)const{
    std::vector< std::pair<short,size_t> > res;
    
    //A partial mapping from index that can be calculated
    //to whether it is explicit in the serialized subtree (true)
    //or implied by lower layer (false)
    std::map<size_t,bool> knownIndices;

    for(const size_t& idx : queriedIndices){
        knownIndices[idx] = true;
        knownIndices[idx^1UL] = true;
    }

    for(unsigned short layerIdx = 0; layerIdx <  layers_.size(); layerIdx++){
        std::map<size_t,bool> next_knownIndices;
        for(const auto& known : knownIndices){
            
            if(known.second == true){
                //If current data is explicit in subtree, it must be fetched from serialization
                res.push_back(std::pair<short,size_t>(layerIdx,known.first));
            }
            
            //Any case, the hash of current data and its mate
            //is computed, thus known implicitly
            next_knownIndices[known.first>>1] = false;
        }

        //Any element in next known indices that is known must have its mate
        //known as well, thus if it is not known implicitly it must be passed explicitly
        //in the serialization
        std::vector<size_t> explictlyKnown;
        for(const auto& k : next_knownIndices){
            if (next_knownIndices.count(k.first^1UL) == 0){
                explictlyKnown.push_back(k.first^1UL);
            }
        }
        for(const auto& idx : explictlyKnown){
            next_knownIndices[idx] = true;
        }

        //switch the knowledge lists and continue
        knownIndices = next_knownIndices;
    }

    return res;
}

void SparceMerkleTree::addPath(const std::array<hashDigest_t,2>& data, const path_t& path, const size_t pairIdx){
    layers_[0].addEntry(pairIdx<<1,data[0]);
    layers_[0].addEntry((pairIdx<<1)^1UL,data[1]);
    
    size_t currIdx = pairIdx;
    for(size_t i=1; i< layers_.size(); i++){

        //just write the path
        layers_[i].addEntry(currIdx^1UL,path[i-1UL]);

        //update the currIdx
        currIdx = currIdx>>1;
    }
}

hashDigest_t SparceMerkleTree::calculateRoot()const{
    SparceMerkleLayer currLayer;

    for(const auto& receivedLayer : layers_){
        currLayer = currLayer.calculateNextLayer(receivedLayer);
    }
    
    const hashDigest_t rootCalculated = currLayer.hashPair(0);

    return rootCalculated;
}

bool SparceMerkleTree::hasData(const size_t idx)const{
    return layers_[0].hasElement(idx);
}

const hashDigest_t& SparceMerkleTree::readData(const size_t idx)const{
    return layers_[0].readData(idx);
}

} // namespace CryptoCommitment
} // namespace Protocols
} // namespace libstark
