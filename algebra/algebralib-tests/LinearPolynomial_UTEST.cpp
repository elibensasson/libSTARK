#include <algebraLib/LinearPolynomial.hpp>
#include <gtest/gtest.h>

namespace{

using Algebra::FieldElement;
using Algebra::LinearPolynomial;
using Algebra::generateRandom;

TEST(Algebra,LinearPolynomial_eval){

    //construct a random poly
    const FieldElement coeff0 = generateRandom();
    const FieldElement coeff1 = generateRandom();
    const LinearPolynomial poly(coeff0,coeff1);

    //choose some random assignment
    const FieldElement x = generateRandom();

    //calculate reference
    const FieldElement ref = coeff0 + coeff1*x;

    //calculate poly result
    const FieldElement res = poly.eval(x);

    //compare
    EXPECT_EQ(ref,res);
}

TEST(Algebra,LinearPolynomial_composition){

    //construct a random poly1
    const FieldElement coeff01 = generateRandom();
    const FieldElement coeff11 = generateRandom();
    const LinearPolynomial poly1(coeff01,coeff11);

    //construct a random poly2
    const FieldElement coeff02 = generateRandom();
    const FieldElement coeff12 = generateRandom();
    const LinearPolynomial poly2(coeff02,coeff12);

    //choose some random assignment
    const FieldElement x = generateRandom();

    //calculate reference
    const FieldElement ref = coeff01 + coeff11*(coeff02 + coeff12*x);

    //calculate poly result
    const FieldElement res = poly1.compose(poly2).eval(x);

    //compare
    EXPECT_EQ(ref,res);
}

}
