#include "BairWitnessChecker_UTEST.hpp"
#include <languages/Acsp/AcspWitnessChecker.hpp>
#include <reductions/BairToAcsp/BairToAcsp.hpp>

#include <gtest/gtest.h>

namespace {

using libstark::CBairToAcsp;
using libstark::BairWitness;
using libstark::BairInstance;
using libstark::AcspWitnessChecker;
using std::pair;

typedef pair<BairInstance,BairWitness> BairPair;

/******************************************
 *     Input consistency tests
 ******************************************/
TEST(BairToAcsp_boundaryConstraints,complitness){
	const BairPair src = PCP_UTESTS::generate_valid_boundary();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first);
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_TRUE(AcspWitnessChecker::verify_boundary(*instance,*witness));
}

TEST(BairToAcsp_boundaryConstraints,soundness){
	const BairPair src = PCP_UTESTS::generate_invalid_boundary();
    
    const auto instance = CBairToAcsp::reduceInstance(src.first);
    const auto witness = CBairToAcsp::reduceWitness(src.first,src.second);
    
    EXPECT_FALSE(AcspWitnessChecker::verify_boundary(*instance,*witness));
}

}
