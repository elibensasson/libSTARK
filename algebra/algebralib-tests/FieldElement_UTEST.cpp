/**
 *       @file  FieldElement_UTEST.cpp
 *      @brief  A file for unit testing FieldElement class
 *      Unit tests use GTEST library
 *      This file can be linked to external main function 
 *      (for example for wide system unit-testing)
 *      Or it can be compiled to an executable by defining the macro
 *	FIELD_ELEMENT_UTEST_MAIN.
 *	
 *	It expected to be linked with an implementation for
 *	FieldElement class.
 *
 *     @author  Michael Riabzev, RiabzevMichael@gmail.com
 * =====================================================================================
 */

#include "algebraLib/FieldElement.hpp"
#include <gtest/gtest.h>

namespace{

using Algebra::FieldElement;
using Algebra::zero;
using Algebra::one;
using Algebra::elementsSet_t;
using Algebra::generateRandom;
using Algebra::getStandartBasis;
using Algebra::getSpaceElementByIndex;
using Algebra::getSpaceIndexOfElement;
using Algebra::invertPointwise;
using std::vector;

/**
 * A function to test that field elements comparison
 * function (for stl collections usage)
 * works
 */
TEST(Algebra,elementComparison){
	//Generate roots
	elementsSet_t elements;
	for (int i=0; i<100; i++){
		FieldElement e = Algebra::generateRandom();
		elements.insert(e);
	}
	for(elementsSet_t::iterator e=elements.begin(); e!=elements.end(); e++){
		const FieldElement x = Algebra::generateRandom();
		FieldElement e1 = *e + x;
		EXPECT_GT(elements.count(e1-x),0);
	}
	for(int i=0 ;i<200; i++){
		const FieldElement x = Algebra::generateRandom();		

		if(elements.count(x) > 0){ //is in set
			bool found = false;
			for(elementsSet_t::iterator e=elements.begin(); e!=elements.end(); e++){
				if (x == *e){
					found = true;
					break;
				}
			}
			EXPECT_EQ(found,true);
		}
		else { //is not in set
			for(elementsSet_t::iterator e=elements.begin(); e!=elements.end(); e++){
				EXPECT_NE(*e,x);
			}
		}
	}
}

TEST(Algebra,spacesIndexing){
    const short basisSize = 1+ (rand()%10);
    const auto basis = getStandartBasis(10);
    const vector<FieldElement> orderedBasis(basis.begin(),basis.end());
    const FieldElement affineShift = generateRandom();

    const size_t spaceSize = (1<<basisSize);
    for(size_t i=0; i<spaceSize; i++){
        const FieldElement e = getSpaceElementByIndex(orderedBasis,affineShift,i);
        const size_t index = getSpaceIndexOfElement(orderedBasis,affineShift,e);
        EXPECT_EQ(i,index);
    }
}
/**
 * A function to test that field elements inversion of a bunch of elements
 * function works
 */
TEST(Algebra,elementInvertVector){
	const size_t numElemsBound = 1000;

    //Generate roots
	vector<FieldElement> elements;
	for (unsigned int i=0; i<numElemsBound; i++){
		FieldElement e = Algebra::generateRandom();
		if(e != zero()){
            elements.push_back(e);
        }
	}

    const auto inv_vals = invertPointwise(elements);

    for(size_t i=0; i< elements.size(); i++){
        EXPECT_EQ(one(),elements[i]*inv_vals[i]);
    }
}

} //anonymous namespace
