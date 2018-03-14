/**
 *       @file  DeBruijnPermutationNet.hpp
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

#ifndef DE_BRUIJN_PERMUTATION_NET_HPP__
#define DE_BRUIJN_PERMUTATION_NET_HPP__

#include "BtrflyBasedPermutation.hpp"
#include "Deg2PermutationRoutingNet.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class DeBruijnPermutationNet
 * @brief A network that can rout any permutation using 4 De Bruijn networks
 * 
 * # De Bruijn routing network #
 * A De Bruijn routing network of degree \f$ k \f$ is a routing network with
 * \f$ k+1 \f$ layers, namely \f$ \ell_0 , \ell_1 , \dots , \ell_k \f$, and
 * \f$ 2^k \f$ nodes in each layer, labeled by  \f$ {0 , 1 , \dots , 2^k} \f$.
 * We would prefer to interpret the labels as binary vectors of \f$ k \f$ bits,
 * which is \f$\{0,1\}^k\f$.
 * We use the trivial bijection between the two sets, which maps an integer \f$ n \in [2^k] \f$
 * to its binary representation using exactly \f$ k \f$ bits.
 * 
 * We define \f$ sr : \{0,1\}^k \to \{0,1\}^k \f$ as the cyclic shift right.
 *
 * A node \f$ (\ell_i , \omega) \f$ can pass its data exactly to one of its two neighbours,
 * which are \f$ n_1 = ( \ell_{i+1} , sr(\omega)) \f$ and \f$ n_1 = ( \ell_{i+1} , sr(\omega) \oplus e_1) \f$.
 *
 * For example, if \f$ k = 3 \f$ the neighbors of \f$ (\ell_0 , 011) \f$ are
 * \f$ (\ell_1 , 101), (\ell_1 , 100) \f$, and the neighbors of \f$ (\ell_1 , 101) \f$ are
 * \f$ (\ell_2 , 110), (\ell_2 , 111) \f$
 *
 * # De-Bruijn to Butterfly isomorphism #
 * The isomorphism is between network of degree \f$k\f$ and defined by
 * \f$ \phi(\ell_i , \omega) = (\ell_i , sr^{(k + 1 -l) \mod k}(\omega)) \f$
 *
 * # What is this class #
 * This class uses the isomorphism mentioned above. It holds an instance of a 4 butterfly network
 * that routes the needed permutation, and translates the addresses using the mentioned isomorphism to
 * get a De Bruijn from it.
 */
class DeBruijnPermutationNet : public Deg2PermutationRoutingNet {
public:
	DeBruijnPermutationNet(const labelID_t& k)
		:Deg2PermutationRoutingNet(1<<k),
		k_(k),
		btrfly_(k){;}
	
	layerID_t width() const {return btrfly_.width();}
	labelID_t height() const {return btrfly_.height();}
	dataID_t getDataID(const layerID_t& l, const labelID_t& w) const;
	void rout(const permutation_t& permutation);
    short routingBit(const layerID_t& l, const labelID_t& w) const;

private:
	const layerID_t k_;
	BtrflyBasedPermutation btrfly_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //DE_BRUIJN_PERMUTATION_NET_HPP__
