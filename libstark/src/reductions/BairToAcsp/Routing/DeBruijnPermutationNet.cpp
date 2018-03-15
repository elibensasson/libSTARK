#include "DeBruijnPermutationNet.hpp"

#include "common/Infrastructure/Infrastructure.hpp"

namespace libstark{
namespace BairToAcsp{

typedef RoutingNetwork::layerID_t layerID_t;
typedef RoutingNetwork::labelID_t labelID_t;
typedef RoutingNetwork::dataID_t dataID_t;

namespace{

/***
 * @class fixIndexes
 * @brief permutes a given sequence for a specific fix needed for DeBruijn network
 * this is made to have the first column always ordered in increasing order 0,1,2,...
 *
 * We define \f$ sr : {0,1}^{numBits} \to {0,1}^{numBits} \f$ as 
 * the cyclic shift right operator.
 * And we define \f$ sl : {0,1}^{numBits} \to {0,1}^{numBits} \f$ as 
 * the cyclic shift left operator.
 * Let \f$ A \f$ be a given sequence (named "src" in the constructor),
 * this class defines a new sequence \f$ B \f$ such that
 * \f$ b_i = sr(a_{sl(i)}) \f$
 *
 * @note
 * For internal usage only
 * As one might see, if an instance leaves outside of the scope it was initialized in
 * it may cause a dangeling reference (to origSequence_)
 * it is defined with the knowledge that such a thing will not happen
 * because any instance of this class dies before the "rout" method ends
 * and the Sequence "src" is a parameter to this function
 */
class shiftRight : public Sequence<size_t>{
public:
	shiftRight(const Sequence<size_t>& src, const layerID_t& numBits)
	:src_(src),numBits_(numBits){};
	
	size_t getElementByIndex(index_t index)const {
        const labelID_t mask = (1<<numBits_)-1; // the mask is least segnificant $k$ bits set
	    const auto shifted_index = ((index<<1) | (index>>(numBits_-1))) & mask;
        const auto origVal = src_.getElementByIndex(shifted_index);
        const auto shifted_data = ((origVal>>1) | (origVal<<(numBits_-1))) & mask;

        return shifted_data;
    }
private:
    const Sequence<size_t>& src_;
    const size_t numBits_;
};

}

dataID_t DeBruijnPermutationNet::getDataID(const layerID_t& l, const labelID_t& w) const{
	const size_t shift_size = (l+k_-1)%k_;
	const labelID_t mask = (1<<k_)-1; // the mask is least segnificant $k$ bits set
	
	//cyclic shift left of least segnificant $k$ bits, $shift_size$ steps
	const labelID_t btrfly_w = ((w<<shift_size) | (w>>(k_-shift_size))) & mask;

	//get data from btrfly
	const dataID_t btrfly_data =  btrfly_.getDataID(l,btrfly_w);

    //shifted data - to keep the first column ordered
    const auto shifted_data = ((btrfly_data<<1) | (btrfly_data>>(k_-1))) & mask;

	return shifted_data;
}
	
void DeBruijnPermutationNet::rout(const permutation_t& permutation){
	btrfly_.rout(shiftRight(permutation,k_));
}

short DeBruijnPermutationNet::routingBit(const layerID_t& l, const labelID_t& w) const{
    
    const labelID_t mask = (1<<k_)-1;
    const auto neighbor1_label = ((w>>1) | (w<<(k_-1))) & mask;
    const auto neighbor2_label = neighbor1_label ^ 1;
			
    return getDataID(l,w) == getDataID(l+1,neighbor2_label);
}


} //namespace BairToAcsp
} //namespace libstark
