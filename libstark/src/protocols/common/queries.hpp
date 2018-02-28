#ifndef PROTOCOLS_COMMON_HPP__
#define PROTOCOLS_COMMON_HPP__

#include "CryptoCommitment/MerkleCommitment.hpp"
#include <set>

namespace libstark{
namespace Protocols{

// A set of indices of pairs of hashDigest_t blocks
// this is the minimal block that can be read and verified
typedef std::set<unsigned long long> rawQuery_t;

// A serialize subset of a merkle tree as result
typedef std::vector<CryptoCommitment::hashDigest_t> rawResult_t;

} //namespace Protocols
} //namespace libstark

#endif //#ifndef PROTOCOLS_COMMON_HPP__
