/**
 *       @file  PermutationRoutingNet.hpp
 *      @brief  
 *
 * Detailed description starts here.
 *
 *     @author  Michael Riabzev (), RiabzevMichael@gmail.com
 *
 *   @internal
 *     Created  12/04/2013
 *     Company  SCIPR-LAB
 *   Copyright  Copyright (c) 2013, SCIPR-LAB
 * =====================================================================================
 */

#ifndef PERMUTATION_ROUTING_NET_HPP__
#define PERMUTATION_ROUTING_NET_HPP__

#include "RoutingNetwork.hpp"
#include "common/langCommon/Sequence.hpp"

#include <stdlib.h>

namespace libstark{
namespace BairToAcsp{

class PermutationRoutingNet : public RoutingNetwork {
public:
	typedef Sequence<size_t> permutation_t;
	
	PermutationRoutingNet(const labelID_t numElements):numElements_(numElements){;}
	virtual void rout(const permutation_t& permutation) = 0;
protected:
	bool isPermutation(const permutation_t& permutation)const;

	const labelID_t numElements_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //  PERMUTATION_ROUTING_NET_HPP__
