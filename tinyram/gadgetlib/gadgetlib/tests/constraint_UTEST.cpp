#include <set>
#include <gtest/gtest.h>
#include <gadgetlib2/pp.hpp>
#include <gadgetlib2/constraint.hpp>

using ::std::set;

namespace PCP_Project {

TEST(ConstraintsLib, Rank1Constraint) {
    initPublicParamsFromEdwardsParam();
    //Rank1Constraint(const LinearCombination& a,
    //               const LinearCombination& b,
    //               const LinearCombination& c,
    //               const ::std::string& name);
    VariableArray x(10,"x");
    VariableAssignment assignment;
    for(int i = 0; i < 10; ++i) {
        assignment[x[i]] = Fp(i);
    }
    LinearCombination a = x[0] + x[1] + 2;     // <a,assignment> = 0+1+2=3
    LinearCombination b = 2*x[2] - 3*x[3] + 4; // <b,assignment> = 2*2-3*3+4=-1
    LinearCombination c = x[5];                // <c,assignment> = 5
    Rank1Constraint c1(a,b,c,"c1");
    //LinearCombination a() const;
    //LinearCombination b() const;
    //LinearCombination c() const;
    EXPECT_EQ(c1.a().eval(assignment), a.eval(assignment));
    EXPECT_EQ(c1.b().eval(assignment), b.eval(assignment));
    EXPECT_EQ(c1.c().eval(assignment), c.eval(assignment));
    //virtual bool isSatisfied(const VariableAssignment& assignment, bool printOnFail = false) const;
    EXPECT_FALSE(c1.isSatisfied(assignment));
    EXPECT_FALSE(c1.isSatisfied(assignment, PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    assignment[x[5]] = -3;
    EXPECT_TRUE(c1.isSatisfied(assignment));
    EXPECT_TRUE(c1.isSatisfied(assignment, PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    //virtual ::std::string annotation() const; --NOT TESTED, CAN CHANGE, FOR DEBUG ONLY
    //const Variable::set getUsedVariables() const; 
    const Variable::set varSet = c1.getUsedVariables();
    EXPECT_EQ(varSet.size(), 5);
    EXPECT_TRUE(varSet.find(x[0]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[1]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[2]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[3]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[4]) == varSet.end());
    EXPECT_TRUE(varSet.find(x[5]) != varSet.end());
}

TEST(ConstraintsLib, PolynomialConstraint) {
    //PolynomialConstraint(const Polynomial& a,
    //               const Polynomial& b,
    //               const ::std::string& name);
    VariableArray x(10,"x");
    VariableAssignment assignment;
    for(int i = 0; i < 10; ++i) {
        assignment[x[i]] = Algebra::one();
    }
    Polynomial a = x[0] + x[1] + 1;      // <a,assignment> = 1+1+1=1
    Polynomial b = x[1]*x[2] - x[3] + 0; // <b,assignment> = 1*1-1+0=0
    PolynomialConstraint c1(a,b,"c1");
    //virtual bool isSatisfied(const VariableAssignment& assignment, bool printOnFail = false) const;
    EXPECT_FALSE(c1.isSatisfied(assignment));
    EXPECT_FALSE(c1.isSatisfied(assignment, PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    assignment[x[3]] = 0;
    EXPECT_TRUE(c1.isSatisfied(assignment));
    EXPECT_TRUE(c1.isSatisfied(assignment, PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    //virtual ::std::string annotation() const; --NOT TESTED, CAN CHANGE, FOR DEBUG ONLY
    //const Variable::set getUsedVariables() const; 
    const Variable::set varSet = c1.getUsedVariables();
    EXPECT_EQ(varSet.size(), 4);
    EXPECT_TRUE(varSet.find(x[0]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[1]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[2]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[3]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[4]) == varSet.end());
}


} // namespace PCP_Project
