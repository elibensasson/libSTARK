#include "spaces.hpp"

namespace libstark{
namespace BairToAcsp{

using Algebra::FieldElement;
using Algebra::zero;
using Algebra::elementsSet_t;
using Algebra::mapIntegerToFieldElement;
using Infrastructure::POW2;
using std::vector;

size_t spaces::offsetOfNetworkLayerId()const{
    return commonDef_.heightSpaceDimension() + commonDef_.widthSpaceDimension();
}

elementsSet_t spaces::getVanishingSpaceBasis()const{
    elementsSet_t basis;
    
    const size_t amountOfBasisElement = commonDef_.vanishingSpaceDimension();

    for(size_t i=0; i< amountOfBasisElement; i++){
        //the i'th element of the standard basis
        FieldElement basisElement = mapIntegerToFieldElement(i,1,1);

        //add to basis
        basis.insert(basisElement);
    }

    return basis;
}

elementsSet_t spaces::getNetworkLayersBasis()const{
    elementsSet_t basis;
    
    //construct the standard basis for the two layers space
    const size_t amountOfBasisElement = offsetOfNetworkLayerId();

    for(size_t i=0; i< amountOfBasisElement; i++){
        //the i'th element of the standard basis
        FieldElement basisElement = mapIntegerToFieldElement(i,1,1);

        //add to basis
        basis.insert(basisElement);
    }

    return basis;
}

//returns the basis of a column space
//as a subspace of a layer
elementsSet_t spaces::getNetworkColumnBasis()const{
    elementsSet_t basis;
    
    //construct the standard basis for the column linear space
    const size_t amountOfBasisElement = commonDef_.heightSpaceDimension();
    const size_t firstStandardBasisElement = 0;
    for(size_t i=0; i< amountOfBasisElement; i++){
        //the i'th element of the standard basis
        FieldElement basisElement = mapIntegerToFieldElement(firstStandardBasisElement+i,1,1);

        //add to basis
        basis.insert(basisElement);
    }

    return basis;
}

//returns the basis of half a column space
//as a subspace of a layer
//It spans only the parts where the vectorId has a 0 most significant bit
elementsSet_t spaces::getNetworkColumnAssignmentNoCarryBasis()const{
    elementsSet_t basis;
    
    //construct the standard basis for the half column linear space
    const size_t amountOfBasisElement = commonDef_.heightSpaceDimension();
    const size_t firstStandardBasisElement = 0;
    for(size_t i=0; i< amountOfBasisElement-1; i++){
        //the i'th element of the standard basis
        FieldElement basisElement = mapIntegerToFieldElement(firstStandardBasisElement+i,1,1);

        //add to basis
        basis.insert(basisElement);
    }

    return basis;
}

//indexes that are not included in a basis for DeBruijn routing checks
vector<size_t> spaces::getBitIndexesForDeBruijnNeighboursPairIndicator()const{

    const size_t amountOfIndicatorBits = 2;
    const size_t colId_lastBitLocation = commonDef_.heightSpaceDimension() + commonDef_.widthSpaceDimension() - 1;
    const size_t rowId_lastBitLocation = commonDef_.heightSpaceDimension() - 1;

    
    vector<size_t> bitsIndicator(amountOfIndicatorBits);
    bitsIndicator[0] = colId_lastBitLocation;
    bitsIndicator[1] = rowId_lastBitLocation;

    return bitsIndicator;
}

//Returns the basis of a subspace of a layer space
//Such that all the vertexes in the DeBruijn network
//in that space has the same 2 neighbours
elementsSet_t spaces::getDeBruijnNeighborsPairSpaceBasis()const{
    elementsSet_t basis;
    
    //construct the subset of the layers basis that does not include
    //the basis elements relevant for the neighbor pairs indicator
    const size_t amountOfLayerBasisElement = offsetOfNetworkLayerId();
    
    //'i's that should not be included
    vector<size_t> notIncludedIndexes = getBitIndexesForDeBruijnNeighboursPairIndicator();

    for(size_t i=0; i< amountOfLayerBasisElement; i++){
        
        //check if 'i' should be not included in the basis
        if(count(notIncludedIndexes.begin(),notIncludedIndexes.end(),i) > 0)continue;
        
        //the i'th element of the standard basis
        FieldElement basisElement = mapIntegerToFieldElement(i,1,1);

        //add to basis
        basis.insert(basisElement);
    }

    return basis;
}

//calculate shifts for DeBruijn routing test
vector<FieldElement> spaces::getDeBruijnLayerCosets()const{
    const vector<size_t> cosetIndicatorBits = getBitIndexesForDeBruijnNeighboursPairIndicator();
    const size_t amountOfCosets = POW2(cosetIndicatorBits.size());
    vector<FieldElement> DeBruijnShifts(amountOfCosets);
    for(size_t cosetId=0; cosetId < amountOfCosets; cosetId++){

        //construct an element in the coset
        FieldElement cosetRepresentor = zero();
        {
            size_t currCosetId = cosetId;
            for(const size_t bitIndex : cosetIndicatorBits){
                size_t currBitVal = currCosetId&1;
                if(currBitVal == 1){
                    cosetRepresentor += mapIntegerToFieldElement(bitIndex,1,1);
                }
                currCosetId>>=1;
            }

        }
        DeBruijnShifts[cosetId] = cosetRepresentor;
    }

    return DeBruijnShifts;
}


} //namespace BairToAcsp
} //namespace libstark
