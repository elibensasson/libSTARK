#ifndef MERKLECOMMITMENT_HPP__
#define MERKLECOMMITMENT_HPP__

#include <vector>
#include <array>
#include <cstring>
#include <map>
#include <set>
#include <string>

namespace libstark{
namespace Protocols{
namespace CryptoCommitment{

struct hashDigest_t{
    char buffer[128/8];
    
    std::string toString()const;
};

bool operator==(const hashDigest_t& a, const hashDigest_t& b);
bool operator!=(const hashDigest_t& a, const hashDigest_t& b);
bool operator<(const hashDigest_t& a, const hashDigest_t& b);

typedef std::vector<hashDigest_t> path_t;

//hashes 64 bytes from src into 32 bytes in dst
void hash(void const* const src, void * const dst); 
hashDigest_t hash(void const* const src); 

const short logBytesPerHash = 4;

unsigned short getBlockSize();
unsigned short getDualBlockSize();
size_t getBlockIndex(const size_t elementIndex);
size_t getElementIndex(const size_t blockIndex);
unsigned short getOffsetInBlock(const size_t index);
unsigned short getOffsetInDualBlock(const size_t index);

//
// Constructs a Merkle tree for the src buffer (srcLen expected in bytes)
// The tree is written to dst, and its root is returned.
// It is expected src_logLen is in bytes.
// It is expected the size of dst in bytes is at least srcLen.
//
hashDigest_t constructMerkleTree(void const* const src, const short src_logLen, void * const dst);
hashDigest_t getMerkleCommitmentInplace(void * data, const short src_logLen);

//
// Constructs a Merkle sub-tree for a sigment in the src buffer (srcLen expected in bytes)
// The sub - tree is written to dst
// It is expected src_logLen is in bytes.
// It is expected the size of dst in bytes is at least srcLen.
//
void constructMerkleSubTree(void const* const src, const short src_logLen, const size_t sigment_logLen, const size_t sigment_index, void * const dst);

path_t getPathToBlock(void const*const tree, const short src_logLen, const size_t blockIndex);
std::vector<path_t> getPathToBlocksInPlace(void * data, const short src_logLen, const std::vector<size_t>& blockIndces);

bool verifyPathToBlock(void const*const blockData, const hashDigest_t& root, const path_t& path, const size_t blockIndex);

//
// An efficient representation of subtree containing only
// data needed to pass to show consistency of many queried elements
// with the commitment
//
class SparceMerkleLayer{
public:
    bool hasElement(const size_t idx)const;
    void addEntry(const size_t idx, const hashDigest_t& data);
    void deleteEntry(const size_t idx);
    const hashDigest_t& readData(const size_t idx)const;
    
    //idx is the index of a pair of hashes
    hashDigest_t hashPair(const size_t idx)const;
    
    //calculates the next layer, with merging received data, for verification
    SparceMerkleLayer calculateNextLayer(const SparceMerkleLayer& recieved)const;
    
    std::vector<hashDigest_t> toVector()const;
    std::set<size_t> getIndices()const;
private:
    std::map<size_t,hashDigest_t> data_;
};

class SparceMerkleTree{
public:
    //construct empty sparse tree
    // It is expected src_logLen is in bytes.
    SparceMerkleTree(const short src_logLen);
    
    //De serialization
    void DeSerialize(const std::set<size_t>& queriedIndices, const std::vector<hashDigest_t>& serializedSubtree);

    void addPath(const std::array<hashDigest_t,2>& data, const path_t& path, const size_t pairIdx);
    hashDigest_t calculateRoot()const;

    bool hasData(const size_t idx)const;
    const hashDigest_t& readData(const size_t idx)const;
    
    //Serialization
    std::vector<hashDigest_t> toVector()const;
    
    //used to get expected results length
    std::vector< std::pair<short,size_t> > getSerializationMapping(const std::set<size_t>& queriedIndices)const;
private:
    std::vector<SparceMerkleLayer> layers_;
};

} // namespace CryptoCommitment
} // namespace Protocols
} // namespace libstark

#endif //#ifndef MERKLECOMMITMENT_HPP__
