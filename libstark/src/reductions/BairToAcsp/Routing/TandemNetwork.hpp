/**
 *       @file  TandemNetwork.hpp
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

#ifndef TANDEM_NETWORK_HPP__
#define TANDEM_NETWORK_HPP__

#include "RoutingNetwork.hpp"

#include <memory>
#include <vector>

namespace libstark{
namespace BairToAcsp{

/**
 * @class TandemNetwork
 * @brief A network defined as a concatenation in tandem of some existing networks
 *
 * This implementation is initialized by a vector of RoutingNetworks, and represents
 * a network that is a concatenation of those network, in the given order.
 * It is assumed that the last layer of a given network is exactly the
 * same as the first layer of the following layer, and they are merged into a single
 * layer. This is only an assumption, and if the networks vector does not respect this,
 * the result is undefined. For example, if we have the vector \f$ (N_1, N_2, \dots , N_k) \f$
 * and amount of layers in \f$ N_i \f$ is \f$ \ell_i \f$, the amount of layers
 * in the concatenation networks is \f$ \ell_1 + (\ell_2 -1) + (\ell_3 - 1) + \cdots + (\ell_k - 1)\f$.
 *
 * @note It is assumed all the networks in vector has at least 1 layer, and the amount of nodes per layer is common to all of them, otherwise the result is undefined
 */
class TandemNetwork : public RoutingNetwork{
public:
	typedef std::vector<std::unique_ptr<RoutingNetwork>> netsVec_t;
	
	TandemNetwork(netsVec_t&& networks);
	layerID_t width()const;
	labelID_t height()const;
	dataID_t getDataID(const layerID_t& l, const labelID_t& w)const;
	void reset(netsVec_t&& networks);

private:
	netsVec_t networks_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //TANDEM_NETWORK_HPP__
