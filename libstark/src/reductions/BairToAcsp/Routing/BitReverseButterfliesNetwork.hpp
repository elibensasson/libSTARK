/**
 *       @file  BitReverseButterfliesNetwork.hpp
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

#ifndef BIT_REVERSE_BUTTERFLIES_NETWORK_HPP__
#define BIT_REVERSE_BUTTERFLIES_NETWORK_HPP__

#include "RoutingNetwork.hpp"
#include "FoldNetwork.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class BitReverseButterfliesNetwork
 * @brief A network that implements the bit reverse permutation using 2 butterfly networks
 *
 * We notice the following three facts:
 *
 * - The composition \f$ foldLeft \circ foldRight \circ foldLeft \f$ is exactly the bit reverse permutation
 *
 * - The foldLeftNetwork of degree \f$k\f$ is a restriction of the first half of a butterfly network of degree \f$k\f$ (layers \f$ \ell_0 \dots \ell_{\lceil \frac{k}{2} \rceil} \f$)
 * 
 * - The foldRightNetwork of degree \f$k\f$ is a restriction of the second half of a butterfly network of degree \f$k\f$ (layers \f$ \ell_{\lceil \frac{k}{2} \rceil \dots \ell_k} \f$)
 *
 *   From all above we conclude that we can route the bit reverse permutation on 1.5 butterfly networks, but instead of using a concatenation, we need in this case
 *   composition (the permutation of a network effects the location of the input data in the next network network). We may also implement the bit reverse
 *   permutation using 2 butterfly networks, by 'finishing' the second network using the identity permutation (each node passes the data to the node with the same
 *   label in the next layer).
 *
 *   This class implements exactly the described 2 butterflies bit reverse network, it is initialized using the wanted degree \f$k\f$.
 */
class BitReverseButterfliesNetwork : public RoutingNetwork{
public:
	BitReverseButterfliesNetwork(const layerID_t& k):
		k_(k),fleft_(k),fright_(k){;}
	
	layerID_t width() const {return 1 + 2*k_;} // = 1 + ceil(k/2)
	labelID_t height() const {return 1<<k_;}
	dataID_t getDataID(const layerID_t& l, const labelID_t& w) const;

private:
	const layerID_t k_;
	const foldLeftNetwork fleft_;
	const foldRightNetwork fright_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //BIT_REVERSE_BUTTERFLIES_NETWORK_HPP__
