#ifndef DEG2_PERMUTATION_ROUTING_NET_HPP__
#define DEG2_PERMUTATION_ROUTING_NET_HPP__

#include "PermutationRoutingNet.hpp"

namespace libstark{
namespace BairToAcsp{

class Deg2PermutationRoutingNet : public PermutationRoutingNet {
public:
	
	Deg2PermutationRoutingNet(const labelID_t numElements):PermutationRoutingNet(numElements){;}
    virtual short routingBit(const layerID_t& l, const labelID_t& w) const = 0;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //  DEG2_PERMUTATION_ROUTING_NET_HPP__
