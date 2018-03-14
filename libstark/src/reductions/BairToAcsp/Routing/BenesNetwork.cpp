/**
 *       @file  BenesNetwork.cpp
 *      @brief  This file implements algorithms needed for BenesNetwork construction
 *
 * # Claim #
 * For any any given permutation \f$ \pi: \{0,1,\dots,2^k -1\} \to \{0,1,\dots,2^k -1\} \f$,
 * there exists a Benes network of degree \f$ k \f$ that routs the data packets \f$ \{0,1,\dots,2^k -1\} \f$ using distinct
 * nodes, such that a data packet \f$ d \f$ is in the last layer in the node labeled \f$ \omega \f$
 * if and only if the data packet \f$ \pi(d) \f$ is located in the first layer at the node labeled \f$ \omega \f$
 *
 * # Proof #
 * The proof is constructive, and well explained at 
 * http://pages.cs.wisc.edu/~tvrdik/10/html/Section10.html
 * (under off-line permutation routing)
 *
 *
 *     @author  Michael Riabzev (), RiabzevMichael@gmail.com
 *
 *   @internal
 *     Created  11/27/2013
 *     Company  SCIPR-LAB
 *   Copyright  Copyright (c) 2013, SCIPR-LAB
 * =====================================================================================
 */

#include "BenesNetwork.hpp"
#include "MatrixNetwork.hpp"
#include "common/Infrastructure/Infrastructure.hpp"

#include <vector>
#include <set>
#include <algorithm>

namespace libstark{
namespace BairToAcsp{

using std::vector;
using std::move;
using std::set;
using std::unique_ptr;
using std::find;

typedef RoutingNetwork::layerID_t layerID_t;
typedef RoutingNetwork::labelID_t labelID_t;
typedef RoutingNetwork::dataID_t dataID_t;
typedef TandemNetwork::netsVec_t netsVec_t;

/**
 * @brief   A recursive function that routs a permutation, that is defined by its first and last layer, using Benes routing. This function is an implementation of the algorithm given in the proof to the claim in the file header.
 * @param   revBtrfly the reverse butterfly network part of the full Benes network (not only the one specific to this recursion step)
 * @param   btrfly  the butterfly network part of the full Benes network (not only the one specific to this recursion step)
 * @param current_k the degree of the Benes network relevant to the current recursion step.
 * @param offset the offset of the Benes network that is routed in the current recursion state, relatively to the first node in each layer in the full Benes network. For example, when routing a Benes network of degree \f$ k \f$ we have at the first step offset=0, when it calls to itself again, it calls to itself once with offset=0, and another time with offset=\f$\frac{2^k}{2} = 2^{k-1}\f$
 */
void static constructBenes_recursive(matrixNetwork& revBtrfly, matrixNetwork& btrfly, const layerID_t& current_k, const labelID_t& offset){

	//constants relevant for current layer
	const dataID_t numLabels = 1<<current_k;
	const layerID_t currLayer = (revBtrfly.width()-1) - current_k;
	
	const vector<dataID_t>& firstLayer = revBtrfly.data[currLayer];
	vector<dataID_t>& secondLayer = revBtrfly.data[currLayer+1];
	const vector<dataID_t>& lastLayer = btrfly.data[current_k];
	vector<dataID_t>& preLastLayer = btrfly.data[current_k-1];
	
	const layerID_t mask = 1<<(current_k-1);

	//Recursion stopping condition
	if (numLabels == 2){
			secondLayer[offset] = firstLayer[offset];
			secondLayer[offset+1] = firstLayer[offset+1];
			preLastLayer[offset] = lastLayer[offset];
			preLastLayer[offset+1] = lastLayer[offset+1];
            return;
	}

	set<labelID_t> unrouted;
	for (dataID_t i=0; i < numLabels; i++){
		unrouted.insert(i);
	}
	
	
	while (!unrouted.empty()){
		
		//get source port to rout (not yet routed)
		auto src_indx = offset + *(unrouted.begin());

		while(unrouted.count(src_indx - offset) > 0){
			const dataID_t currData = firstLayer[src_indx];
			unrouted.erase(src_indx - offset);
		
			//route the source port to the "upper" network (clear bit)
			{
			labelID_t src_nxt = src_indx & ~mask;
			secondLayer[src_nxt] = currData;
			}
			
			//find destination of current source port to rout
			auto dst_indx = find(lastLayer.begin(), lastLayer.end(),currData) - lastLayer.begin();
			
			//route the destination port also to the "upper" network (clear bit)
			{
			labelID_t dst_nxt = dst_indx & ~mask;
			preLastLayer[dst_nxt] = currData;
			}

			//find the twin of the output port
			auto dst_twin = dst_indx ^ mask;
			const dataID_t twinData = lastLayer[dst_twin];
			
			//route the twin of the destination port to the "lower" network (set bit)
			{
			labelID_t twin_nxt = dst_indx | mask;
			preLastLayer[twin_nxt] = twinData;
			}

			//find the source of the output port twin
			auto src_twin = find(firstLayer.begin(), firstLayer.end(),twinData) - firstLayer.begin();
			
			//route the source of the output twin port also to the "lower" network (set bit)
			{
			labelID_t twin_nxt = src_twin | mask;
			secondLayer[twin_nxt] = twinData;
			
			//mark as routed by removing from "unrouted" set
			unrouted.erase(src_twin - offset);
			}
			
			//find twin of the source recently routed, and rout it
			src_indx = src_twin ^ mask;
		}
	}
	constructBenes_recursive(revBtrfly, btrfly,current_k-1, offset);
	constructBenes_recursive(revBtrfly, btrfly,current_k-1, offset + numLabels/2);
}
	
void BenesNetwork::rout(const permutation_t& permutation){
	if(!isPermutation(permutation)){
		_COMMON_FATAL("Can't rout given sequence, it is not a permutation");
	}

	data_.reset(constructBenes(permutation));

	isRouted = true;
}

dataID_t BenesNetwork::getDataID(const layerID_t& l, const labelID_t& w) const{
	if (!isRouted){
		_COMMON_FATAL("Can't get data, network not routed");
	}
	return data_.getDataID(l,w);
}

/**
 * @brief   constructBenes
 * @param   k degree of Benes network to construct
 * @param   permutation a sequence of integers that is assumed to be a permutation when restricted to \f$ 2^k \f$ first elements
 * @return  A vector of 2 routing networks, the first is a reverse butterfly, and the second is a butterfly, and when concatenated they form a Benes vector that routs the given permutation
 */
netsVec_t BenesNetwork::constructBenes(const permutation_t& permutation){
	const labelID_t labelsAmount = 1<<k_;
	
	unique_ptr<matrixNetwork> reverseBtrfly(new matrixNetwork()), btrfly(new matrixNetwork());
	reverseBtrfly->setSize(k_+1,labelsAmount);
	btrfly->setSize(k_+1,labelsAmount);

	for (labelID_t i = 0; i< labelsAmount; i++){
		reverseBtrfly->data[0][i] = i ;
		btrfly->data[k_][i] = permutation.getElementByIndex(i);
	}
	constructBenes_recursive(*(reverseBtrfly.get()),*(btrfly.get()),k_,0);
	
	netsVec_t res;
	res.push_back(move(reverseBtrfly));	
	res.push_back(move(btrfly));
	return res;
}

} //namespace BairToAcsp
} //namespace libstark
