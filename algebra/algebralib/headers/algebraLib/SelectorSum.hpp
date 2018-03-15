#ifndef ALGEBRA__SELECTOR_SUM_HPP
#define ALGEBRA__SELECTOR_SUM_HPP

/* SelectorSum.hpp
 * The purpose of this class is to have an efficient way of computing the types of polynomials that come up in the TinyRam -> BREX reduction
 * In general , this will be sums over i of S_i*P_i where
 * S_i - is a `multivariate selector of the binary representation of i'. Specifically, if (b1,..,bm) is the binary rep of i,
 * then S_i = (X_1+b1+1)*..*(X_m + bm + 1) . Note that, assuming (X1,..,Xm) are boolean, S_i does not vanish IFF X_1=b_1,..,X_m=b_m
 * P_i - a general constraint, but almost always at most a product of linear combinations.
 * There is a complication to this description: Typically, there are less distinct P_i's than S_i's.
 * So it's worthwhile to store the distinct P_i's, and a map that tells us for each j, what P_i S_j should multiply. 
 * This is the purpose of the vector selectorToConstraint. This way we compute, e.g., (S_3 + S_5)*P_2 rather than S_3*P_2 + S_5*P_2 - i.e., num of multiplications
 * is like num of distinct constraints
 */

#include <algebraLib/PolynomialInterface.hpp>
#include <algebraLib/CircuitPolynomial.hpp>
#include <algebraLib/variable.hpp>
using namespace std;
typedef  Algebra::Variable::set  varSet;
namespace Algebra{
	class CircuitPolynomial;
	class SelectorSum : public PolynomialInterface
	{
	public:
		SelectorSum();
		SelectorSum(const vector<CircuitPolynomial>& constraints, const vector<Variable>& selectorVariableVector, const vector<long>& selectorToConstraint,
			const vector<bool>& selectorRelevant);

		//if selectorRelevant vector not given as input,assume all selectors are relevant
		SelectorSum(const vector<CircuitPolynomial>& constraints, const vector<Variable>& selectorVariableVector, const vector<long>& selectorToConstraint);
				


		//evaluates a polynomial on a given assignment
		FieldElement eval(const vector<FieldElement>& x)const;
		
		//evaluates a polynomial on a given assignment. This one's implemented just to be compatible with old tests - implemented somewhat inefficiently!
		FieldElement eval(const VariableAssignment& x)const;

        size_t numVars() const;

		void getConsts(map<FieldElement, unsigned long long, Algebra::classCompElements>& FElemMap) const;
		string asString() const;
		string asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const;
		//the need for this method, is because variable indices are reset in cs2BREX reduction, before passing constraints to BREX
		void setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New);


		PolynomialDegree getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees)const;

		varSet getUsedVariables()const;

		PolynomialDegree getSelectorDegreeBound(const std::vector<PolynomialDegree>& inputDegrees)const;
		unique_ptr<PolynomialInterface> clone() const;

	private:

		//the need for this method has to do with different selectors having same P_i, as explaing in top of hpp file
		vector<FieldElement> getConstraintCoeffs(const vector<FieldElement>& x)const;		
		vector<FieldElement> getSelectorVals(const vector<FieldElement>& x)const;
		vector <CircuitPolynomial> constraints;
		vector<VarIndex_t> selectorVars;
		varSet selectorVariables;//need to store this in addition to one above is to be able to implement the getUsedVariables method - needed for SelectorSum class to 
										//inside CircuitPolynomial
		vector<long> selectorToConstraint; //v[i] tells us what constraint S_i should multiply
		vector<bool> selectorRelevant; //v[i] tells us if i'th selector S_i really used in this polynomial
		//vector<FieldElement> selectorVals;
		long selectorVarNum, selectorNum, constraintNum;
		long selectorNumBound=(long)std::pow(2, selectorVarNum);
	};
}

#endif // ALGEBRA__SELECTOR_SUM_HPP
