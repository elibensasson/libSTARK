#include <gtest/gtest.h>
#include <algebraLib/variable.hpp>
#include <algebraLib/variable_operators.hpp>

using namespace Algebra;
using std::cout;
using std::endl;

namespace {

TEST(variable, Name) {
	Variable x("x");
	EXPECT_EQ(x.name(), "x");
	Variable y("abc");
	EXPECT_EQ(y.name(), "abc");
}

TEST(linearTerm, Name) {
	Variable x("x");
	LinearTerm l(x, Algebra::one());
	EXPECT_EQ(l.asString(), "x");
	LinearTerm l2(x, Algebra::zero());
	EXPECT_EQ(l2.asString(), Algebra::zero().asString());
	LinearTerm l3(x, Algebra::xFE());
	EXPECT_EQ(l3.asString(), Algebra::xFE().asString() + "*x");
}

TEST(linearTerm, EvaluationWithMap) {
	VariableAssignment assignemnt;
	Variable x("x");
	Variable y("y");
	assignemnt[x] = Algebra::one();
	assignemnt[y] = Algebra::xFE();
	LinearTerm l1(x);
	LinearTerm l2(x, Algebra::zero());
	EXPECT_EQ(l1.eval(assignemnt), Algebra::one());
	EXPECT_EQ(l2.eval(assignemnt), Algebra::zero());
	LinearTerm l3(y, Algebra::xFE());
	FElem elem1 = Algebra::xFE() * Algebra::xFE();
	EXPECT_EQ(l3.eval(assignemnt), elem1);
}

TEST(linearCombination, Name) {
	Variable x("x");
	Variable y("y");
	LinearCombination l1 = x + y;
	EXPECT_EQ(l1.asString(), "(x+y)");
	LinearCombination l2(Algebra::zero());
	EXPECT_EQ(l2.asString(), "([])");
	l2 += l1;
	EXPECT_EQ(l2.asString(), "(x+y)");
	l2 += Algebra::one();
	EXPECT_EQ(l2.asString(), "(x+y+[1])");
	l2 += Algebra::one();
	EXPECT_EQ(l2.asString(), "(x+y)");
	LinearCombination l3(Algebra::one());
	EXPECT_EQ(l3.asString(), "([1])");
	LinearCombination l4(x);
	EXPECT_EQ(l4.asString(), "(" + x.name() + ")");
}

TEST(linearCombination, Constructor) {
	Variable x("x");
	LinearTerm lt1(x, Algebra::zero());
	LinearCombination l1(lt1);
	EXPECT_EQ("([])", l1.asString());
}

TEST(linearCombination, Addition) {
	Variable x("x");
	LinearCombination l1 = x + Algebra::one();
	EXPECT_EQ(l1.asString(), "(x+[1])");
	LinearCombination l2 = Algebra::zero() + x;
	EXPECT_EQ(l2.asString(), "(x)");
	LinearTerm lt1(x, Algebra::one());
	LinearTerm lt2(x, Algebra::xFE());
	LinearCombination l3 = lt1 + lt2;
	EXPECT_EQ(l3.asString(), "(x+[0 1]*x)");
	LinearCombination l4 = l3 + Algebra::one();
	EXPECT_EQ(l4.asString(), "(x+[0 1]*x+[1])");
	LinearCombination l5 = Algebra::xFE() + l4;
	EXPECT_EQ(l5.asString(), "(x+[0 1]*x+[1 1])");
	
	Variable y("y");
	LinearCombination l6 = x + y;
	EXPECT_EQ(l6.asString(), "(x+y)");
	LinearCombination l7 = lt2 + y;
	EXPECT_EQ(l7.asString(), "([0 1]*x+y)");

	LinearTerm lt3(x, Algebra::zero());
	LinearCombination l8 = lt1 + lt3;
	EXPECT_EQ(l8.asString(), "(x)");
	LinearCombination l9(lt3 + lt1);
	EXPECT_EQ(l9.asString(), "(x)");
}

TEST(linearCombination, Multiplication) {
	Variable x("x");
	Variable y("y");
	LinearCombination l1 = x + y;
	LinearCombination l2 = l1 * Algebra::zero();
	EXPECT_EQ(l2.asString(), "([])");
	LinearCombination l3 = Algebra::xFE() * l1;
	EXPECT_EQ(l3.asString(), "([0 1]*x+[0 1]*y)");
}

}
