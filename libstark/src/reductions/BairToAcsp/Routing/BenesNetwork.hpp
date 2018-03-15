/**
 *       @file  BenesNetwork.hpp
 *      @brief  Benes network generator and representation
 *
 *     @author  Michael Riabzev (), RiabzevMichael@gmail.com
 *
 *   @internal
 *     Created  11/27/2013
 *     Company  SCIPR-LAB
 *   Copyright  Copyright (c) 2013, SCIPR-LAB
 * =====================================================================================
 */

#ifndef BENES_NETWORK_HPP__
#define BENES_NETWORK_HPP__

#include "PermutationRoutingNet.hpp"
#include "TandemNetwork.hpp"
#include "common/langCommon/Sequence.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class BenesNetwork
 * 
 * # Butterfly routing network #
 * A butterfly routing network of degree \f$ k \f$ is a routing network with
 * \f$ k+1 \f$ layers, namely \f$ \ell_0 , \ell_1 , \dots , \ell_k \f$, and
 * \f$ 2^k \f$ nodes in each layer, labeled by  \f$ {0 , 1 , \dots , 2^k} \f$.
 * We would prefer to interpret the labels as binary vectors of \f$ k \f$ bits,
 * which is \f$\{0,1\}^k\f$.
 * We use the trivial bijection between the two sets, which maps an integer \f$ n \in [2^k] \f$
 * to its binary representation using exactly \f$ k \f$ bits.
 * 
 * A node \f$ (\ell_i , \omega) \f$ can pass its data exactly to one of its two neighbours,
 * which are \f$ n_1 = ( \ell_{i+1} , \omega) \f$ and \f$ n_1 = ( \ell_{i+1} , \omega \oplus e_{i+1}) \f$.
 *
 * For example, if \f$ k = 3 \f$ the neighbors of \f$ (\ell_0 , 011) \f$ are
 * \f$ (\ell_1 , 011), (\ell_1 , 010) \f$, and the neighbors of \f$ (\ell_1 , 011) \f$ are
 * \f$ (\ell_2 , 011), (\ell_2 , 001) \f$
 *
 * # Reverse butterfly routing network #
 * A reverse butterfly routing network of degree \f$ k \f$ is a routing network with
 * \f$ k+1 \f$ layers, namely \f$ \ell_0 , \ell_1 , \dots , \ell_k \f$, and
 * \f$ 2^k \f$ nodes in each layer, labeled by  \f$ {0 , 1 , \dots , 2^k} \f$.
 * We would prefer to interpret the labels as binary vectors of \f$ k \f$ bits,
 * which is \f$\{0,1\}^k\f$.
 * We use the trivial bijection between the two sets, which maps an integer \f$ n \in [2^k] \f$
 * to its binary representation using exactly \f$ k \f$ bits.
 * 
 * A node \f$ (\ell_i , \omega) \f$ can pass its data exactly to one of its two neighbours,
 * which are \f$ n_1 = ( \ell_{i+1} , \omega) \f$ and \f$ n_1 = ( \ell_{i+1} , \omega \oplus e_{k-i}) \f$.
 *
 * For example, if \f$ k = 3 \f$ the neighbors of \f$ (\ell_0 , 011) \f$ are
 * \f$ (\ell_1 , 011), (\ell_1 , 111) \f$, and the neighbors of \f$ (\ell_1 , 011) \f$ are
 * \f$ (\ell_2 , 011), (\ell_2 , 001) \f$
 *
 * # Benes network #
 * A Benes network of degree \f$k\f$ is a routing network with
 * \f$ 2k + 1 \f$ layers, namely \f$ \ell_0 , \ell_1 , \dots , \ell_{2k+1} \f$, and
 * \f$ 2^k \f$ nodes in each layer, labeled by  \f$ {0 , 1 , \dots , 2^k} \f$.
 * We would prefer to interpret the labels as binary vectors of \f$ k \f$ bits,
 * which is \f$\{0,1\}^k\f$.
 * We use the trivial bijection between the two sets, which maps an integer \f$ n \in [2^k] \f$
 * to its binary representation using exactly \f$ k \f$ bits.
 * 
 * A Benes network is a concatenation of a reverse butterfly network, and a butterfly network,
 * such that the central layer is common to both network (it is the last layer of the reverse butterfly network
 * and the first layer of the butterfly network)
 *
 * ## Important fact ##
 * For any given permutation \f$ \pi: \{0,1,\dots,2^k -1\} \to \{0,1,\dots,2^k -1\} \f$,
 * there exists a Benes network of degree \f$ k \f$ that routs the data packets \f$ \{0,1,\dots,2^k -1\} \f$ using distinct
 * nodes, such that a data packet \f$ d \f$ is in the last layer in the node labeled \f$ \omega \f$
 * if and only if the data packet \f$ \pi(d) \f$ is located in the first layer at the node labeled \f$ \omega \f$
 *
 * # This class #
 * A class that represents a Benes permutation network for a given permutation
 * An instance of this class is initialized using a degree \f$ k \f$ and a sequence of integers
 * which assumed to represent a permutation when restricted to the first \f$ 2^k \f$ elements,
 * and constructs a Benes network that routs the given permutation.
 * 
 * @note The generated respects the order of data in the first layer, meaning the data in \f$ (\ell_0 , \omega) \f$
 * is \f$ \omega \f$ (it's decimal representation)
 */
class BenesNetwork: public PermutationRoutingNet {
public:
	BenesNetwork(const layerID_t& k)
		:PermutationRoutingNet(1<<k),
		k_(k),
		isRouted(false),
		data_(TandemNetwork::netsVec_t()) {};
	
	layerID_t width() const {return data_.width();}
	labelID_t height() const {return data_.height();}
	dataID_t getDataID(const layerID_t& l, const labelID_t& w) const;
	
	void rout(const permutation_t& permutation);

private:
	TandemNetwork::netsVec_t constructBenes(const permutation_t& permutation);
	labelID_t k_;
	bool isRouted;
	TandemNetwork data_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //BENES_NETWORK_HPP__
