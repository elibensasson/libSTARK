#include "BairWitnessChecker_UTEST.hpp"
#include <languages/Acsp/AcspWitnessChecker.hpp>
#include <reductions/BairToAcsp/BairToAcsp.hpp>

#include <gtest/gtest.h>

namespace {

using libstark::CBairToAcsp;
using libstark::BairWitness;
using libstark::BairInstance;
using libstark::AcspWitnessChecker;
using Algebra::FieldElement;
using Algebra::generateRandom;
using std::pair;
using std::vector;

typedef pair<BairInstance,BairWitness> BairPair;

/******************************************
 *     Input consistency tests
 ******************************************/
vector<FieldElement> getRandVector(const unsigned int len){
    vector<FieldElement> res(len);
    for(auto& e : res){
        e = generateRandom();
    }
    return res;
}

TEST(BairToAcsp_boundaryConstraints,complitness){
	const BairPair src = PCP_UTESTS::generate_valid_boundary();
    
    const auto& bairInstance = src.first;
    const auto coeffsPi = getRandVector(bairInstance.constraintsPermutation().numMappings());
    const auto coeffsChi = getRandVector(bairInstance.constraintsAssignment().numMappings());
    const auto instance = CBairToAcsp::reduceInstance(bairInstance, coeffsPi, coeffsChi);
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_TRUE(AcspWitnessChecker::verify_boundary(*instance,*witness));
}

TEST(BairToAcsp_boundaryConstraints,soundness){
	const BairPair src = PCP_UTESTS::generate_invalid_boundary();
    
    const auto& bairInstance = src.first;
    const auto coeffsPi = getRandVector(bairInstance.constraintsPermutation().numMappings());
    const auto coeffsChi = getRandVector(bairInstance.constraintsAssignment().numMappings());
    const auto instance = CBairToAcsp::reduceInstance(bairInstance, coeffsPi, coeffsChi);
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_boundary(*instance,*witness));
}

}
