#include "BairWitnessChecker_UTEST.hpp"
#include <languages/Acsp/AcspWitnessChecker.hpp>
#include <reductions/BairToAcsp/Details/witnessReduction.hpp>
#include <reductions/BairToAcsp/BairToAcsp.hpp>
#include <gtest/gtest.h>
namespace{

using libstark::BairToAcsp::common;
using libstark::BairToAcsp::commonMappings;
using libstark::BairToAcsp::witnessMappings;
using libstark::BairToAcsp::witnessReduction;
using libstark::CBairToAcsp;
using libstark::BairWitness;
using libstark::BairInstance;
using libstark::AcspWitness;
using libstark::AcspInstance;
using libstark::AcspWitnessChecker;
using libstark::ConstraintSys;
using libstark::Sequence;
using Algebra::FieldElement;
using Algebra::PolynomialInterface;
using Algebra::UnivariatePolynomialInterface;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::PolynomialDegree;
using Algebra::zero;
using Algebra::one;
using Algebra::generateRandom;
using Algebra::elementsSet_t;
using Algebra::mapFieldElementToInteger;
using Infrastructure::POW2;
using Infrastructure::Log2;
using Infrastructure::CEIL;
using std::pair;
using std::rand;
using std::vector;
using std::unique_ptr;
using std::move;
using std::max;
using std::vector;

vector<FieldElement> genRandVector(const unsigned int len){
    vector<FieldElement> res(len);
    for(auto& e : res){
        e = generateRandom();
    }
    return res;
}

typedef pair<BairInstance,BairWitness> BairPair;

/*************************************************************
 *                   TESTS IMPLEMENTATION
 ************************************************************/

class BairToAcsp_tester : private CBairToAcsp, private witnessReduction{
    public:

    /*******************************************************
     *           Acsp Witness modifiers
     *     (for soundness tests of the constraints poly)
     *******************************************************/
     static AcspWitness ruinRoutingBit(const BairPair& bair_pair){
   
        //get common information
        common commonDef(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
        witnessMappings witnessMapping(commonDef);
    
        //ordered basis for witness space
        const auto& basis = witnessMapping.getImageSpaceOrderedBasis();
        const vector<FieldElement> orderedBasis(basis.begin(),basis.end());
  
        const auto& partialInstance = bair_pair.first;
        
        //get the mapping
        auto mappings = witnessReduction::getEmbeddingMapping(partialInstance,bair_pair.second, commonDef,witnessMapping);

        //find some field element not in GF2
        FieldElement newVal = generateRandom();
        while((newVal == zero()) || (newVal == one())){
            newVal = generateRandom();
        }

        //find some victim index
        const size_t column = rand()%(POW2(commonDef.widthSpaceDimension())-2);
        const size_t row = rand()%POW2(commonDef.heightSpaceDimension());
        const size_t layer = rand()%2;

        //get the victim indicator
        const auto victim = witnessMapping.mapNetworkRoutingBit_witnessIndex(row,column,layer);

        //change the value
        mappings[victim.first][victim.second] = newVal;

        //construct witness
        //and return it
        std::vector<unique_ptr<const AcspWitness::polynomial>> assignments;
        for(const auto& mapping : mappings){
            assignments.push_back(unique_ptr<const AcspWitness::polynomial>(new UnivariatePolynomialGeneral(mapping,basis,zero())));
        }
        return AcspWitness(move(assignments));
     }
    
     static AcspWitness ruinDeBruijnLastColumn(const BairPair& bair_pair){
   
        //get common information
        common commonDef(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
        witnessMappings witnessMapping(commonDef);
    
        //ordered basis for witness space
        const auto& basis = witnessMapping.getImageSpaceOrderedBasis();
        const vector<FieldElement> orderedBasis(basis.begin(),basis.end());
  
        const auto& partialInstance = bair_pair.first;
        
        //get the mapping
        auto mappings = witnessReduction::getEmbeddingMapping(partialInstance,bair_pair.second, commonDef,witnessMapping);

        //find some victim index
        const size_t lastCol = commonDef.imageWidth() - 2;
        const size_t row = rand()%POW2(commonDef.heightSpaceDimension());
        const size_t layer = rand()%(2*commonDef.variablesPerm().size());

        //get the victim indicator
        const auto victim = witnessMapping.mapNetworkElement_witnessIndex(row,lastCol,layer);

        //change the value
        //adding 1 makes it different for sure
        mappings[victim.first][victim.second] += one();

        //construct witness
        //and return it
        std::vector<unique_ptr<const AcspWitness::polynomial>> assignments;
        for(const auto& mapping : mappings){
            assignments.push_back(unique_ptr<const AcspWitness::polynomial>(new UnivariatePolynomialGeneral(mapping,basis,zero())));
        }
        return AcspWitness(move(assignments));
     }

     static AcspWitness ruinAdditionalElementRouting(const BairPair& bair_pair){
   
        //get common information
        common commonDef(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
        witnessMappings witnessMapping(commonDef);
  
        //ordered basis for witness space
        const auto& basis = witnessMapping.getImageSpaceOrderedBasis();
        const vector<FieldElement> orderedBasis(basis.begin(),basis.end());
  
        const auto& partialInstance = bair_pair.first;
        
        //get the mapping
        auto mappings = witnessReduction::getEmbeddingMapping(partialInstance,bair_pair.second, commonDef,witnessMapping);

        //find some victim index
        const size_t firstCol = 0;
        const size_t layer = rand()%(2*commonDef.variablesPerm().size());

        //get the victim indicator
        const FieldElement victim = witnessMapping.mapPermutationColumnId_spaceElement(firstCol);
        const size_t victim_index = Algebra::getSpaceIndexOfElement(orderedBasis,Algebra::zero(),victim);

        //change the value
        //adding 1 makes it different for sure
        mappings[layer][victim_index] += one();

        //construct witness
        //and return it
        std::vector<unique_ptr<const AcspWitness::polynomial>> assignments;
        for(const auto& mapping : mappings){
            assignments.push_back(unique_ptr<const AcspWitness::polynomial>(new UnivariatePolynomialGeneral(mapping,basis,zero())));
        }
        return AcspWitness(move(assignments));
     }

     static AcspWitness ruinDeBruijnVertexData(const BairPair& bair_pair){
   
        //get common information
        common commonDef(bair_pair.first,genRandVector(bair_pair.first.constraintsPermutation().numMappings()),genRandVector(bair_pair.first.constraintsAssignment().numMappings()));
        witnessMappings witnessMapping(commonDef);
  
        //ordered basis for witness space
        const auto& basis = witnessMapping.getImageSpaceOrderedBasis();
        const vector<FieldElement> orderedBasis(basis.begin(),basis.end());
  
        const auto& partialInstance = bair_pair.first;
        
        //get the mapping
        auto mappings = witnessReduction::getEmbeddingMapping(partialInstance,bair_pair.second, commonDef,witnessMapping);

        //find some victim index
        const size_t lastCol = commonDef.imageWidth() - 2;
        const size_t column = rand()%lastCol;
        const size_t row = rand()%POW2(commonDef.heightSpaceDimension());
        const size_t layer = rand()%(2*commonDef.variablesPerm().size());

        //get the victim indicator
        const auto victim = witnessMapping.mapNetworkElement_witnessIndex(row,column,layer);

        //change the value
        //adding 1 makes it different for sure
        mappings[victim.first][victim.second] += one();

        //construct witness
        //and return it
        std::vector<unique_ptr<const AcspWitness::polynomial>> assignments;
        for(const auto& mapping : mappings){
            assignments.push_back(unique_ptr<const AcspWitness::polynomial>(new UnivariatePolynomialGeneral(mapping,basis,zero())));
        }
        return AcspWitness(move(assignments));
     }

};


/******************************************
 *     Constraints polynomial tests
 ******************************************/

TEST(BairToAcsp_Constraints,reductionCompletness){
	const BairPair src = PCP_UTESTS::generate_valid_pair();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_TRUE(AcspWitnessChecker::verify_vanishing(*instance,*witness));
}

TEST(BairToAcsp_Constraints,reductionSoundness_routingBits){
	const BairPair src = PCP_UTESTS::generate_valid_pair();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = BairToAcsp_tester::ruinRoutingBit(src);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_vanishing(*instance,witness));
}

TEST(BairToAcsp_Constraints,reductionSoundness_DeBruijnLastColumn){
	const BairPair src = PCP_UTESTS::generate_valid_pair();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = BairToAcsp_tester::ruinDeBruijnLastColumn(src);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_vanishing(*instance,witness));
}

TEST(BairToAcsp_Constraints,reductionSoundness_DeBruijnLastAdditionalElementRouting){
	const BairPair src = PCP_UTESTS::generate_valid_pair();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = BairToAcsp_tester::ruinAdditionalElementRouting(src);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_vanishing(*instance,witness));
}

TEST(BairToAcsp_Constraints,reductionSoundness_DeBruijnDataRouting){
	const BairPair src = PCP_UTESTS::generate_valid_pair();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = BairToAcsp_tester::ruinDeBruijnVertexData(src);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_vanishing(*instance,witness));
}

TEST(BairToAcsp_Constraints,reductionComplitness_Constraints){
	const BairPair src = PCP_UTESTS::generate_valid_constraints();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_TRUE(AcspWitnessChecker::verify_vanishing(*instance,*witness));
}

TEST(BairToAcsp_Constraints,reductionSoundness_Permutation_Constraints){
	const BairPair src = PCP_UTESTS::generate_invalid_constraints_Permutation();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_vanishing(*instance,*witness));
}

TEST(BairToAcsp_Constraints,reductionSoundness_Assignment_Constraints){
	const BairPair src = PCP_UTESTS::generate_invalid_constraints_Assignment();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first,genRandVector(src.first.constraintsPermutation().numMappings()),genRandVector(src.first.constraintsAssignment().numMappings()));
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_vanishing(*instance,*witness));
}

}
