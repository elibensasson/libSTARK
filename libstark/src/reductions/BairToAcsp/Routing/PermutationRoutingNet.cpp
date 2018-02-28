/**
 *       @file  PermutationRoutingNet.cpp
 *      @brief  
 *
 * Detailed description starts here.
 *
 *     @author  Michael Riabzev (), RiabzevMichael@gmail.com
 *
 *   @internal
 *     Created  12/04/2013
 *     Company  SCIPR-LAB
 *   Copyright  Copyright (c) 2013, SCIPR-LAB
 * =====================================================================================
 */

#include "PermutationRoutingNet.hpp"

#include <vector>

using std::vector;

namespace libstark{
namespace BairToAcsp{

bool PermutationRoutingNet::isPermutation(const permutation_t& permutation)const{
	//We check the mapping is a bijection (permutation)
	//by checking that it is injective
	
	//inImage[i] will be set to 'true' iff
	//there exists j that is mapped to it
	vector<bool> inImage(numElements_);
	
	//initialization
	for(labelID_t i=0; i< numElements_; i++) inImage[i] = false;

	//Check if is an injection
	for(labelID_t i=0; i< numElements_; i++){
		dataID_t img = permutation.getElementByIndex(i);

		//check the image is inside {0 ... numElements-1}
		if ((img < 0) || (img >= numElements_)) return false;

		//check no element was mapped to img before (validate injectivity)
		if (inImage[img]) return false;

		//mark in image
		inImage[img] = true;
	}
	
	return true;
}

} //namespace BairToAcsp
} //namespace libstark
