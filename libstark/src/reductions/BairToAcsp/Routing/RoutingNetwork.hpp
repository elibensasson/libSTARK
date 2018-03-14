/**
 *       @file  routingNetwork.hpp
 *      @brief  
 *
 * Detailed description starts here.
 *
 *     @author  Michael Riabzev (), RiabzevMichael@gmail.com
 *
 *   @internal
 *     Created  11/27/2013
 *     Company  SCIPR-LAB
 *   Copyright  Copyright (c) 2013, SCIPR-LAB
 * =====================================================================================
 */

#ifndef ROUTING_NETWORK_HPP__
#define ROUTING_NETWORK_HPP__

#include <stdint.h>
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/ErrorHandling.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class RoutingNetwork
 * @brief An interface for routing networks
 *
 * A routing network with no constraints on, might be thought as
 * a matrix of nodes, each node holds some arbitrary data.
 * This may sound weird, but there are no constraints on 
 * on the data either. It may appear from no-where and disappear to nowhere.
 * Such a definition is used in order to get better performance,
 * because constraints defined must be enforced,
 * but we are not interested in doing that,
 * because any case, the initialization of classes implementing
 * this interface is done (in practical cases) using automatic
 * algorithms that build the network, such that it satisfies
 * all constraints that should be satisfied.
 *
 * The main difference between a routing network and a matrix
 * is the terminology used.
 *
 * The matrix columns are named network layers. It is expected that
 * data is passed in the network from one layer to the next layer
 * (although this is not enforced by this interface, as mentioned)
 *
 * Instead of rows, as we have in matrixes, we have nodes in each
 * layer, each node is labeled by a unique label (in the specific layer),
 * the labels are \f$ \{0 \dots (<\text{number of nodes per layer}>-1)\} \f$.
 *
 * One may ask 'what is the data in the node, on layer \f$ \ell \f$ and label \f$\omega\f$'.
 * the matrix-kind parallel question is 'what is the value of the cell in column \f$ \ell \f$ and row \f$ \omega \f$ '
 * or simply, for a matrix \f$M\f$ that is exactly \f$M_{\ell,\omega}\f$
 */
class RoutingNetwork{
public:
	typedef uint64_t layerID_t;
	typedef uint64_t labelID_t;
	typedef labelID_t dataID_t;
	
	virtual layerID_t width() const = 0;
	virtual labelID_t height() const = 0;
	virtual dataID_t getDataID(const layerID_t& l, const labelID_t& w) const = 0;

	virtual ~RoutingNetwork(){;}
};

//
//A common method used by some routing networks constructions
//
template <class T>
T reverseBits(const T& src){
	T res = src;
	const auto numBits = sizeof(T)*8;

	switch(numBits){
	case 64: res = ((res & 0xffffffff00000000)>>32) | ((res & 0x00000000ffffffff)<<32);
	case 32: res = ((res & 0xffff0000ffff0000)>>16) | ((res & 0x0000ffff0000ffff)<<16);
	case 16: res = ((res & 0xff00ff00ff00ff00)>>8) | ((res & 0x00ff00ff00ff00ff)<<8);
	case 8 : res = ((res & 0xf0f0f0f0f0f0f0f0)>>4) | ((res & 0x0f0f0f0f0f0f0f0f)<<4);
	case 4 : res = ((res & 0xcccccccccccccccc)>>2) | ((res & 0x3333333333333333)<<2);
	case 2 : res = ((res & 0xaaaaaaaaaaaaaaaa)>>1) | ((res & 0x5555555555555555)<<1); break;
	default : _COMMON_FATAL("Unsupported variable size for bit reversing (if it is because we have an architecture with 128 bit or more than 'WOW!' and the fix should be trivial, otherwise this should not happen)");
	}
	return res;
}

} //namespace BairToAcsp
} //namespace libstark

#endif //ROUTING_NETWORK_HPP__
