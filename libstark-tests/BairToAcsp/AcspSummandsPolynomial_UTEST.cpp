#include "lightCircLib/lightCircPoly.hpp"
#include <reductions/BairToAcsp/BairToAcsp.hpp>
#include <reductions/BairToAcsp/Details/AcspSummandsPolynomial.hpp>
#include <algebraLib/FieldElement.hpp>
#include <gtest/gtest.h>

#include <random>
namespace{
	using namespace Algebra;
	using Algebra::FieldElement;
	using Algebra::elementsSet_t;
	using Algebra::UnivariatePolynomialGeneral;
	using Algebra::generateRandom;
	using libstark::lightCircPoly;
	using libstark::BairToAcsp::AcspSummandsPolynomial;
	typedef  std::unique_ptr<const Algebra::PolynomialInterface> Constraint;  //currently we just have the constraints be general polynomials, but perhaps will change later on
	typedef std::vector<Algebra::FieldElement>  EvaluationPoint;
	typedef libstark::BairToAcsp::AcspSummandsPolynomial::polyID polyID;
	typedef std::vector<Algebra::SubspacePolynomial> SubspacePolyVec;
	typedef std::pair<polyID, Algebra::FieldElement>  AuxPoly;   //the auxiliary polynomials are represented by (an ID of a) subspace polynomial, and its shift
	typedef std::vector<AuxPoly> AuxPolyVec;
	typedef Algebra::UnivariatePolynomialInterface uniPoly;
	using std::move;
	using std::unique_ptr;
	using std::vector;
	using std::cout;
	

#define SET_SIZE 11
#define NUM_OF_SUMMANDS 200
#define NUM_OF_NEIGHBORS 20
	/***Auxiliary functions to be used in tests **/

	//creates a set containing only the 0 field element
	elementsSet_t zeroSet(){
		elementsSet_t set;
		set.insert(zero());
		return set;
	}

	//random bivariate poly of form c1*x + c2*y
	unique_ptr<lightCircPoly> randPoly(){

		//construct as univariate polynomial
		UnivariatePolynomialGeneral idPoly;
		idPoly.setCoefficient(1, generateRandom());

		//construct and return the circuitPolynomial
		unique_ptr<lightCircPoly> res(new lightCircPoly(idPoly));
		idPoly.setCoefficient(1, generateRandom());
		unique_ptr<lightCircPoly> res2(new lightCircPoly(idPoly));
		res->addDistinct(*res2);
		return res;
	}
	
    //	returns a random set of elements of size SET_SIZE
elementsSet_t randSet(){
		elementsSet_t spanSet;
		
		for (int i = 0; i < SET_SIZE; i++){
			spanSet.insert(generateRandom());
		}

		return spanSet;
	}

//
//
//******TESTS*******//

	//checking AcspSummandsPolynomial with one summand of form Z_H/X computes correctly
	TEST(Acsp, summandPolyBasic){
		
        elementsSet_t spanSet= randSet();

		FieldElement a;

		SubspacePolynomial Z_H(spanSet);
		AcspSummandsPolynomial A(spanSet);
		spanSet = elementsSet_t();
		spanSet.insert(zero());
		SubspacePolynomial S(spanSet);//the polynomial X
		polyID i = A.addSubspacePoly(spanSet);
		UnivariatePolynomialGeneral C(one());
		Constraint P(new UnivariatePolynomialGeneral(one()));

		A.addSummand(i, zero(), *P);
		a = generateRandom();
		FieldElement c = A.eval({ FieldElement(a) });
		FieldElement d = Z_H.eval(a) / a;

		EXPECT_EQ(c, d);
	}


	//Testing copy constructor of class
	TEST(Acsp, summandPolyCopy){

		elementsSet_t spanSet = randSet();

		FieldElement a;

		SubspacePolynomial Z_H(spanSet);
		AcspSummandsPolynomial A(spanSet);
		spanSet = elementsSet_t();
		spanSet.insert(zero());
		SubspacePolynomial S(spanSet);//the polynomial X
		polyID i = A.addSubspacePoly(spanSet);
		UnivariatePolynomialGeneral C(one());
		Constraint P(new UnivariatePolynomialGeneral(one()));

		A.addSummand(i, zero(), *P);
		AcspSummandsPolynomial B(A);
		FieldElement d = Z_H.eval(one()) / one();
		FieldElement c, e;
		
        c = A.eval({ one() });
        e = B.eval({ one() });
		
		EXPECT_EQ(c, d);
		EXPECT_EQ(e, d);

	}




	//checking a poly with constraint that are NUM_OF_SUMMANDS random linear bi variate terms computes correctly, random Z_H, Z_S =X, c =0
	TEST(Acsp, summandPolyNoConstInDenum){

		//generating the rand subspace Z_H
		elementsSet_t spanSet= randSet();
		FieldElement a;

		SubspacePolynomial Z_H(spanSet);
		AcspSummandsPolynomial A(spanSet);
		
		polyID Z_S = A.addSubspacePoly(zeroSet());
		a = generateRandom();
		FieldElement res = zero();
		Constraint C;
		vector<FieldElement> x;
		for (int i = 0; i < NUM_OF_SUMMANDS; i++){
			C = move(randPoly());
			x = { a, one() };
			res += C->eval(x);
			A.addSummand(Z_S, zero(), *C);

		}
		res *= Z_H.eval(a)/a;


		//		Constraint P(randPoly());

		
		FieldElement c = A.eval({ a, one()});
		//std::cout << c;
		EXPECT_EQ(c, res);


	}

	//checking a poly with constraint that are random linear bi variate terms computes correctly, random Z_H, Z_S  subspace of Z_H of varying size,, c =Z_S(1)
	TEST(Acsp, summandPolyConstInDenum){

		std::default_random_engine generator;
		std::uniform_int_distribution<int> rand(0, SET_SIZE - 1);
		//generating the rand subspace Z_H
		elementsSet_t spanSet=randSet();
		spanSet.insert(one());//need to add 1 to the span - cause want to use it as shift - which has to be in H

		vector<elementsSet_t> subspaceSets(SET_SIZE);

		FieldElement a;
		vector<polyID> Z_S;
		for (int i = 0; i < SET_SIZE; i++){
			a = generateRandom();
			spanSet.insert(a);
			for (int j = 0; j <= i; j++){
				subspaceSets[j].insert(a);
			}

		}
		SubspacePolynomial Z_H(spanSet);
		AcspSummandsPolynomial A(spanSet);
		std::vector<SubspacePolynomial> subspacePolys;
		for (int i = 0; i < SET_SIZE; i++){
			subspacePolys.push_back(SubspacePolynomial(subspaceSets[i]));
			Z_S.push_back(A.addSubspacePoly(subspaceSets[i]));

		}

		vector<Constraint> V;
		a = generateRandom();
		do{
			a = generateRandom();
			} while (Z_H.eval(a) == zero());

		FieldElement res = zero();
		Constraint C;
		vector<FieldElement> x;
		for (int i = 0; i < NUM_OF_SUMMANDS; i++){


			C = move(randPoly());
			x = { a, one() };
			res += C->eval(x)*((subspacePolys[i % SET_SIZE].eval(a+one()) ).inverse());
			A.addSummand(Z_S[i % SET_SIZE], one(), *C);

		}
		res *= Z_H.eval(a);


		//		Constraint P(randPoly());


		FieldElement c = A.eval({ a, one() });
		//std::cout << c << "rand element" << generateRandom();
	EXPECT_EQ(c, res);


	}

	//checking a poly with constraint that are 10 random linear bi variate terms computes correctly, random Z_H, Z_{S_i}  subspace of Z_H of varying size, varying c_i
	//varying aux polys in each summand also (aux polys just have zero shifts)
	TEST(Acsp, summandPolyDiffAuxPolys){

		elementsSet_t spanSet;
		vector<elementsSet_t> subspaceSets(SET_SIZE);

		FieldElement a = generateRandom();;
		vector<polyID> Z_S;
		
		FieldElement c = a;
		for (int i = 0; i < SET_SIZE; i++){
			
			spanSet.insert(a);
			for (int j = 0; j <= i; j++){
				subspaceSets[j].insert(a);
			}
			a = generateRandom();

		}
		
		SubspacePolynomial Z_H(spanSet);
		AcspSummandsPolynomial A(spanSet);
		std::vector<SubspacePolynomial> subspacePolys;
		for (int i = 0; i < SET_SIZE; i++){
			subspacePolys.push_back(SubspacePolynomial(subspaceSets[i]));
			Z_S.push_back(A.addSubspacePoly(subspaceSets[i]));

		}
		
		vector<Constraint> V;
		a = generateRandom();
		FieldElement y = generateRandom();
		FieldElement res = zero(), auxProd;
		Constraint C;
		vector<polyID>::const_iterator last, first = Z_S.begin(); //needed in loop to copy appropriate subvector of subspace polys
		vector<polyID> subVec;
		int pos;
		vector<FieldElement> x;
		for (int i = 0; i < NUM_OF_SUMMANDS; i++){

			C = move(randPoly());
			auxProd = one();
			pos = i % SET_SIZE;
			for (int j = 0; j < pos + 1; j++){
				auxProd *= subspacePolys[j].eval(a);
			}
			x = { a, y };
			res += C->eval(x)*((subspacePolys[pos].eval(a + c)).inverse())*auxProd;
			last = Z_S.begin() + pos + 1;
			subVec = vector<polyID>(first, last);
			AuxPolyVec e;
			for (unsigned int j = 0; j < subVec.size(); j++){
				e.push_back(std::make_pair(subVec[j], zero()));

			}
			A.addSummand(Z_S[i % SET_SIZE], c, *C, e);

		}
		res *= Z_H.eval(a);
		c = A.eval({ a, y });
		//std::cout << "result:" << c;
		//check the poly is computed correctly on the random point (a,y)
		EXPECT_EQ(c, res);
	}
}
