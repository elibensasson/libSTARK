#include "lightCircPoly.hpp"
#include <gtest/gtest.h>

#define maxDegree 20
#define amountOfEvals 10

using libstark::lightCircPoly;
using Algebra::FieldElement;
using Algebra::zero;
using Algebra::one;
using Algebra::generateRandom;
using Algebra::UnivariatePolynomialGeneral;
using std::vector;
using std::max;


/**
 * A test to check conversion from UnivariatePolynomialGeneral
 * to lightCircPoly is done right.
 * A random UnivariatePolynomialGeneral is generated and converted
 * into a lightCircPoly.
 * Afterwards they are both evaluated on some random set
 * of elements.
 */
TEST(lightCircPoly,conversionFromUnivariate){
	//Define polynomial
	UnivariatePolynomialGeneral poly;	//polynomial for test
	for(int i=0; i<maxDegree; i++){
		FieldElement coeff = generateRandom();
		poly.setCoefficient(i,coeff);
	}

	lightCircPoly circ(poly);

	for(int i=0; i< amountOfEvals; i++){
		FieldElement x = generateRandom();
		FieldElement polyRes = poly.eval(x);

		vector< FieldElement> xvec(1);
		xvec[0] = x;
		FieldElement circRes = circ.eval(xvec);

		EXPECT_EQ(circRes,polyRes);
	}
}

/**
 *
 */


/**
 * A test to check conversion from UnivariatePolynomialGeneral
 * to lightCircPoly is done right in the case the
 * UnivariatePolynomialGeneral is the zero polynomial.
 * A zero UnivariatePolynomialGeneral is generated and converted
 * into a lightCircPoly.
 * Afterwards they are both evaluated on some random set
 * of elements.
 */
TEST(lightCircPoly,conversionFromZeroUnivariate){
	//Define polynomial
	UnivariatePolynomialGeneral poly;	//polynomial for test

	lightCircPoly circ(poly);

	for(int i=0; i< amountOfEvals; i++){
		FieldElement x = generateRandom();
		FieldElement polyRes = poly.eval(x);

		vector< FieldElement> xvec(1);
		xvec[0] = x;
		FieldElement circRes = circ.eval(xvec);

		EXPECT_EQ(circRes,polyRes);
	}
}

/**
 * A test for distinct polynomial multiplication.
 * A vector of random polynomial is generated,
 * and in addition a polynomial which is their
 * distinct product.
 *
 * Then some random vectors over the field are generated
 * and the product polynomial is evaluated on them, its
 * results are compared to the product of evaluation of
 * each polynomial from the vector on the corresponding field
 * element from the vector
 */
TEST(lightCircPoly,distinctMultiplication){
#define NUM_POLYS 10
	//Define polynomial
	UnivariatePolynomialGeneral poly[NUM_POLYS];	//polynomial for test
	for(int j=0; j<NUM_POLYS; j++){
		for(int i=0; i<maxDegree; i++){
			FieldElement coeff = generateRandom();
			poly[j].setCoefficient(i,coeff);
		}
	}

	lightCircPoly circ(poly[0]);
	for(int j=1; j<NUM_POLYS; j++){
		const auto circToAdd = lightCircPoly(poly[j]);
        circ.multiplyDistinct(circToAdd);
	}


	for(int i=0; i< amountOfEvals; i++){
		vector< FieldElement> xvec(NUM_POLYS);
		for(int j=0; j< NUM_POLYS; j++){
			FieldElement x = generateRandom();
			xvec[j] = x;
		}

		FieldElement polyRes = poly[0].eval(xvec[0]);
		for(int j=1; j<NUM_POLYS; j++){
			polyRes *= poly[j].eval(xvec[j]);
		}

		FieldElement circRes = circ.eval(xvec);

		EXPECT_EQ(circRes,polyRes);
	}
}

/**
 * A test for distinct polynomial addition.
 * A vector of random polynomial is generated,
 * and in addition a polynomial which is their
 * distinct summary.
 *
 * Then some random vectors over the field are generated
 * and the summary polynomial is evaluated on them, its
 * results are compared to the summary of evaluation of
 * each polynomial from the vector on the corresponding field
 * element from the vector
 */
TEST(lightCircPoly,distinctAddition){
#define NUM_POLYS 10
	//Define polynomial
	UnivariatePolynomialGeneral poly[NUM_POLYS];	//polynomial for test
	for(int j=0; j<NUM_POLYS; j++){
		for(int i=0; i<maxDegree; i++){
			FieldElement coeff = generateRandom();
			poly[j].setCoefficient(i,coeff);
		}
	}

	lightCircPoly circ(poly[0]);
	for(int j=1; j<NUM_POLYS; j++){
		const auto circToAdd = lightCircPoly(poly[j]);
        circ.addDistinct(circToAdd);
	}


	for(int i=0; i< amountOfEvals; i++){
		vector< FieldElement> xvec(NUM_POLYS);
		for(int j=0; j< NUM_POLYS; j++){
			FieldElement x = generateRandom();
			xvec[j] = x;
		}

		FieldElement polyRes = poly[0].eval(xvec[0]);
		for(int j=1; j<NUM_POLYS; j++){
			polyRes += poly[j].eval(xvec[j]);
		}

		FieldElement circRes = circ.eval(xvec);

		EXPECT_EQ(circRes,polyRes);
	}
}

/**
 * A test for polynomial addition, using same variables
 * A vector of random polynomial is generated,
 * and in addition a polynomial which is their
 * distinct summary.
 *
 * Then some random vectors over the field are generated
 * and the summary polynomial is evaluated on them, its
 * results are compared to the summary of evaluation of
 * each polynomial from the vector on the corresponding field
 * element from the vector
 */
TEST(lightCircPoly,nonDistinctAddition){
#define NUM_POLYS 10
	//Define polynomial
	UnivariatePolynomialGeneral poly[NUM_POLYS];	//polynomial for test
	for(int j=0; j<NUM_POLYS; j++){
		for(int i=0; i<maxDegree; i++){
			FieldElement coeff = generateRandom();
			poly[j].setCoefficient(i,coeff);
		}
	}

	lightCircPoly circ(poly[0]);
	for(int j=1; j<NUM_POLYS; j++){
		const lightCircPoly circToAdd(poly[j]);
        circ.addSameInputs(circToAdd);
	}


	for(int i=0; i< amountOfEvals; i++){
		vector< FieldElement> xvec(1);
        xvec[0] = generateRandom();

		FieldElement polyRes = poly[0].eval(xvec[0]);
		for(int j=1; j<NUM_POLYS; j++){
			polyRes += poly[j].eval(xvec[0]);
		}

		FieldElement circRes = circ.eval(xvec);

		EXPECT_EQ(circRes,polyRes);
	}
}

/**
 * A test for polynomial addition, using same variables
 * A vector of random polynomial is generated,
 * and in addition a polynomial which is their
 * distinct summary.
 *
 * Then some random vectors over the field are generated
 * and the summary polynomial is evaluated on them, its
 * results are compared to the summary of evaluation of
 * each polynomial from the vector on the corresponding field
 * element from the vector
 */
TEST(lightCircPoly,nonDistinctAddition_different_inputs_ammount){
#define NUM_POLYS 10
	//Define polynomial
	
    UnivariatePolynomialGeneral poly[NUM_POLYS];	//polynomial for test
	for(int j=0; j<NUM_POLYS; j++){
		for(int i=0; i<maxDegree; i++){
			FieldElement coeff = generateRandom();
			poly[j].setCoefficient(i,coeff);
		}
	}

	lightCircPoly circ1(poly[0]);
    vector<size_t> inputsInCirc1;
    inputsInCirc1.push_back(0);

    lightCircPoly circ2(poly[1]);
    vector<size_t> inputsInCirc2;
    inputsInCirc2.push_back(1);

    for(int j=2; j<NUM_POLYS; j++){
        const lightCircPoly toMult(poly[j]);
        if(rand()%2 == 0){
            circ1.multiplyDistinct(toMult);
            inputsInCirc1.push_back(j);
        }
        else{
		    circ2.multiplyDistinct(toMult);
            inputsInCirc2.push_back(j);
        }
	}
    //add circuits
    lightCircPoly combCirc(circ1);
    combCirc.addSameInputs(circ2);

    const size_t totalInputsAmount = max(inputsInCirc1.size(), inputsInCirc2.size());

	for(int i=0; i< amountOfEvals; i++){
		vector< FieldElement> xvec(totalInputsAmount);
        for(unsigned int j=0; j< xvec.size(); j++){
            xvec[j] = generateRandom();
        }

        //calculate result of circ1
        FieldElement circ1Ref = one();
        {
        size_t inpIndx = 0;
        for(const auto& inp : inputsInCirc1){
            circ1Ref *= poly[inp].eval(xvec[inpIndx++]);
        }
        }
        const vector<FieldElement> circ1_ass(xvec.begin(), xvec.begin()+inputsInCirc1.size());
        const FieldElement circ1Res = circ1.eval(circ1_ass);
        EXPECT_EQ(circ1Ref, circ1Res);

        //calculate result of circ2
        FieldElement circ2Ref = one();
        {
        size_t inpIndx = 0;
        for(const auto& inp : inputsInCirc2){
            circ2Ref *= poly[inp].eval(xvec[inpIndx++]);
        }
        }
        const vector<FieldElement> circ2_ass(xvec.begin(), xvec.begin()+inputsInCirc2.size());
        const FieldElement circ2Res = circ2.eval(circ2_ass);
        EXPECT_EQ(circ2Ref, circ2Res);

        //calculate total expected result
        const FieldElement expectedRes = circ1Res + circ2Res;
        
        //get results
		const FieldElement circRes = combCirc.eval(xvec);
		EXPECT_EQ(expectedRes,circRes);
	}
}

/**
 * Verify the input effectiveness query works
 * as expected. It says that inputs that are 
 * connected to the output in some way are effective,
 * and not otherwise.
 *
 * We first define some random polynomial with all its inputs effective,
 * than we expand it by adding some non effective inputs.
 *
 * We verify each input.
 */
TEST(lightCircPoly,isEffectiveInput){
	
    //define constants
    const size_t amountOfEffectiveInputs = 1+rand()%10;
    const size_t totalInputsAmount = amountOfEffectiveInputs + rand()%10;

    //generate random polynomial
    UnivariatePolynomialGeneral uniPoly;
    for(int i=0; i<maxDegree; i++){
        FieldElement coeff = generateRandom();
        uniPoly.setCoefficient(i,coeff);
    }
	lightCircPoly circ(uniPoly);
	for(unsigned int j=1; j<amountOfEffectiveInputs; j++){
        UnivariatePolynomialGeneral uniPoly;
        for(int i=0; i<maxDegree; i++){
            FieldElement coeff = generateRandom();
            uniPoly.setCoefficient(i,coeff);
        }
        circ.multiplyDistinct(lightCircPoly(uniPoly));
    }

    //define the mapping for expansion
    //it is the ID mapping
    vector<size_t> inputsMapping(amountOfEffectiveInputs);
    for(size_t i=0; i< amountOfEffectiveInputs; i++){
        inputsMapping[i] = i;
    }

    //expend the polynomials inputs
    lightCircPoly expendedPoly(circ,totalInputsAmount,inputsMapping);

    //test the original inputs are effective
    for(size_t i=0; i< amountOfEffectiveInputs; i++){
        EXPECT_TRUE(expendedPoly.isEffectiveInput(i));
    }

    //tests the additional inputs
    for(size_t i=amountOfEffectiveInputs; i< totalInputsAmount; i++){
        EXPECT_FALSE(expendedPoly.isEffectiveInput(i));
    }

}
