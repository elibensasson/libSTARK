/**
*       @file  LinearizedPolynomials_UTEST.cpp
*      @brief  A file for unit testing the classes of linearized polynomials - AffinePolynomial, LinearizedPolynomial and SubspacePolynomial
*      Unit tests use GTEST library
*      This file can be linked to external main function
*      (for example for wide system unit-testing)
*      Or it can be compiled to an executable by defining the macro
*	LINEARIZEDPOLYNOMIALS_UTEST_MAIN.
*
*
*	Additionally uses library GTEST.
*
*     @author  Ariel Gabizon, ariel.gabizon@gmail.com
* =====================================================================================
*/

#include <iostream>
#include "algebraLib/SubspacePolynomial.hpp"
#include "algebraLib/AffinePolynomial.hpp"
#include <gtest/gtest.h>



using namespace Algebra;

using std::vector;

namespace{

	const int NUM_OF_ELEMENTS = 20;
	const int NUM_OF_COEFF = 15;
	const int NUM_OF_REPETITIONS = 20;
	const int NUM_OF_SPEEDREPETITIONS = 100000;


	/******************************************************************************************************/
	/***************************************** Helper functions *******************************************/
	/******************************************************************************************************/

	/* The function evaluates an Affine polynomial at a given point and returns the result.
	It is used in the tests here to check that AffinePolynomial.eval is correct
	*/
	FieldElement evalNoMat(const FieldElement& x, const vector<FieldElement>& coefficients, const FieldElement& constantFactor)  {

		FieldElement res = zero();

		/*we take advantage of the fact squaring is very efficient in GF2E,
		and that x^{2^i} = (x^{2^{i-1})^2. Each iteration we square the current power
		of x- power_x, multiply it by the relevant coefficient, and add to the sum,
		resulting in sum_{i=0}^{size-1} coeffcieints[i]*x^{2^i} */

		FieldElement currPower, power_x = x;
		for (unsigned long i = 0; i<coefficients.size(); i++) {
			currPower = coefficients[i] * power_x;

			res += currPower;
			power_x *= power_x;
		}

		return	FieldElement(res + constantFactor);
	}

	//generates a random coefficient vector
	vector<FieldElement> randomCoeff(){
		vector<FieldElement> coeff;
		for (int i = 0; i < NUM_OF_COEFF; i++){
			coeff.push_back(generateRandom());
		}
		return coeff;
	}

	//generates a random set of field elements
	elementsSet_t randomSet(){
		elementsSet_t set;
		for (int i = 0; i < NUM_OF_ELEMENTS; i++){
			set.insert(generateRandom());
		}
		return set;
	}



	/******************************************************************************************************/
	/***************************************** Tests *******************************************/
	/******************************************************************************************************/

		//Testing constructor of AffinePolynomial 
	TEST(Algebra, AffinePolynomialConstructor){
		const FieldElement ZERO = zero();

		vector<FieldElement> coeff=randomCoeff();
		FieldElement constFact = generateRandom();
		AffinePolynomial p(coeff, constFact);
		for (int i = 0; i < NUM_OF_COEFF; i++){
			if (coeff[i]!= ZERO){
                EXPECT_EQ(coeff[i], p.getCoefficient(1<<(i)));
            }

		}
		EXPECT_EQ(constFact, p.getCoefficient(0));
	

	}

//Testing constructor of LinearizedPolynomial 
TEST(Algebra, linearizedPolynomialConstructor){
	const FieldElement ZERO = zero();

	vector<FieldElement> coeff = randomCoeff();
	LinearizedPolynomial p(coeff);
	for (int i = 0; i < NUM_OF_COEFF; i++){
		if (coeff[i] != ZERO){
			EXPECT_EQ(coeff[i], p.getCoefficient(1<<(i)));
        }

	}
	EXPECT_EQ(p.getCoefficient(0), ZERO);
}


	//testing AffinePolynomial evaluates correctly
	TEST(Algebra, AffinePolynomialEval){

		vector<FieldElement> coeff =  randomCoeff();
		FieldElement constFact =  generateRandom();
		AffinePolynomial p(coeff, constFact);
		FieldElement x;
		for (int i = 0; i < NUM_OF_REPETITIONS; i++){
			x = generateRandom();
				EXPECT_EQ(evalNoMat(x,coeff,constFact), p.eval(x));

		}

	}

#define CHECK_SPEED
#ifdef CHECK_SPEED

	//testing how fast AffinePolynomial evaluate
	TEST(Algebra, AffinePolynomialSpeed){

		vector<FieldElement> coeff = randomCoeff();
		FieldElement constFact = generateRandom();
		AffinePolynomial p(coeff, constFact);
		FieldElement x;
		for (int i = 0; i < NUM_OF_SPEEDREPETITIONS; i++){
			x = generateRandom();
	 p.eval(x);

		}

	}

#endif
	//testing the constructor of SubspacePolynomial - seeing it creates a polynomial zero exactly on the desired span

	TEST(Algebra, SubspacePolynomialConstructor){

		elementsSet_t spanSet = randomSet();
		
		
		/*spanSet.insert(a);
		spanSet.insert(b);
		spanSet.insert(a + b);
		SubspacePolynomial p(spanSet);
		std::cout << "size::" << p.getSize();
		EXPECT_EQ(z, p.eval(a));
		EXPECT_EQ(z, p.eval(b));

		EXPECT_EQ(z, p.eval(a + b));
		EXPECT_EQ(4, p.getDegree());
		EXPECT_NE(z, p.eval( a*b+a));*/

	}


	//testing the constructor of SubspacePolynomial - seeing it creates a polynomial zero exactly on the desired span

	TEST(Algebra, SubPoly){
		const FieldElement ZERO = zero();
		FieldElement a = generateRandom();
		FieldElement b;
		do{
			b = generateRandom();
		} while (a == b);
		elementsSet_t spanSet;
		spanSet.insert(a);
		spanSet.insert(b);
		spanSet.insert(a + b);
		SubspacePolynomial p(spanSet);
		//std::cout << "degree::" << p.getDegree();
		EXPECT_EQ(ZERO, p.eval(a));
		EXPECT_EQ(ZERO, p.eval(b));

		EXPECT_EQ(ZERO, p.eval(a + b));
		EXPECT_EQ(4, PolynomialDegree::integral_t(p.getDegree()));
		EXPECT_NE(ZERO, p.eval(a*b + a));

	}



} //anonymous namespace
