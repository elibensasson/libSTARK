#ifndef LONG_SYMMETRIC_DEBRUIJN_NETWORK
#define LONG_SYMMETRIC_DEBRUIJN_NETWORK

#include "SymmetricDeBruijnNetwork.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class LongSymmetricDeBruijnNetwork
 *
 * This is an extended version of the SymmetricDeBruijnNetwork,
 * it extends such a network symmetrically in the center of it,
 * adding columns, such that it would become a network of
 * a reverse DeBruijn and a DeBruijn in tandem, such that each
 * side is of a width that is of the form \f$ 2^m -1 \f$ for
 * the unique \f$m\f$ that satisfies \f$ 2^{m-1} -1 < k+1 \le 2^m -1 \f$.
 * In total, the length of such a network is \f$ 2(2^m -1)-1 \f$ (the central column is common)
 */

class LongSymmetricDeBruijnNetwork : public Deg2PermutationRoutingNet {
public:
    LongSymmetricDeBruijnNetwork(const layerID_t& k);

    layerID_t width() const {return 2*halfWidth_ - 1;} //The central column is common to both halfs
    labelID_t height() const {return baseNet_.height();}
    dataID_t getDataID(const layerID_t& l, const labelID_t& w)const;
    short routingBit(const layerID_t& l, const labelID_t& w)const;

    void rout(const permutation_t& permutation){baseNet_.rout(permutation);}

    ///
    /// The network can be seen as two inverse DeBruijn networks
    /// such that the last column of both are same,
    /// and the permutation it routs is exactly the mapping
    /// from a value in some label \f$ \omega \f$ in the first column of
    /// the first network, to the value in label \f$ \omega \f$ in
    /// the first column of the second label.
    /// The following function give this interpretation to the network.
 
    layerID_t getWingWidth()const {return halfWidth_;}   
    dataID_t getDataID(const short networkID, const layerID_t& l, const labelID_t& w)const;
    short routingBit(const short networkID, const layerID_t& l, const labelID_t& w)const;

private:
    SymmetricDeBruijnNetwork baseNet_;
    const labelID_t k_;
    const layerID_t halfWidth_;
};
    
} //namespace BairToAcsp
} //namespace libstark

#endif //LONG_SYMMETRIC_DEBRUIJN_NETWORK
