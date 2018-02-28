#include "algebraLib/FieldElement.hpp"
#include "algebraLib/ErrorHandling.hpp"
#include "algebraLib/SubspacePolynomial.hpp"
#include <iostream>
#include <string>
#include <sstream> 
#include <cstdint>
#include <stack>

namespace Algebra {
	using std::vector;
	std::mt19937_64 rng;


	std::ostream& operator<<(std::ostream& s, const FieldElement& a)
	{
		return s << a.asString();
	}

	std::string FieldElement::asString() const{
		::std::stringstream s;
		s << "[";
        size_t elem=element_.c[0];
        bool first = true;
        while(elem != 0){
            if(!first){
                s<<" ";
            }
            s<<(elem & 1);
            elem>>=1;

            first = false;
        }
        s <<"]";
		return s.str();
	}
	

elementsSet_t getStandartBasis(const unsigned short basisSize){
    elementsSet_t basis;
    for(unsigned short i=0; i< basisSize; i++){
        basis.insert(mapIntegerToFieldElement(i,1,1));
    }
    return basis;
}

vector<FieldElement> getStandartOrderedBasis(const unsigned short basisSize){
    vector<FieldElement> basis;
    for(unsigned short i=0; i< basisSize; i++){
        basis.push_back(mapIntegerToFieldElement(i,1,1));
    }
    return basis;
}


FieldElement generateRandom(){
	//return mapIntegerToFieldElement(0,64,rand());
	//return mapIntegerToFieldElement(0, 64, ((unsigned long long)rand() << 32) + rand());
	return mapIntegerToFieldElement(0,64,rng());
}

FieldElement getSpaceElementByIndex(const std::vector<FieldElement>& orderedBasis, const FieldElement& affineShift, const size_t index){
    size_t iter = index;
    FieldElement res = zero();

    for(const auto& b : orderedBasis){
        if(iter % 2 == 1) res += b;
        iter >>= 1;
    }

    return affineShift + res;
}

size_t getSpaceIndexOfElement(const std::vector<FieldElement>& orderedBasis, const FieldElement& affineShift, const FieldElement& e){
    FieldElement elemInSpan = e - affineShift;
    
    vector<FieldElement> partialBasis = orderedBasis;
    const auto firstElemLoc = orderedBasis.begin();
    auto lastElemLoc = orderedBasis.end();

    //first check "e" really is in space
    {
        SubspacePolynomial spacePoly(elementsSet_t(firstElemLoc,lastElemLoc));
        if (spacePoly.eval(elemInSpan) != zero()){
            ALGEBRALIB_FATAL("element is not in spanned space");
        }
    }
    
    //now check for every basis element if
    //elemInSpace is in the space spanned by the compliment basis set
    //(using the space polynomial)
    //if so, the coefficient of it in the representation is zero
    //otherwise is one

    size_t elementIndex = 0;
    lastElemLoc--;
    for(;firstElemLoc <= lastElemLoc; lastElemLoc--){
        elementIndex<<=1;
        
        SubspacePolynomial spacePoly(elementsSet_t(firstElemLoc,lastElemLoc));
        if (spacePoly.eval(elemInSpan) != zero()){
            //add to index
            elementIndex+=1;
            
            //take the element projection on the subspace
            elemInSpan -= *lastElemLoc;
        }

		if (firstElemLoc == lastElemLoc){
			break;
		}
    }

    return elementIndex;
}

vector<FieldElement> invertPointwise(const vector<FieldElement>& elems){
    
    if(elems.size() == 0){
        return elems;
    }

    std::stack<size_t> layersEnd;
    
    vector<FieldElement> treeOfElems(elems);
    layersEnd.push(treeOfElems.size());
    size_t currLocation = 0;

    
    //this loop builds a binary tree
    //the leafs hold the elements to invert
    //and each inner node holds the product of both its sons.
    //In the case the number of elements in some depth is odd,
    //one of the elements (the last one) is a single child of its
    //father in the higher layer
    while(true){
        const size_t currLayerEnd = layersEnd.top();
        while(currLocation < currLayerEnd){
            
            //handling the case of a single child
            //because current layer contains odd number of nodes
            if(currLocation +1 == currLayerEnd){
                treeOfElems.push_back(treeOfElems[currLocation]);
                currLocation+=1;
            }
            
            //handles the "ordinary case"
            //where two brothers are multiplied to get their fathers
            //value
            else{
                treeOfElems.push_back(treeOfElems[currLocation]*treeOfElems[currLocation+1]);
                currLocation+=2;
            }
        }

        //end when the next layer contains a single element
        //it is the root
        if(currLayerEnd+1 == treeOfElems.size()){
            break;
        }

        layersEnd.push(treeOfElems.size());
    }

    //invert the root (the product of all elements
    treeOfElems[treeOfElems.size()-1] = treeOfElems[treeOfElems.size()-1].inverse();

    //calculated the inverse of each subtree node
    while(!layersEnd.empty()){
        const size_t currLayerEnd = layersEnd.top();
        layersEnd.pop();
        size_t currLayerStart = 0;
        if(!layersEnd.empty()){
            currLayerStart = layersEnd.top();
        }

        size_t parentIndex = currLayerEnd;
        size_t childIndex = currLayerStart;
        while (childIndex < currLayerEnd){
            //handling the case of a single child
            //because current layer contains odd number of nodes
            if(childIndex +1 == currLayerEnd){
                treeOfElems[childIndex] =treeOfElems[parentIndex];
                childIndex+=1;
                //no need to advance parentIndex because this is the last child in the layer
            }
            
            //handles the "ordinary case"
            //where each of the two children's inverse is the product of the parent and the brother
            else{
                const FieldElement thisChildVal = treeOfElems[parentIndex]*treeOfElems[childIndex+1];
                const FieldElement brothersVal = treeOfElems[parentIndex]*treeOfElems[childIndex];
                treeOfElems[childIndex] = thisChildVal;
                treeOfElems[childIndex+1] = brothersVal;
                childIndex+=2;
                parentIndex+=1;
            }
        }
    }

   treeOfElems.resize(elems.size());
   return treeOfElems; 
}
 

}// namespace Algebra
