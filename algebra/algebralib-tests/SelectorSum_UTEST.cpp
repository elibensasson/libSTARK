//#include <algebraLib/SelectorSum.hpp>
//#include <gtest/gtest.h>
//using namespace Algebra;
//using namespace std;
//
////here the only constraint is the constant one polynomial
//TEST(SelectorSum, basic){
////	SelectorSum T;
//	vector<CircuitPolynomial> constraints;
//	constraints.push_back(CircuitPolynomial(one()));
////	vector<VarIndex_t> selectorVars({ 0, 1, 2 });
//	vector<Variable> selectorVars({ Variable("0"), Variable("1"), Variable("2") });
//
//	vector<long> selectorToConstraint({ 0, 0, 0 });
//	SelectorSum P(constraints, selectorVars, selectorToConstraint);
//
//	EXPECT_EQ(P.eval({ one(), one(), one() }), zero());
//	EXPECT_EQ(P.eval({ one(), zero(), zero() }), one());
//
//}
//
////here the only constraint is the constant one polynomial
//TEST(SelectorSum, test2){
//	vector<CircuitPolynomial> constraints;
//	constraints.push_back(CircuitPolynomial(zero()));
//	constraints.push_back(CircuitPolynomial(xFE()));
//	//vector<VarIndex_t> selectorVars({ 0, 1, 2 });
//
//	vector<Variable> selectorVars({ Variable("0"), Variable("1"), Variable("2") });
//	
//	vector<long> selectorToConstraint({ 0, 0,0,0,1,1,1,1 });
//	SelectorSum P(constraints, selectorVars, selectorToConstraint);
//
//
//	//if the input is binary encoding of 0-3, we should see 0
//	// if it is binary encoding of 4-7, we should see x
//	EXPECT_EQ(P.eval({ one(), zero(),zero() }), zero());
//	EXPECT_EQ(P.eval({ one(), one(), zero() }), zero());
//	EXPECT_EQ(P.eval({ zero(), one(), zero() }), zero());
//	EXPECT_EQ(P.eval({ zero(), zero(), zero() }), zero());
//
//	EXPECT_EQ(P.eval({ zero(), zero(), one() }), xFE());
//	EXPECT_EQ(P.eval({ zero(), one(), one() }), xFE());
//	EXPECT_EQ(P.eval({ one(), one(), one() }), xFE());
//	EXPECT_EQ(P.eval({ one(), zero(), one() }), xFE());
//
//}