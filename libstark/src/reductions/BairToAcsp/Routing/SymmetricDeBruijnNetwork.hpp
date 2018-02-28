#ifndef SYMMETRIC_DEBRUIJN_NETWORK
#define SYMMETRIC_DEBRUIJN_NETWORK

#include "BenesNetwork.hpp"
#include "Deg2PermutationRoutingNet.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class SymmetricDeBruijnNetwork
 *
 * A merge between a DeBruijn network and Benes network.
 * Just as Benes network is a reversed Butterfly and a Butterfly in tandem,
 * a symmetric DeBruijn network is a reversed DeBruijn and a DeBruijn in tandem.
 * such a network is isomorphic to a Benes network, in particular it can rout
 * any permutation over a set of element (of power of 2 cardinality) on distinct paths.
 */

class SymmetricDeBruijnNetwork : public Deg2PermutationRoutingNet {
public:
    SymmetricDeBruijnNetwork(const layerID_t& k): Deg2PermutationRoutingNet(1<<k), benes_(k), k_(k){};

    layerID_t width() const {return benes_.width();}
    labelID_t height() const {return benes_.height();}
    dataID_t getDataID(const layerID_t& l, const labelID_t& w)const;
    short routingBit(const layerID_t& l, const labelID_t& w)const;

    void rout(const permutation_t& permutation);

private:
    BenesNetwork benes_;
    labelID_t k_;

    //The isomorphism from DeBruijn network to Butterfly network
    //to a give layer and label in the DeBruijn return the
    //label in the Butterfly that should be used (on same layer)
    //for same data
    labelID_t btrflyToDebruijn(const layerID_t& l, const labelID_t& w)const;
};

} //namespace BairToAcsp
} //namespace libstark

#endif // SYMMETRIC_DEBRUIJN_NETWORK
