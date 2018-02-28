#include "BitReverseButterfliesNetwork.hpp"

namespace libstark{
namespace BairToAcsp{

typedef RoutingNetwork::layerID_t layerID_t;
typedef RoutingNetwork::labelID_t labelID_t;
typedef RoutingNetwork::dataID_t dataID_t;

dataID_t BitReverseButterfliesNetwork::getDataID(const layerID_t& l, const labelID_t& w) const{
	const layerID_t first_left1 = 0;
	const layerID_t first_right2 = k_/2 + k_%2; //=ceil(k/2)
	const layerID_t first_left3 = k_;
	const layerID_t first_identity = k_ + k_/2 + k_%2;
	
	//if $l$ is the range of the identity permutations
	//just return the reverse of $w$
	if (l >= first_identity) {
		return reverseBits(w)>>(sizeof(w)*8 - k_);
	}
	
	layerID_t curr_l = l;
	labelID_t curr_w = w;

	if (curr_l > first_left3){
		curr_w = fleft_.getDataID(curr_l-first_left3, curr_w);
		curr_l = first_left3;
	}

	if (curr_l > first_right2){
		curr_w = fright_.getDataID(curr_l-first_right2, curr_w);
		curr_l = first_right2;
	}

	if (curr_l > first_left1){
		curr_w = fleft_.getDataID(curr_l-first_left1, curr_w);
		curr_l = first_left1;
	}

	return curr_w;
}
} //namespace BairToAcsp
} //namespace libstark
