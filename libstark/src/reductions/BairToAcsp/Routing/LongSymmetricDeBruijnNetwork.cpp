#include "LongSymmetricDeBruijnNetwork.hpp"
#include "common/Infrastructure/Infrastructure.hpp"

#include <cassert>

using Infrastructure::POW2;
using Infrastructure::CEIL;
using Infrastructure::Log2;

namespace libstark{
namespace BairToAcsp{
    
typedef RoutingNetwork::layerID_t layerID_t;
typedef RoutingNetwork::labelID_t labelID_t;
typedef RoutingNetwork::dataID_t dataID_t;

LongSymmetricDeBruijnNetwork::LongSymmetricDeBruijnNetwork(const layerID_t& k):
    Deg2PermutationRoutingNet(1<<k),
    baseNet_(k),
    k_(k),
    halfWidth_(POW2(CEIL(Log2(k+2)))-1)
    {};

dataID_t LongSymmetricDeBruijnNetwork::getDataID(const layerID_t& l, const labelID_t& w)const{
    
    if (l <= k_){
        //first half, no extension
        return baseNet_.getDataID(l,w);
    } 
    
    if ( (width()-1) - l <= k_){
        //second half, no extension
        const layerID_t baseLayer = baseNet_.width() - (width()-l);
        return baseNet_.getDataID(baseLayer,w);
    }

    //else : simulate constant routing to "first" neighbor (no XOR)
	
    const labelID_t mask = (1<<k_)-1; // the mask is least significant $k$ bits set
    labelID_t baseLabel;

    if ( l < halfWidth_){
        //first half, extension
        //rout as reverse DeBruijn
        const size_t shift_size = l-k_;
        baseLabel = ((w>>shift_size) | (w<<(k_-shift_size))) & mask;
    }
    else{
        //second half, extension
        //rout as straight DeBruijn
        const size_t shift_size = width()-(k_+1)-l;
        baseLabel = ((w>>shift_size) | (w<<(k_-shift_size))) & mask;
    }

    return baseNet_.getDataID(k_,baseLabel);
}
    
dataID_t LongSymmetricDeBruijnNetwork::getDataID(const short networkID, const layerID_t& l, const labelID_t& w)const{
    assert( l < getWingWidth());
    switch(networkID){
        case 0: return getDataID(l,w);
        case 1: return getDataID((width()-1)-l,w);
        default: _COMMON_FATAL("Unsupported network ID");
    }
}

short LongSymmetricDeBruijnNetwork::routingBit(const layerID_t& l, const labelID_t& w) const{
    
    const labelID_t mask = (1<<k_)-1;
    
    labelID_t neighbor1_label;
    labelID_t neighbor2_label;
	
    if( l < halfWidth_-1){
        //first half - reverse DeBruijn
        neighbor1_label = ((w<<1) | (w>>(k_-1))) & mask;
        const auto shifted1 = ((1<<1) | (1>>(k_-1))) & mask;
        neighbor2_label = neighbor1_label ^ shifted1;
    }
    else{
        //second half - straight DeBruijn
        neighbor1_label = ((w>>1) | (w<<(k_-1))) & mask;
        neighbor2_label = neighbor1_label ^ 1;
    }
    		
    return getDataID(l,w) == getDataID(l+1,neighbor2_label);
}

short LongSymmetricDeBruijnNetwork::routingBit(const short networkID, const layerID_t& l, const labelID_t& w) const{
    
    const labelID_t mask = (1<<k_)-1;

    const auto w1 = w^1;
    const auto neighbor2_label = ((w1<<1) | (w1>>(k_-1))) & mask;
			
    return getDataID(networkID,l,w) == getDataID(networkID,l+1,neighbor2_label);
}

} //namespace BairToAcsp
} //namespace libstark
