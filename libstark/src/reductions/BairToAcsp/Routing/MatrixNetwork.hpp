/**
 *       @file  MatrixNetwork.hpp
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

#ifndef MATRIX_NETWORK_HPP__
#define MATRIX_NETWORK_HPP__

#include "RoutingNetwork.hpp"

#include <vector>
#include <cassert>

namespace libstark{
namespace BairToAcsp{
/**
 * @class matrixNetwork
 * @brief A trivial implementation to RoutingNetwork interface, using a matrix with no constraints
 *
 * This class is for internal use of the routing networks library only, and should not be exposed as some interface outside,
 * because it is made with poor interface, no encapsulation, and state may be non consistent 
 * (the dimensions may be changed independently to the 'data' vector). One who uses it must be responsible
 * for keeping the state consistent.
 */
class matrixNetwork : public RoutingNetwork{
public:
	std::vector<std::vector<dataID_t>> data;
	layerID_t numLayers;
	layerID_t numNodes;

	layerID_t width()const{
		return numLayers;
	}
	
	labelID_t height()const{
		return numNodes;
	}
	
	dataID_t getDataID(const layerID_t& l, const labelID_t& w)const{
        assert(l >= 0);
        assert(l < data.size());
        assert(w >= 0);
        assert(w < data[l].size());
        return data[l][w];
	}
	
	void setSize(const layerID_t& layersAmount, const labelID_t& labelsAmount){
		numLayers = layersAmount;
		numNodes = labelsAmount;
		data.resize(numLayers);
		for (auto& layer : data){
			layer.resize(numNodes);
		}
	}
};
} //namespace BairToAcsp
} //namespace libstark

#endif //MATRIX_NETWORK_HPP__
