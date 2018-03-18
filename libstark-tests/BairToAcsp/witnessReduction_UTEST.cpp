#include "BairWitnessChecker_UTEST.hpp"
#include <reductions/BairToAcsp/Details/witnessReduction.hpp>
#include <reductions/BairToAcsp/Routing/LongSymmetricDeBruijnNetwork.hpp>

#include <gtest/gtest.h>

using libstark::BairToAcsp::witnessReduction;
using libstark::BairToAcsp::common;
using libstark::BairToAcsp::commonMappings;
using libstark::BairToAcsp::witnessMappings;
using libstark::BairToAcsp::LongSymmetricDeBruijnNetwork;
using libstark::BairWitness;
using libstark::BairInstance;
using libstark::AcspWitness;

using Infrastructure::POW2;

using Algebra::FieldElement;
using Algebra::zero;
using Algebra::one;

using std::pair;
using std::vector;

namespace{    

typedef pair<BairInstance,BairWitness> BairPair;

FieldElement advanceToNextRow(const FieldElement src, const common& commonInfo){
    
    //Row global properties
    const int64_t rowOverflowMask = 1UL<<commonInfo.heightSpaceDimension();
    const int64_t rowMask = rowOverflowMask - 1;
    
    //initialize the polynomial containing current location
    const int64_t srcLocation = mapFieldElementToInteger(0,Algebra::ExtensionDegree,src);

    //initialize a polynomial for the row location
    const int64_t srcRow = srcLocation & rowMask;

    //initialize the next row location polynomial
    int64_t dstRow = srcRow<<1;
    if(dstRow & rowOverflowMask){
        dstRow ^= commonInfo.rowsModulus();
    }

    //the destination location is exactly swapping the row location
    const int64_t dstLocation = (srcLocation ^ srcRow) ^ dstRow;

    //return result
    return Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,dstLocation);
}

void verifyIntegrity(const BairPair& bair_pair, const AcspWitness& acsp_witness){

    //get common information
    const common commonDef(bair_pair.first,vector<FieldElement>(), vector<FieldElement>());
    const witnessMappings witnessMapping(commonDef);
    const commonMappings& commonMapping = witnessMapping;

    //get the Bair pair
    const BairInstance& instance = bair_pair.first;
    const BairWitness& witness = bair_pair.second;
    
    // Bair permutation
    const auto& permutation = witness.permutation();

    //get amount of rows
    const size_t cyclicDomainSize = instance.domainSize();
    
    //get variables partition
    const vector<size_t> unroutedVars = commonDef.variablesNonPerm();
    const vector<size_t> routedVars = commonDef.variablesPerm();

    {
        //verify elements for circle
        size_t vecId = 0;
        FieldElement indicator = witnessMapping.mapNonPermutationElement_witness(vecId,0).second - commonMapping.mapNonPermutationElement_witness(0).second;
        for(unsigned int i=0; i<cyclicDomainSize; i++){
            const auto& coloring = witness.get_color(vecId);
            const auto& permColoring = witness.get_color(permutation.getElementByIndex(vecId));

            //verify assignment on variables not in permutation
            for(size_t varLocation = 0; varLocation < unroutedVars.size(); varLocation++){
                const witnessMappings::witnessElement_t currDelta = commonMapping.mapNonPermutationElement_witness(varLocation);

                //get the Acsp witness
                const auto& acspPoly = *(acsp_witness.assignmentPolys()[currDelta.first]); 
    
                const FieldElement expectedVal = coloring[unroutedVars[varLocation]];
                const FieldElement val = acspPoly.eval(indicator + currDelta.second);

                EXPECT_EQ(expectedVal,val);
            }

            //verify assignment & permutation on variables in permutation
            for(size_t varLocation = 0; varLocation < routedVars.size(); varLocation++){

                //verify assignment
                {
                const commonMappings::witnessElement_t currDelta = commonMapping.mapPermutationElement_witnessElement(0,2*varLocation);

                //get the Acsp witness
                const auto& acspPoly = *(acsp_witness.assignmentPolys()[currDelta.first]); 

                const FieldElement expectedVal = coloring[routedVars[varLocation]];
                const FieldElement val = acspPoly.eval(indicator + currDelta.second);

                EXPECT_EQ(expectedVal,val);
                }

                //verify permutation
                {
                const commonMappings::witnessElement_t currDelta = commonMapping.mapPermutationElement_witnessElement(0,2*varLocation+1);

                //get the Acsp witness
                const auto& acspPoly = *(acsp_witness.assignmentPolys()[currDelta.first]); 

                const FieldElement expectedVal = permColoring[routedVars[varLocation]];
                const FieldElement val = acspPoly.eval(indicator + currDelta.second);

                EXPECT_EQ(expectedVal,val);
                }
            }
            
            
            indicator = advanceToNextRow(indicator,commonDef);
            vecId = (vecId+1) % cyclicDomainSize;
        }
    }
}

size_t cyclicShift(const size_t src, const char bitsAmount){
    
    //get mask for cyclic shift
    const size_t mask = POW2(bitsAmount) - 1;

    return ((src<<1)&mask) ^ (((src<<1)&(~mask)) >> bitsAmount);


}

void verifyRoutingNetwork(const BairPair& bair_pair, const AcspWitness& acsp_witness){

    //get common information
    common commonDef(bair_pair.first,vector<FieldElement>(), vector<FieldElement>());
    witnessMappings witnessMapping(commonDef);

    //get the Bair pair
    const BairInstance& instance = bair_pair.first;

    //get amount of layers
    const size_t layersAmount = commonDef.variablesPerm().size()*2;
    
    //get amount of rows
    const size_t amountOfRows = instance.domainSize()+1;
    
    //get columns amount
    const size_t columnsAmount = LongSymmetricDeBruijnNetwork(instance.domainSizeIndicator()).getWingWidth();

    //get index of last column
    const size_t lastColumn = columnsAmount-1;

    //bits for row Id
    const size_t bitsForRowId = commonDef.heightSpaceDimension();

    for(size_t layerId=0; layerId < layersAmount; layerId++){
        for(size_t rowId=0; rowId < amountOfRows; rowId++){
            for (size_t columnId=0; columnId < columnsAmount; columnId++){
                
                //get the location of current point
                const commonMappings::witnessElement_t currLocation = witnessMapping.map_spaceIndex_to_witnessElement(witnessMapping.mapNetworkElement_spaceIndex(rowId,columnId,layerId));

                //get the Acsp witness
                const auto& acspPoly = *(acsp_witness.assignmentPolys()[currLocation.first]); 

                //get the data of current point
                const FieldElement currData = acspPoly.eval(currLocation.second);

                //if current column is not the last column, verify DeBruijn routing
                if(columnId != lastColumn){
                    
                    //get neighbors row ids
                    const size_t DB_neighbor0_rowId = cyclicShift(rowId   , bitsForRowId);
                    //because this is a reverse DeBruijn, the xor with "1" must be done before the cyclic shift
                    const size_t DB_neighbor1_rowId = cyclicShift(rowId^1 , bitsForRowId);

                    //get the value of the routing bit
                    const commonMappings::witnessElement_t routingBitLocation = witnessMapping.map_spaceIndex_to_witnessElement(witnessMapping.mapNetworkRoutingBit_spaceIndex(rowId,columnId,layerId));

                    //get the Acsp witness
                    const auto& acspPoly_routingBits = *(acsp_witness.assignmentPolys()[routingBitLocation.first]); 

                    const FieldElement routingBitVal = acspPoly_routingBits.eval(routingBitLocation.second);

                    //verify routing bit is boolean
                    EXPECT_TRUE((routingBitVal == zero()) || (routingBitVal == one()));

                    //verify the routing
                    if (routingBitVal ==  zero()){
                        const auto neighborLocation = witnessMapping.map_spaceIndex_to_witnessElement(witnessMapping.mapNetworkElement_spaceIndex(DB_neighbor0_rowId,columnId+1,layerId));

                        //get the Acsp witness
                        const auto& acspPoly_N = *(acsp_witness.assignmentPolys()[neighborLocation.first]); 

                        const FieldElement neighborVal = acspPoly_N.eval(neighborLocation.second);
                        EXPECT_EQ(currData,neighborVal);
                    }
                    if (routingBitVal ==  one()){
                        const auto neighborLocation = witnessMapping.map_spaceIndex_to_witnessElement(witnessMapping.mapNetworkElement_spaceIndex(DB_neighbor1_rowId,columnId+1,layerId));

                        //get the Acsp witness
                        const auto& acspPoly_N = *(acsp_witness.assignmentPolys()[neighborLocation.first]); 

                        const FieldElement neighborVal = acspPoly_N.eval(neighborLocation.second);
                        EXPECT_EQ(currData,neighborVal);
                    }
                    
                }

                //if current column is the last column, verify identity with the twin column
                if(columnId == lastColumn){
                    
                    //get the location of twin point
                    const commonMappings::witnessElement_t twinLocation = witnessMapping.map_spaceIndex_to_witnessElement(witnessMapping.mapNetworkElement_spaceIndex(rowId,columnId,layerId^1));

                    //get the Acsp witness
                    const auto& acspPoly_twin = *(acsp_witness.assignmentPolys()[twinLocation.first]); 

                    //get the data of twin point
                    const FieldElement twinData = acspPoly_twin.eval(twinLocation.second);

                    //verify
                    EXPECT_NE(currLocation, twinLocation);
                    EXPECT_EQ(currData,twinData);
                }
            }
        }
    }
}


void verifyPermutation_aditionalElement(const BairPair& bair_pair, const AcspWitness& acsp_witness){

    //get common information
    common commonDef(bair_pair.first,vector<FieldElement>(), vector<FieldElement>());
    witnessMappings witnessMapping(commonDef);

    //get the Bair pair
    const BairInstance& instance = bair_pair.first;

    //get the variables in permutation
    const vector<size_t> routedVars = commonDef.variablesPerm();
    
    for(size_t varIndex=0; varIndex < routedVars.size(); varIndex++){
                
        //get the location of current point
        const auto currLocation = witnessMapping.map_spaceIndex_to_witnessElement(witnessMapping.mapNetworkElement_spaceIndex(0,0,2*varIndex));

        //get the Acsp witness
        const auto& acspPoly = *(acsp_witness.assignmentPolys()[currLocation.first]); 

        //get the data of current point
        const FieldElement currData = acspPoly.eval(currLocation.second);

        //get the location of twin point
        const auto twinLocation = witnessMapping.map_spaceIndex_to_witnessElement(witnessMapping.mapNetworkElement_spaceIndex(0,0,2*varIndex+1));

        //get the Acsp witness
        const auto& acspPoly_twin = *(acsp_witness.assignmentPolys()[twinLocation.first]); 

        //get the data of twin point
        const FieldElement twinData = acspPoly_twin.eval(twinLocation.second);

        //expected data
        const FieldElement expectedVal = instance.paddingPi()[routedVars[varIndex]];

        //verify
        EXPECT_NE(currLocation, twinLocation);
        EXPECT_EQ(currData,expectedVal);
        EXPECT_EQ(twinData,expectedVal);

    }
}


/******************************************
 *                UTESTS
 ******************************************/

TEST(BairToAcspWitness,integrityWithBairWitness){
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const auto witness = witnessReduction::reduceWitness(src.first,src.second);
    verifyIntegrity(src,*witness);
}

TEST(BairToAcspWitness,RoutingNetwork){
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const auto witness = witnessReduction::reduceWitness(src.first,src.second);
    verifyRoutingNetwork(src,*witness);
}

TEST(BairToAcspWitness,PermutationOnAdditionalElement){
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const auto witness = witnessReduction::reduceWitness(src.first,src.second);
    verifyPermutation_aditionalElement(src,*witness);
}

} //anonymous namespace
