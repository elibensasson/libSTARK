/**
 *       @file  LongDeBruijnPermutationNet.hpp
 *      @brief  
 *
 * Detailed description starts here.
 *
 *     @author  Michael Riabzev (), RiabzevMichael@gmail.com
 *
 *   @internal
 *     Created  13/2/2014
 *     Company  SCIPR-LAB
 *   Copyright  Copyright (c) 2014, SCIPR-LAB
 * =====================================================================================
 */

#ifndef LONG_DE_BRUIJN_PERMUTATION_NET_HPP__
#define LONG_DE_BRUIJN_PERMUTATION_NET_HPP__

#include "DeBruijnPermutationNet.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class LongDeBruijnPermutationNet
 * @brief A network that can rout any permutation using 4 De Bruijn networks + reminder to make the amount of columns a power of 2
 *
 * It is simply a DeBruijn network, in tandem with a DeBruijn network that routs in constant direction:
 * each vertex \f$ ( \ell_i , \omega ) \f$ in the additional network has exactly one neighbor : \f$ (\ell_{i+1}, sr(\omega)) \f$
 *
 * This is used for the arithmetization step, where the amount of layers must be a power of 2
 */
class LongDeBruijnPermutationNet : public  Deg2PermutationRoutingNet{
public:
	LongDeBruijnPermutationNet(const labelID_t& k);
	layerID_t width() const {return width_;} 
	labelID_t height() const {return DB_net_.height();}
	dataID_t getDataID(const layerID_t& l, const labelID_t& w) const;
	void rout(const permutation_t& permutation);
    short routingBit(const layerID_t& l, const labelID_t& w) const;

private:
    DeBruijnPermutationNet DB_net_;
	const layerID_t k_;
    const layerID_t width_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //LONG_DE_BRUIJN_PERMUTATION_NET_HPP__
