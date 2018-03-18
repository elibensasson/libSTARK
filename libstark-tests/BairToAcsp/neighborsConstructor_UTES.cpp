#include "BairWitnessChecker_UTEST.hpp"
#include <reductions/BairToAcsp/Details/neighborsConstructor.hpp>
#include <reductions/BairToAcsp/Details/witnessMappings.hpp>
#include <common/Utils/TaskReporting.hpp>

#include <gtest/gtest.h>

using Infrastructure::POW2;

using Algebra::FieldElement;
using Algebra::generateRandom;
using Algebra::mapFieldElementToInteger;
using Algebra::PolynomialInterface;
using Algebra::UnivariatePolynomialInterface;

using libstark::BairWitness;
using libstark::BairInstance;
using libstark::ConstraintSys;

using libstark::BairToAcsp::common;
using libstark::BairToAcsp::instanceMappings;
using libstark::BairToAcsp::witnessMappings;
using libstark::BairToAcsp::AcspNeighbors;
using libstark::BairToAcsp::CS_testLocations;

using std::pair;
using std::unique_ptr;
using std::vector;


namespace{

    vector<FieldElement> genRandVector(const unsigned int len){
    vector<FieldElement> res(len);
    for(auto& e : res){
        e = generateRandom();
    }
    return res;
}

typedef pair<BairInstance,BairWitness> BairPair;

vector<pair<size_t,size_t>> constructNeighborsLocations(const vector<vector<unique_ptr<const UnivariatePolynomialInterface>>>& neighborsVec){
    
    //construct a mapping from neighbor id to (poly,layer)
    vector<pair<size_t,size_t>> neighborsInLayers;
    size_t currLayer = 0;
    for(const auto& nVec : neighborsVec){
        size_t currIdx = 0;
        for(unsigned int i=0; i<nVec.size(); i++){
            neighborsInLayers.push_back(pair<size_t,size_t>(currLayer,currIdx));
            currIdx++;
        }
        currLayer++;
    }

    return neighborsInLayers;
}

void verifyID(const BairPair& bair_pair, const AcspNeighbors& neighbors){
    
    const common defs(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
    const witnessMappings mappings(defs);

    //some constants
    const size_t layersAmount = (defs.variablesPerm().size()+1)*2;
    const size_t amountOfRows = defs.imageHeight();
    const size_t columnsAmount = defs.imageWidth()-1;

    //get the neighbors polynomials vector
    const auto neighborsVec = neighbors.getNeighborPolynomials();

    //construct a mapping from neighbor id to (poly,layer)
    vector<pair<size_t,size_t>> neighborsInLayers = constructNeighborsLocations(neighborsVec);

    //verify neighbors by testing on all network locations
    for(size_t layerId=0; layerId < layersAmount; layerId++){
        
        const auto curr_loc = neighborsInLayers[neighbors.locationOfId(layerId)];
        const auto& idPoly = *(neighborsVec[curr_loc.first][curr_loc.second]);
        
        for(size_t rowId=0; rowId < amountOfRows; rowId++){
            for (size_t columnId=0; columnId < columnsAmount; columnId++){

                //get the current location
                const FieldElement currLocation = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId,columnId,layerId));

                //get expected locations
                const FieldElement ref = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId,columnId,0));
                //get results from neighbor polynomials
                const FieldElement res = idPoly.eval(currLocation);

                EXPECT_EQ(ref,res);
                EXPECT_EQ(layerId,curr_loc.first);
            }
        }
    }
}

void verifyTwinLayer(const BairPair& bair_pair, const AcspNeighbors& neighbors){
    
    const common defs(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
    const witnessMappings mappings(defs);

    //some constants
    const size_t layersAmount = defs.variablesPerm().size()*2;
    const size_t amountOfRows = defs.imageHeight();
    const size_t columnsAmount = defs.imageWidth()-1;

    //get the neighbors polynomials vector
    const auto neighborsVec = neighbors.getNeighborPolynomials();

    //construct a mapping from neighbor id to (poly,layer)
    vector<pair<size_t,size_t>> neighborsInLayers = constructNeighborsLocations(neighborsVec);

    //verify neighbors by testing on all network locations
    for(size_t layerId=0; layerId < layersAmount; layerId++){
        
        const auto curr_loc = neighborsInLayers[neighbors.locationOfTwinLayer(layerId)];
        const auto& twinPoly = *(neighborsVec[curr_loc.first][curr_loc.second]);
        
        for(size_t rowId=0; rowId < amountOfRows; rowId++){
            for (size_t columnId=0; columnId < columnsAmount; columnId++){

                //get the current location
                const FieldElement currLocation = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId,columnId,layerId));

                //get expected locations
                const FieldElement ref = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId,columnId,0));
                //get results from neighbor polynomials
                const FieldElement res = twinPoly.eval(currLocation);

                EXPECT_EQ(ref,res);
                EXPECT_EQ(layerId^1,curr_loc.first);
            }
        }
    }
}

size_t cyclicShift(const size_t src, const char bitsAmount){
    
    //get mask for cyclic shift
    const size_t mask = POW2(bitsAmount) - 1;

    return ((src<<1)&mask) ^ (((src<<1)&(~mask)) >> bitsAmount);
}

void verifyDeBruijn(const BairPair& bair_pair, const AcspNeighbors& neighbors){
    
    const common defs(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
    const witnessMappings mappings(defs);

    //some constants
    const size_t bitsForRowId = defs.heightSpaceDimension();
    const size_t bitsForColumnId = defs.widthSpaceDimension();
    const size_t layersAmount = defs.variablesPerm().size()*2;
    const size_t amountOfRows = defs.imageHeight();
    const size_t columnsAmount = defs.imageWidth()-2;

    //get the neighbors polynomials vector
    const auto neighborsVec = neighbors.getNeighborPolynomials();

    //construct a mapping from neighbor id to (poly,layer)
    vector<pair<size_t,size_t>> neighborsInLayers = constructNeighborsLocations(neighborsVec);

    //verify neighbors by testing on all network locations except
    //the last column which is not routed using DeBruijn neighbors
    for(size_t layerId=0; layerId < layersAmount; layerId++){
        for(size_t rowId=0; rowId < amountOfRows; rowId++){
            for (size_t columnId=0; columnId < columnsAmount; columnId++){

                //get the DeBruijn neighbors for curr location
                const size_t rowId_N0 = cyclicShift(rowId   , bitsForRowId);
                //because this is a reverse DeBruijn, the xor with "1" must be done before the cyclic shift
                const size_t rowId_N1 = cyclicShift(rowId^1 , bitsForRowId);

                //get the current location
                const FieldElement currLocation = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId,columnId,layerId));

                //calculate the coset Id, so we can
                //find the relevant DeBruijn neighbor
                const size_t rowIdLastBitLocation = bitsForRowId - 1;
                const size_t columnIdLastBitLocation = bitsForRowId + bitsForColumnId - 1;
                const size_t rowLastBit = mapFieldElementToInteger(rowIdLastBitLocation,1,currLocation);
                const size_t columnLastBit = mapFieldElementToInteger(columnIdLastBitLocation,1,currLocation);
                const short cosetId = 2*rowLastBit + columnLastBit;

                //get the neighbor polynomials
                const auto N0_loc = neighborsInLayers[neighbors.locationOfDeBruijn(0,cosetId,layerId)];
                const auto N1_loc = neighborsInLayers[neighbors.locationOfDeBruijn(1,cosetId,layerId)];
                const auto& N0 = neighborsVec[N0_loc.first][N0_loc.second];
                const auto& N1 = neighborsVec[N1_loc.first][N1_loc.second];

                //get expected locations
                const FieldElement ref0 = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId_N0,columnId+1,0));
                const FieldElement ref1 = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId_N1,columnId+1,0));

                //get results from neighbor polynomials
                const FieldElement res0 = N0->eval(currLocation);
                const FieldElement res1 = N1->eval(currLocation);

                EXPECT_EQ(ref0,res0);
                EXPECT_EQ(ref1,res1);

                EXPECT_EQ(layerId,N0_loc.first);
                EXPECT_EQ(layerId,N1_loc.first);
            }
        }
    }
}

void verifyRoutingBit(const BairPair& bair_pair, const AcspNeighbors& neighbors){
    
    const common defs(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
    const witnessMappings mappings(defs);

    //some constants
    const size_t layersAmount = defs.variablesPerm().size()*2;
    const size_t amountOfRows = defs.imageHeight();
    const size_t columnsAmount = defs.imageWidth()-1;
    const size_t firstRoutingBitLayerId = layersAmount;

    //get the neighbors polynomials vector
    const auto neighborsVec = neighbors.getNeighborPolynomials();

    //construct a mapping from neighbor id to (poly,layer)
    vector<pair<size_t,size_t>> neighborsInLayers = constructNeighborsLocations(neighborsVec);

    //verify neighbors by testing on all network locations
    for(size_t layerId=0; layerId < layersAmount; layerId++){
        
        const auto curr_loc = neighborsInLayers[neighbors.locationOfRoutingBit(layerId)];
        const auto& routingBitAccessPoly = *(neighborsVec[curr_loc.first][curr_loc.second]);
        const size_t relevantRoutingBitsLayerId = firstRoutingBitLayerId + (layerId & 1);
        
        for(size_t rowId=0; rowId < amountOfRows; rowId++){
            for (size_t columnId=0; columnId < columnsAmount; columnId++){

                //get the current location
                const FieldElement currLocation = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId,columnId,layerId));

                //get expected locations
                const FieldElement ref = mappings.map_spaceIndex_to_fieldElement(mappings.mapNetworkElement_spaceIndex(rowId,columnId,0));
                //get results from neighbor polynomials
                const FieldElement res = routingBitAccessPoly.eval(currLocation);

                EXPECT_EQ(ref,res);
                EXPECT_EQ(relevantRoutingBitsLayerId,curr_loc.first);
            }
        }
    }
}

void verifyPermutationConstraints(const BairPair& bair_pair, const AcspNeighbors& neighbors, const CS_testLocations& testLocations){
    
    const BairInstance& partialInstance = bair_pair.first;
    const common defs(partialInstance,genRandVector(partialInstance.constraintsPermutation().numMappings()),genRandVector(partialInstance.constraintsAssignment().numMappings()));
    const witnessMappings mappings(defs);
    const instanceMappings& mappings_common(defs);

    //some constants
    const size_t rowsNum = defs.imageHeight();

    //get the neighbors polynomials vector
    const auto neighborsVec = neighbors.getNeighborPolynomials();

    //construct a mapping from neighbor id to (poly,layer)
    vector<pair<size_t,size_t>> neighborsInLayers = constructNeighborsLocations(neighborsVec);

    //verify neighbors by testing on all permutation constraints, and all rows
    for(size_t currPoly_indx=0; currPoly_indx < defs.getConstraintsPi().size(); currPoly_indx++){
        const PolynomialInterface* currPoly = defs.getConstraintsPi()[currPoly_indx].get();
        const size_t testIndex = testLocations.indexOfConstraint_Permuation(currPoly_indx);

        //neighbors for current row
        for (size_t varId=0; varId < partialInstance.vectorsLen(); varId++){
            
            //verify the neighbor for current variable exists if and only if
            //it is used by the polynomial
            EXPECT_EQ(neighbors.existsPermCS(currPoly_indx,varId), currPoly->isEffectiveInput(varId));

            //if neighbor does not exist, nothing else to do here
            if(!neighbors.existsPermCS(currPoly_indx,varId))continue;

            //else, get the neighbor
            const auto currNeighbor_loc = neighborsInLayers[neighbors.locationOfPermCS(currPoly_indx,varId)];
            const auto& currNeighbor = *(neighborsVec[currNeighbor_loc.first][currNeighbor_loc.second]);

            //verify the neighbor
            for(size_t rowId=0; rowId < rowsNum; rowId++){
                const FieldElement testLocation = mappings.mapNonPermutationElement(rowId, testIndex);
                const FieldElement rowIndicator = testLocation - mappings_common.mapNonPermutationElement(testIndex);
                const FieldElement res = currNeighbor.eval(testLocation);
                
                const auto varLocation = defs.getVarLocation(varId);
                if(varLocation.inPerm){
                    const auto ref_loc = mappings.mapPermutationElement_witnessElement(0,2*varLocation.index);
                    const FieldElement ref = rowIndicator + ref_loc.second;
                    EXPECT_EQ(ref,res);
                    EXPECT_EQ(ref_loc.first,currNeighbor_loc.first);
                }
                else{
                    const auto ref = mappings.mapNonPermutationElement_witness(rowId,varLocation.index);
                    EXPECT_EQ(ref.second,res);
                    EXPECT_EQ(ref.first,currNeighbor_loc.first);
                }

                
            }
        }

        //neighbors for next row (next using witness permutation)
        const size_t shift = partialInstance.vectorsLen();
        for (size_t varId=shift; varId < shift+partialInstance.vectorsLen(); varId++){

            //verify the neighbor for current variable exists if and only if
            //it is used by the polynomial
            EXPECT_EQ(neighbors.existsPermCS(currPoly_indx,varId), currPoly->isEffectiveInput(varId));

            //if neighbor does not exist, nothing else to do here
            if(!neighbors.existsPermCS(currPoly_indx,varId))continue;

            //else, get the neighbor
            const auto currNeighbor_loc = neighborsInLayers[neighbors.locationOfPermCS(currPoly_indx,varId)];
            const auto& currNeighbor = *(neighborsVec[currNeighbor_loc.first][currNeighbor_loc.second]);

            //verify the neighbor
            for(size_t rowId=0; rowId < rowsNum; rowId++){
                const FieldElement testLocation = mappings.mapNonPermutationElement(rowId, testIndex);
                const FieldElement rowIndicator = testLocation - mappings_common.mapNonPermutationElement(testIndex);
                const FieldElement res = currNeighbor.eval(testLocation);

                const auto varLocation = defs.getVarLocation(varId-shift);

                //A neighbor to a variable from next row in Permutation Constraints, must be to a permutation variable
                EXPECT_TRUE(varLocation.inPerm);

                const auto ref_loc = mappings.mapPermutationElement_witnessElement(0,2*varLocation.index + 1);
                const FieldElement ref = rowIndicator + ref_loc.second;
                EXPECT_EQ(ref,res);
                EXPECT_EQ(ref_loc.first,currNeighbor_loc.first);
            }
        }
    }
}

void verifyAssignmentConstraints(const BairPair& bair_pair, const AcspNeighbors& neighbors, const CS_testLocations& testLocations){
    
    const BairInstance& partialInstance = bair_pair.first;
    const common defs(partialInstance,genRandVector(partialInstance.constraintsPermutation().numMappings()),genRandVector(partialInstance.constraintsAssignment().numMappings()));
    const witnessMappings mappings(defs);
    const instanceMappings& mappings_common(defs);

    //some constants
    const size_t rowsNum = defs.imageHeight();

    //get the neighbors vec
    const auto neighborsVec = neighbors.getNeighborPolynomials();

    //construct a mapping from neighbor id to (poly,layer)
    vector<pair<size_t,size_t>> neighborsInLayers = constructNeighborsLocations(neighborsVec);

    //verify neighbors by testing on all permutation constraints, and all rows
    const size_t rowIdMSB_index = defs.heightSpaceDimension()-1;
    
    for(size_t currPoly_indx=0; currPoly_indx < defs.getConstraintsChi().size(); currPoly_indx++){
        const PolynomialInterface* currPoly = defs.getConstraintsChi()[currPoly_indx].get();
        const size_t testIndex = testLocations.indexOfConstraint_Assignment(currPoly_indx);

        //neighbors for current row
        for (size_t varId=0; varId < partialInstance.vectorsLen(); varId++){
            
            //verify the neighbor for current variable exists if and only if
            //it is used by the polynomial
            EXPECT_EQ(neighbors.existsAssignmentCS(currPoly_indx,varId), currPoly->isEffectiveInput(varId));

            //if neighbor does not exist, nothing else to do here
            if(!neighbors.existsAssignmentCS(currPoly_indx,varId))continue;

            //else, get the neighbor
            
            //verify both versions are the same polynomial
            EXPECT_EQ(neighbors.locationOfAssignmentCS(currPoly_indx,varId,0),neighbors.locationOfAssignmentCS(currPoly_indx,varId,1));
            const auto currNeighbor_loc = neighborsInLayers[neighbors.locationOfAssignmentCS(currPoly_indx,varId,0)];
            const auto& currNeighbor = *(neighborsVec[currNeighbor_loc.first][currNeighbor_loc.second]);

            //verify the neighbor
            for(size_t rowId=0; rowId < rowsNum; rowId++){
                const FieldElement testLocation = mappings.mapNonPermutationElement(rowId, testIndex);
                const FieldElement rowIndicator = testLocation - mappings_common.mapNonPermutationElement(testIndex);
                const FieldElement res = currNeighbor.eval(testLocation);
                
                const auto varLocation = defs.getVarLocation(varId);
                if(varLocation.inPerm){
                    const auto ref_loc = mappings.mapPermutationElement_witnessElement(0,2*varLocation.index);
                    const FieldElement ref = rowIndicator + ref_loc.second;
                    EXPECT_EQ(ref,res);
                    EXPECT_EQ(ref_loc.first,currNeighbor_loc.first);
                }
                else{
                    const auto ref = mappings.mapNonPermutationElement_witness(rowId,varLocation.index);
                    EXPECT_EQ(ref.second,res);
                    EXPECT_EQ(ref.first,currNeighbor_loc.first);
                }
            }
        }
        
        //neighbors for next row (next using instance permutation)
        const size_t shift = partialInstance.vectorsLen();
        for (size_t varId=shift; varId < shift+partialInstance.vectorsLen(); varId++){

            //verify the neighbor for current variable exists if and only if
            //it is used by the polynomial
            EXPECT_EQ(neighbors.existsAssignmentCS(currPoly_indx,varId), currPoly->isEffectiveInput(varId));

            //if neighbor does not exist, nothing else to do here
            if(!neighbors.existsAssignmentCS(currPoly_indx,varId))continue;

            //else, get the neighbor

            //verify the neighbor
            for(size_t rowId=0; rowId < rowsNum; rowId++){
                const FieldElement testLocation = mappings.mapNonPermutationElement(rowId, testIndex);
                const FieldElement nextRowIndicator = mappings.mapNonPermutationElement(rowId+1,0) - mappings_common.mapNonPermutationElement(0);
                
                // get the MSB of the rowId in order to decide the right neighbor version
                const size_t neighborVersion = mapFieldElementToInteger(rowIdMSB_index,1,testLocation);
                const auto currNeighbor_loc = neighborsInLayers[neighbors.locationOfAssignmentCS(currPoly_indx,varId,neighborVersion)];
                const auto& currNeighbor = *(neighborsVec[currNeighbor_loc.first][currNeighbor_loc.second]);
                
                 
                const FieldElement res = currNeighbor.eval(testLocation);

                const auto varLocation = defs.getVarLocation(varId-shift);
                if(varLocation.inPerm){
                    const auto ref_loc = mappings.mapPermutationElement_witnessElement(0,2*varLocation.index);
                    const FieldElement ref = nextRowIndicator + ref_loc.second;
                    
                    EXPECT_EQ(ref_loc.first,currNeighbor_loc.first);

                    switch(neighborVersion){
                        case 0: EXPECT_EQ(ref,res); break;
                        case 1: EXPECT_EQ(ref,res); break;
                        default : _COMMON_FATAL("Bad neighbor version");
                    }
                }
                else{
                    const auto ref = mappings.mapNonPermutationElement_witness(rowId+1,varLocation.index);
                    
                    EXPECT_EQ(ref.first,currNeighbor_loc.first);

                    switch(neighborVersion){
                        case 0: EXPECT_EQ(ref.second,res); break;
                        case 1: EXPECT_EQ(ref.second,res); break;
                        default : _COMMON_FATAL("Bad neighbor version");
                    }
                }

            }
        }
    }
}


//
// GTESTS
//

TEST(BairToAcspNeighbors,verifyID){
    //initialize neighbors instance	
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const common defs(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const instanceMappings mappings(defs);
    const CS_testLocations testLocations(defs);
    const AcspNeighbors neighbors(src.first,defs,mappings,testLocations);
    
    verifyID(src,neighbors);
}

TEST(BairToAcspNeighbors,verifyTwinLayer){

    //initialize neighbors instance	
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const common defs(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const instanceMappings mappings(defs);
    const CS_testLocations testLocations(defs);
    const AcspNeighbors neighbors(src.first,defs,mappings,testLocations);

    //if there is no routing network, there is not expected a
    //TWIN LAYER neighbor
    if(!defs.hasRoutingNetwork())return;
    
    verifyTwinLayer(src,neighbors);
}

TEST(BairToAcspNeighbors,verifyDeBruijn){

    //initialize neighbors instance	
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const common defs(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const instanceMappings mappings(defs);
    const CS_testLocations testLocations(defs);
    const AcspNeighbors neighbors(src.first,defs,mappings,testLocations);

    //if there is no routing network, there is not expected a
    //TWIN LAYER neighbor
    if(!defs.hasRoutingNetwork())return;
    
    verifyDeBruijn(src,neighbors);
}

TEST(BairToAcspNeighbors,verifyRoutingBit){

    //initialize neighbors instance	
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const common defs(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const instanceMappings mappings(defs);
    const CS_testLocations testLocations(defs);
    const AcspNeighbors neighbors(src.first,defs,mappings,testLocations);

    //if there is no routing network, there is not expected a
    //TWIN LAYER neighbor
    if(!defs.hasRoutingNetwork())return;
    
    verifyRoutingBit(src,neighbors);
}

TEST(BairToAcspNeighbors,verifyPermutationConstraintsSystem){

    //initialize neighbors instance	
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const common defs(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const instanceMappings mappings(defs);
    const CS_testLocations testLocations(defs);
    const AcspNeighbors neighbors(src.first,defs,mappings,testLocations);
    
    verifyPermutationConstraints(src,neighbors,testLocations);
}

TEST(BairToAcspNeighbors,verifyAssignmentConstraintsSystem){

    //initialize neighbors instance	
    const BairPair src = PCP_UTESTS::generate_valid_constraints();
    const common defs(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const instanceMappings mappings(defs);
    const CS_testLocations testLocations(defs);
    const AcspNeighbors neighbors(src.first,defs,mappings,testLocations);

    verifyAssignmentConstraints(src,neighbors,testLocations);
}

}
