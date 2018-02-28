#include "BtrflyBasedPermutation.hpp"

#include <vector>

namespace libstark{
namespace BairToAcsp{

using std::vector;

typedef RoutingNetwork::layerID_t layerID_t;
typedef RoutingNetwork::labelID_t labelID_t;
typedef RoutingNetwork::dataID_t dataID_t;

namespace{

/***
 * @class bitReverse
 * @brief permutes a given sequence by bit reversing its values
 *
 * We define \f$ rev : {0,1}^{numBits} \to {0,1}^{numBits} \f$ as 
 * the bit reversal operator.
 * Let \f$ A \f$ be a given sequence (named "src" in the constructor),
 * this class defines a new sequence \f$ B \f$ such that
 * \f$ b_i = rev(a_i) \f$
 *
 * @note
 * For internal usage only
 * As one might see, if an instance leaves outside of the scope it was initialized in
 * it may cause a dangeling reference (to origSequence_)
 * it is defined with the knowledge that such a thing will not happen
 * because any instance of this class dies before the "rout" method ends
 * and the Sequence "src" is a parameter to this function
 */
class bitReverse : public Sequence<size_t>{
public:
	bitReverse(const Sequence<size_t>& src, const layerID_t& numBits)
	:src_(src),numBits_(numBits){};
	
	size_t getElementByIndex(index_t index)const {
	    const auto origVal = src_.getElementByIndex(index);
        return reverseBits(origVal)>>(8*sizeof(origVal)-numBits_);
    }
private:
    const Sequence<size_t>& src_;
    const size_t numBits_;
};

}

dataID_t BtrflyBasedPermutation::getDataID(const layerID_t& l, const labelID_t& w) const{
	const layerID_t first_benesFirstHalf = 0;
	const layerID_t first_bitReverse = k_;
	const layerID_t first_benesSecondHalf = 3*k_;
	
	layerID_t curr_l = l;
	labelID_t curr_w = w;

	if (l >= first_benesSecondHalf) {
		curr_w =  benes_.getDataID(l - first_benesSecondHalf + k_,w);
	}
	else{
		if (curr_l > first_bitReverse){
			curr_w = bitRev_.getDataID(curr_l-first_bitReverse, curr_w);
			curr_l = first_bitReverse;
		}

		if (curr_l >= first_benesFirstHalf){
			curr_w = reverseBits(curr_w)>>(sizeof(curr_w)*8 - k_);
			curr_w = benes_.getDataID(curr_l-first_benesFirstHalf, curr_w);
			curr_l = first_benesFirstHalf;
		}
	}

	//fix the permutation done on the first layer indexes (bit reverse permutation)
	//by bit reversing all the data in the network
	//this is mandatory, because the second half of the Benes network 'assumes' the data
	//inserted to the first half with respect for the order, while before
	//this fix that order is shuffled
	dataID_t res = reverseBits(curr_w)>>(sizeof(curr_w)*8 - k_);

	return res;
}

void BtrflyBasedPermutation::rout(const permutation_t& permutation){
	benes_.rout(bitReverse(permutation,k_));
}

} //namespace BairToAcsp
} //namespace libstark
