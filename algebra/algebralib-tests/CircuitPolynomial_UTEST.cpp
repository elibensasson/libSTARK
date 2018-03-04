#include <algebraLib/CircuitPolynomial.hpp>
#include <algebraLib/variable_operators.hpp>
#include <algebraLib/ErrorHandling.hpp>
#include <algebraLib/FieldElement.hpp>
#include <gtest/gtest.h>

using namespace Algebra;

namespace {

TEST(CircuitPolynomial, isZero) {
	CircuitPolynomial p1(Algebra::zero());
	EXPECT_EQ(p1.isZero(), true);
	CircuitPolynomial p2(Algebra::one());
	p2 += Algebra::one();
	EXPECT_EQ(p2.isZero(), true);
}

TEST(CircuitPolynomial, checkName) {
	Variable x("x");
	Variable y("y");
	Variable z("z");
	CircuitPolynomial p1(x);
	EXPECT_EQ(p1.asString(), "(x)");
	CircuitPolynomial p2 = x * y;
	EXPECT_EQ(p2.asString(), "(x)*(y)");
	p2 *= z;
	EXPECT_EQ(p2.asString(), "(x)*(y)*(z)");
	CircuitPolynomial p3 = x*y + y*z + z*z;
	EXPECT_EQ(p3.asString(), "((x)*(y)+(y)*(z)+(z)*(z))");
	CircuitPolynomial p4(Algebra::xFE() * z);
	EXPECT_EQ(p4.asString(), "([0 1]*z)");
	CircuitPolynomial p5 = p3 + p4;
	EXPECT_EQ(p5.asString(), "((x)*(y)+(y)*(z)+(z)*(z)+([0 1]*z))");
}

TEST(CircuitPolynomial, getOperation) {
	Variable x("x");
	Variable y("y");
	Variable z("z");
	CircuitPolynomial p1(x);
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 *= y;
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 += Algebra::zero();
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	CircuitPolynomial p2(x + y);
	EXPECT_EQ(p2.getOperation(), PolynomialOperation::PROD);
	CircuitPolynomial p3(x*y);
	EXPECT_EQ(p3.getOperation(), PolynomialOperation::PROD);
	CircuitPolynomial p4 = x*y + y*z;
	EXPECT_EQ(p4.getOperation(), PolynomialOperation::ADD);
	p4 *= x;
	EXPECT_EQ(p4.getOperation(), PolynomialOperation::MUL);
}

TEST(CircuitPolynomial, Addition) {
	Variable x("x");
	Variable y("y");
	CircuitPolynomial p1(x);
	p1 += x;
	EXPECT_EQ(p1.asString(), "(x+x)");
	p1 += y;
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	EXPECT_EQ(p1.asString(), "(x+x+y)");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 += Algebra::zero();
	EXPECT_EQ(p1.asString(), "(x+x+y)");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 += Algebra::one();
	EXPECT_EQ(p1.asString(), "(x+x+y+[1])");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 = Algebra::xFE() * x + Algebra::one() * y;
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	EXPECT_EQ(p1.asString(), "([0 1]*x+y)");
	p1 = Algebra::xFE() * x + Algebra::zero() * y;
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	EXPECT_EQ(p1.asString(), "([0 1]*x)");
	p1 = Algebra::zero() * x + Algebra::zero()* y;
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	EXPECT_EQ(p1.asString(), "([])");
	p1 += x;
	EXPECT_EQ(p1.asString(), "(x)");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 += (x*x + y*y);
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::ADD);
}

TEST(CircuitPolynomial, Multiplication) {
	Variable x("x");
	Variable y("y");
	Variable z("z");
	CircuitPolynomial p1 = x * y;
	EXPECT_EQ(p1.asString(), "(x)*(y)");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 *= z;
	EXPECT_EQ(p1.asString(), "(x)*(y)*(z)");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 = x * y * z;
	EXPECT_EQ(p1.asString(), "(x)*(y)*(z)");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::PROD);
	p1 = (((Algebra::xFE() * x + Algebra::one()) * y) + y) * z;
	EXPECT_EQ(p1.asString(), "((([0 1]*x+[1])*(y)+(y))*(z))");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::MUL);
	p1 *= x;
	EXPECT_EQ(p1.asString(), "((([0 1]*x+[1])*(y)+(y))*(z)*(x))");
	EXPECT_EQ(p1.getOperation(), PolynomialOperation::MUL);
}


TEST(CircuitPolynomial, Evaluation) {
	FElem a = mapIntegerToFieldElement(0, 2, 3);
	Variable x("x");
	Variable y("y");
	Variable z("z");
	CircuitPolynomial p1 = x + y;
	VariableAssignment assignment;
	assignment[x] = a;
	assignment[y] = Algebra::one();
	EXPECT_EQ(p1.eval(assignment), Algebra::xFE());
	p1 = x * y;
	EXPECT_EQ(p1.eval(assignment), a);
	p1 = (((Algebra::xFE() * x + Algebra::one()) * y) + y) * z;
	assignment[z] = Algebra::zero();
	EXPECT_EQ(p1.eval(assignment), Algebra::zero());
	p1 = ((Algebra::xFE() * x + Algebra::one()) * y);
	EXPECT_EQ(p1.eval(assignment), (Algebra::xFE()*a + Algebra::one()));
	assignment[y] = a;
	EXPECT_EQ(p1.eval(assignment), (Algebra::xFE()*a + Algebra::one()) * a);
}

TEST(CircuitPolynomial, isSatisfied) {
	VariableAssignment assignment;
	Variable x("x");
	Variable y("y");
	Variable z("z");
	assignment[x] = Algebra::one();
	assignment[y] = Algebra::one();
	assignment[z] = (Algebra::xFE() * Algebra::xFE() + Algebra::one()).inverse();
	CircuitPolynomial p1(Algebra::zero());
	EXPECT_EQ(p1.isSatisfied(assignment), true);
	p1 += x;
	EXPECT_EQ(p1.isSatisfied(assignment), false);
	p1 += y;
	EXPECT_EQ(p1.isSatisfied(assignment), true);
	p1 = (Algebra::xFE() * x + Algebra::xFE()) * x + y * y + Algebra::one();
	EXPECT_EQ(p1.isSatisfied(assignment), true);
	assignment[y] = Algebra::xFE();
	EXPECT_EQ(p1.isSatisfied(assignment), false);
	assignment[y] = Algebra::one();
	CircuitPolynomial p2 = p1 * z;
	EXPECT_EQ(p2.isSatisfied(assignment), true);
	assignment[y] = Algebra::xFE();
	p2 += Algebra::one();
	EXPECT_EQ(p2.isSatisfied(assignment), true);
	

}

} // namespace
