/**
 *       @file  FoldNetwork.hpp
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

#ifndef FOLD_NETWORK_HPP__
#define FOLD_NETWORK_HPP__

#include "RoutingNetwork.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class foldLeftNetwork
 * @brief The network that implements folding left operation (permutation)
 *
 * The fold left operation on a word \f$ \omega \in \{0,1\}^k \f$ is defined
 * in the following way: we represent \f$ \omega \f$ as \f$ \omega = xyz \f$ such that
 * \f$ x,z \in \{0,1\}^{\lfloor \frac{k}{2} \rfloor} \f$ and \f$ y \in \{0,1,\varepsilon\}\f$,
 * so the mapping is defined by \f$ xyz \mapsto  x \cdot y \cdot ( z \oplus x^R )\f$ where \f$x^R \f$
 * is the reverse of \f$x\f$.
 *
 * The foldLeft network of degree \f$k\f$ is a network with \f$ 1 + \lceil \frac{k}{2} \rceil \f$ layers,
 * namely \f$ \ell_0 , \ell_1 ,\dots , \ell_{\lceil \frac{k}{2} \rceil} \f$, and \f$ 2^k \f$ nodes per layer,
 * labeled by  \f$ {0 , 1 , \dots , 2^k} \f$.
 * We would prefer to interpret the labels as binary vectors of \f$ k \f$ bits,
 * which is \f$\{0,1\}^k\f$.
 * We use the trivial bijection between the two sets, which maps an integer \f$ n \in [2^k] \f$
 * to its binary representation using exactly \f$ k \f$ bits.
 * 
 * A node \f$ (\ell_i , \omega) \f$ can pass its data exactly to one neighbour.
 * which is \f$ n = ( \ell_{i+1} , \omega) \f$ if \f$ i+1 = k-i  \vee \omega_{i+1} = \omega_{i+1} \oplus \omega_{k-i} \f$ or to \f$ n = ( \ell_{i+1} , \omega \oplus e_{i+1}) \f$ otherwise.
 *
 * For example, if \f$ k = 3 \f$ the neighbor of \f$ (\ell_0 , 011) \f$ is
 * \f$ (\ell_1 , 010) \f$, and the neighbor of \f$ (\ell_1 , 010) \f$ is
 * \f$ (\ell_2 , 010) \f$
 *
 * Notice this defines a routing in distinct vertexes of the permutation foldLeft.
 *
 * This class implements the above defined network, and constructs it for a given dimension \f$k\f$.
 * 
 * @note The generated respects the order of data in the first layer, meaning the data in \f$ (\ell_0 , \omega) \f$
 * is \f$ \omega \f$ (it's decimal representation)
 */
class foldLeftNetwork : public RoutingNetwork{
public:
	foldLeftNetwork(const layerID_t& k):k_(k){;}
	
	layerID_t width() const {return 1 + k_/2 + k_%2;} // = 1 + ceil(k/2)
	labelID_t height() const {return 1<<k_;}
	dataID_t getDataID(const layerID_t& l, const labelID_t& w) const;

private:
	const layerID_t k_;
};


/**
 * @class foldRightNetwork
 * @brief The network that implements folding right operation (permutation)
 *
 * The fold right operation on a word \f$ \omega \in \{0,1\}^k \f$ is defined
 * in the following way: we represent \f$ \omega \f$ as \f$ \omega = xyz \f$ such that
 * \f$ x,z \in \{0,1\}^{\lfloor \frac{k}{2} \rfloor} \f$ and \f$ y \in \{0,1,\varepsilon\}\f$,
 * so the mapping is defined by \f$ xyz \mapsto  (x \oplus z^R) \cdot y \cdot z \f$ where \f$z^R \f$
 * is the reverse of \f$z\f$.
 *
 * The foldRight network of degree \f$k\f$ is a network with \f$ 1 + \lfloor \frac{k}{2} \rfloor \f$ layers,
 * namely \f$ \ell_0 , \ell_1 ,\dots , \ell_{\lfloor \frac{k}{2} \rfloor} \f$, and \f$ 2^k \f$ nodes per layer,
 * labeled by  \f$ {0 , 1 , \dots , 2^k} \f$.
 * We would prefer to interpret the labels as binary vectors of \f$ k \f$ bits,
 * which is \f$\{0,1\}^k\f$.
 * We use the trivial bijection between the two sets, which maps an integer \f$ n \in [2^k] \f$
 * to its binary representation using exactly \f$ k \f$ bits.
 * 
 * A node \f$ (\ell_i , \omega) \f$ can pass its data exactly to one neighbour,
 * which is \f$ n = ( \ell_{i+1} , \omega) \f$ if 
 * \f$ \omega_{ \lceil \frac{k}{2} \rceil + 1 +  i} =  \omega_{ \lceil \frac{k}{2} \rceil + 1 +  i} \oplus \omega_{1 + k - (\lceil \frac{k}{2} \rceil  + 1 +  i)} \f$
 * or to \f$ n = ( \ell_{i+1} , \omega \oplus e_{\lceil \frac{k}{2} \rceil + i + 1}) \f$ otherwise.
 *
 * For example, if \f$ k = 3 \f$ the neighbor of \f$ (\ell_0 , 011) \f$ is
 * \f$ (\ell_1 , 111) \f$, and the neighbor of \f$ (\ell_0 , 010) \f$ is
 * \f$ (\ell_1 , 010) \f$
 *
 * Notice this defines a routing in distinct vertexes of the permutation foldRight.
 *
 * This class implements the above defined network, and constructs it for a given dimension \f$k\f$.
 * 
 * @note The generated respects the order of data in the first layer, meaning the data in \f$ (\ell_0 , \omega) \f$
 * is \f$ \omega \f$ (it's decimal representation)
 */
class foldRightNetwork : public RoutingNetwork{
public:
	foldRightNetwork(const layerID_t& k):k_(k){;}
	
	layerID_t width() const {return 1 + k_/2;} // = 1 + floor(k/2) = 1 + k - ceil(k/2)
	labelID_t height() const {return 1<<k_;}
	dataID_t getDataID(const layerID_t& l, const labelID_t& w) const;

private:
	const layerID_t k_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //FOLD_NETWORK_HPP__
