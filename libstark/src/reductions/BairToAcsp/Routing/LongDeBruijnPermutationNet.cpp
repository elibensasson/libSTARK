#include "LongDeBruijnPermutationNet.hpp"
#include "common/Infrastructure/Infrastructure.hpp"

using Infrastructure::CEIL;
using Infrastructure::Log2;
using Infrastructure::POW2;

namespace libstark{
namespace BairToAcsp{

typedef RoutingNetwork::layerID_t layerID_t;
typedef RoutingNetwork::labelID_t labelID_t;
typedef RoutingNetwork::dataID_t dataID_t;

namespace{

/**
 * @class rightShiftedIndexes
 * @brief permutes a given sequence by shifting its values
 *
 * We define \f$ sr : {0,1}^{numBits} \to {0,1}^{numBits} \f$ as the cyclic
 * shift right operator.
 * Let \f$ A \f$ be a given sequence (named "src" in the constructor),
 * this class defines a new sequence \f$ B \f$ such that
 * \f$ b_i = sr^{numShifts}(a_i)} \f$
 *
 * @note
 * For internal usage only
 * As one might see, if an instance leaves outside of the scope it was initialized in
 * it may cause a dangeling reference (to origSequence_)
 * it is defined with the knowledge that such a thing will not happen
 * because any instance of this class dies before the "rout" method ends
 * and the Sequence "src" is a parameter to this function
 */

class rightShiftedIndexes : public Sequence<size_t>{
public:
	rightShiftedIndexes(const Sequence<size_t>& src, const layerID_t& numBits, const layerID_t& numShifts)
	:origSequence_(src), numBits_(numBits), numShifts_(numShifts % numBits){;}
	
	size_t getElementByIndex(index_t index)const {
        const index_t mask = (1<<numBits_)-1;
        const auto shifted_index = ((index>>numShifts_) | (index<<(numBits_ - numShifts_))) & mask;
        const auto data = origSequence_.getElementByIndex(shifted_index);
        return data;
	}
private:
	const Sequence<size_t>& origSequence_;
    const layerID_t numBits_;
    const layerID_t numShifts_;
};

} //nameless scope

LongDeBruijnPermutationNet::LongDeBruijnPermutationNet(const labelID_t& k) :
    Deg2PermutationRoutingNet(1<<k),
    DB_net_(k),
    k_(k),
    width_(POW2(CEIL(Log2(DB_net_.width())))){;}

void LongDeBruijnPermutationNet::rout(const permutation_t& permutation){
    const auto numShifts = width() - DB_net_.width();
    DB_net_.rout(rightShiftedIndexes(permutation,k_,numShifts));
}

dataID_t LongDeBruijnPermutationNet::getDataID(const layerID_t& l, const labelID_t& w) const{
    //If the vertex is not in the extension, access directly the DeBruijn network
    if (l < DB_net_.width()) return DB_net_.getDataID(l,w);
  
    //Otherwise, calculate the relevant DeBrujin vertex, by doing cyclic shifts left,
    //as many as the amount of layers we need to go back
    const auto mask = (1<<k_)-1;
    const auto numShifts = (l - (DB_net_.width()-1)) % k_;
    const auto shifted_w = ((w<<numShifts) | (w>>(k_ - numShifts))) & mask;
    const auto lastDbColumn = DB_net_.width()-1;
    return getDataID(lastDbColumn, shifted_w);
}

short LongDeBruijnPermutationNet::routingBit(const layerID_t& l, const labelID_t& w) const{
    //If the vertex is not in the extension, access directly the DeBruijn network
    if (l < DB_net_.width()-1) return DB_net_.routingBit(l,w);
    
    //Otherwise, return a constant 0
    //because the extension has constant routing
    return 0;
}

} //namespace BairToAcsp
} //namespace libstark
