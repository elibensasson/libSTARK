#include <algebraLib/UnivariatePolynomialGeneral.hpp>
#include <gtest/gtest.h>

namespace{

#define maxDegree 20
#define amountOfEvals 10

using Algebra::FieldElement;
using Algebra::UnivariatePolynomialInterface;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::zero;
using Algebra::one;
using Algebra::generateRandom;
using Algebra::elementsSet_t;
using Algebra::compareFieldElements;

FieldElement HornerEval(FieldElement const* coefficient, const FieldElement& x){
	FieldElement const* currCoef = coefficient + maxDegree - 1;
	FieldElement result = zero();
	for (;currCoef >= coefficient; currCoef--){
		result = (*currCoef) + (x*result);
	}
	return result;
}

TEST(Algebra,UniPolyEvalSanity){
	//Define polynomial
	FieldElement coefficient[maxDegree]; //coefficient for test
	UnivariatePolynomialGeneral poly;	//polynomial for test
	for(int i=0; i<maxDegree; i++){
		coefficient[i] = generateRandom();
		poly.setCoefficient(i,coefficient[i]);
	}

	for(int i=0; i< amountOfEvals; i++){
		FieldElement x = generateRandom();
		FieldElement y = HornerEval(coefficient,x);

		FieldElement polyRes = poly.eval(x);

		EXPECT_EQ(y,polyRes);
	}
}

/**
 * Verifies that polynomial constructed using set of roots
 * really has all the roots.
 * This is done by generating a random set of roots,
 * constructing a polynomial using them, and finally
 * checking the evaluation of the polynomial on each
 * of them is the zero element
 */
TEST(Algebra,UniPolyConstructionByRoots_Completeness){
	//Generate roots
	elementsSet_t roots;
	for (int i=0; i<maxDegree; i++){
		FieldElement root = generateRandom();
		roots.insert(root);
	}

	//Define polynomial
	UnivariatePolynomialGeneral poly(roots);	//polynomial for test

	/** Test all roots */
	for(elementsSet_t::iterator root=roots.begin(); root!=roots.end(); root++){
		FieldElement polyRes = poly.eval(*root);
		EXPECT_EQ(polyRes, (zero()));
	}
}

/**
 * Verifies that polynomial constructed using set of roots
 * evaluates to zero only on those roots.
 * This is done by generating a random set of roots,
 * constructing a polynomial using them.
 * Afterwards an iteration generates random elements
 * that the polynomial is evaluated on them.
 * It expected to evaluate to zero if and only if
 * the element is in the roots set.
 */
TEST(Algebra,UniPolyConstructionByRoots_Soundness){
	//Generate roots
	elementsSet_t roots;
	for (int i=0; i<maxDegree; i++){
		FieldElement root = generateRandom();
		roots.insert(root);
	}

	//Define polynomial
	UnivariatePolynomialGeneral poly(roots);	//polynomial for test
	EXPECT_EQ(roots.size(), Algebra::PolynomialDegree::integral_t(poly.getDegree()));
	for(int i=0 ;i<amountOfEvals; i++){
		const FieldElement x = generateRandom();		
		const FieldElement polyRes = poly.eval(x);

		if(roots.count(x) > 0){ //is a root
			EXPECT_EQ(polyRes, (zero()));
		}
		else { //is not a root
			EXPECT_NE(polyRes, (zero()));
		}
	}
}

/**
 * Verifies the default constructor of
 * UnivariatePolynomialGeneral generates the zero polynomial
 */
TEST(Algebra,UniPolyDefault){
	//Define polynomial
	UnivariatePolynomialGeneral poly;	//polynomial for test

	EXPECT_FALSE(poly.getDegree().isInteger());

	for(int i=0; i< amountOfEvals; i++){
		FieldElement x = generateRandom();		
		FieldElement polyRes = poly.eval(x);

		EXPECT_EQ(polyRes, (zero()));
	}
}

/**
 * Verifies that polynomial composition works
 * checks p(q) composition
 */
TEST(Algebra,UniPolyComposition){
	//Generate polynomial 
	UnivariatePolynomialGeneral p;
	UnivariatePolynomialGeneral q;
    for (int i=0; i<maxDegree; i++){
		p.setCoefficient(i,generateRandom());
		q.setCoefficient(i,generateRandom());
	}

	//generate composition polynomial
	UnivariatePolynomialInterface* pq = p.eval(q);
	
	/** compare evaluations */
	for(int i=0 ;i<amountOfEvals; i++){
		FieldElement x = generateRandom();
		FieldElement explicitRes = p.eval(q.eval(x));
		FieldElement pqRes = pq->eval(x);
		EXPECT_EQ(pqRes, explicitRes);
	}

	delete pq;
}

/**
 * Verifies the interpolation on a vector space
 * works good 
 */
TEST(Algebra,UniPolyInterpulation){
    //constants
    const int basisMaxSize = 10;

    //construct basis superset
    //This set will contain the basis
    //which will be exactly the first linearly independent
    //elements the iterator will go over
    elementsSet_t basis_supset;
    for(int i=0; i<basisMaxSize; i++){
        basis_supset.insert(generateRandom());
    }

    //construct evaluation map for the vector space
    //spanned by the basis (each coefficient is 0 or 1)
    //
    //while constructing the mapping, remove linearly dependent
    //elements from the basis set
    //do it by in every step looking at the next element
    //in the basis super set, if this element has already
    //an evaluation than it is dependent with the previous
    //elements, and will be skipped.
    //Otherwise, the element will be added to the basis set,
    //and we will find a random evaluation for all the elements
    //of the form <new basis element>+<element with existing evaluation>
    //notice the set of elements with existing evaluation is exactly the span of
    //the basis before the addition of the new basis element, this way we
    //expend the domain (of the mapping) to be the span of the extended basis
    elementsSet_t basis;
    UnivariatePolynomialGeneral::evaluation_t mapping;
    
    //The smallest vector space (of degree 0) is the {0} space
    //we start with an empty basis, and make an evaluation on it
    mapping[zero()] = generateRandom();
    
    for(const FieldElement& belem : basis_supset){
        //Check if this element is in the span of the current basis
        if(mapping.count(belem) > 0) continue;

        //otherwise, add to basis, and expand the mapping
        basis.insert(belem);
        for(const auto& point : mapping){
            //The if is just so we won't get into an infinite loop
            if(mapping.count(point.first + belem) == 0){
                mapping[point.first + belem] = generateRandom();
            }
        }
    }

    //Interpolate
    UnivariatePolynomialGeneral poly(mapping,basis);

    //Verify

    //verify degree
    EXPECT_TRUE(Algebra::PolynomialDegree::integral_t(poly.getDegree()) <= (int)mapping.size());

    //verify data
    for(const auto& point : mapping){
        EXPECT_EQ(point.second,poly.eval(point.first));
    }
}

} //anonymous namespace
