#include <algebraLib/SelectorSum.hpp>
#include <algebraLib/ErrorHandling.hpp>
namespace Algebra{

	//useful auxiliary method to eval by assignment when only have eval by vector

	vector<FieldElement> assignmentToVector(const Algebra::VariableAssignment& assignment){
		vector<FieldElement> retVal(assignment.size() * 2);
		for (const auto& v : assignment){
			if (v.first.getIndex() > (int)retVal.capacity())
				retVal.resize(v.first.getIndex() * 2);
			retVal[v.first.getIndex()] = v.second;
		}
		return retVal;

	}

	SelectorSum::SelectorSum() {}

	SelectorSum::SelectorSum(const vector<CircuitPolynomial>& constraints, const vector<Variable>& selectorVariableVector, const vector<long>& selectorToConstraint,
		const vector<bool>& selectorRelevant) :
        constraints(constraints),
        selectorToConstraint(selectorToConstraint),
        selectorRelevant(selectorRelevant),
        selectorVarNum(selectorVariableVector.size()),
        selectorNum(selectorToConstraint.size()),
        constraintNum(constraints.size()),
		selectorNumBound(std::pow(2, selectorVariableVector.size())){
		ALGEBRALIB_ASSERT(selectorVariableVector.size() >0, "a variable vector of size 0");
		ALGEBRALIB_ASSERT(selectorToConstraint.size() == selectorRelevant.size(), "selectorToConstraint different size from selectorRelevant");

		for (unsigned int i = 0; i < selectorVariableVector.size(); i++){
			selectorVars.push_back(selectorVariableVector[i].getIndex());
			selectorVariables.insert(selectorVariableVector[i]);
		}
	}

	//if selectorRelevant vector not given as input, assume all selectors are relevant
	SelectorSum::SelectorSum(const vector<CircuitPolynomial>& constraints, const vector<Variable>& selectorVariableVector, const vector<long>& selectorToConstraint) :
		constraints(constraints),
        selectorToConstraint(selectorToConstraint),
        selectorVarNum(selectorVariableVector.size()),
        selectorNum(selectorToConstraint.size()),
        constraintNum(constraints.size()),
		selectorNumBound(std::pow(2, selectorVariableVector.size())) {

		ALGEBRALIB_ASSERT(selectorVariableVector.size() >0, "a variable vector of size 0");

		for (unsigned int i = 0; i < selectorVariableVector.size(); i++){
			selectorVars.push_back(selectorVariableVector[i].getIndex());
			selectorVariables.insert(selectorVariableVector[i]);
					}
		for (unsigned int i = 0; i < selectorToConstraint.size();i++)
			selectorRelevant.push_back(true);//if selectorRelevant vector not given as input, assume all selectors are relevant

	}



	//evaluates a polynomial on a given assignment
	FieldElement SelectorSum::eval(const vector<FieldElement>& x)const{
		vector<FieldElement> coeffs = getConstraintCoeffs(x);
		FieldElement returnVal = zero();
		for (auto i = 0; i < constraintNum; i++)
			returnVal += coeffs[i] * constraints[i].eval(x);
		return returnVal;

	}

	
	//evaluates a polynomial on a given assignment - the need to eval constraints[i] on assignment rather than x has to do with mess regarding double indices in Variable object
	FieldElement SelectorSum::eval(const VariableAssignment& assignment)const{
		vector<FieldElement> x = assignmentToVector(assignment);
		vector<FieldElement> coeffs = getConstraintCoeffs(x);
		FieldElement returnVal = zero();
		for (auto i = 0; i < constraintNum; i++)
			if (coeffs[i] != Algebra::zero())
			returnVal += coeffs[i] * constraints[i].eval(assignment);
		return returnVal;

	}

    size_t SelectorSum::numVars()const{
        ALGEBRALIB_FATAL("Not implemented");
    }

	//the need for this method has to do with different selectors having same P_i, as explained in top of hpp file
	vector<FieldElement> SelectorSum::getConstraintCoeffs(const vector<FieldElement>& x)const{
		vector<FieldElement> selectorVals = getSelectorVals(x);
		vector<FieldElement> coeffs;
		for (auto i = 0; i < constraintNum; i++)
			coeffs.push_back(zero());
		for (auto i = 0; i < selectorNum; i++)
			if (selectorRelevant[i])//not all SelectorSum objects actually use all possible selector vals
				coeffs[selectorToConstraint[i]] += selectorVals[i];
		return coeffs;
	}


	varSet SelectorSum::getUsedVariables()const {
		varSet retVal(selectorVariables);
		for (const auto& constraint : constraints){
			varSet v = constraint.getUsedVariables();
			retVal.insert(v.begin(), v.end());
		}
		return retVal;

	}

	vector<FieldElement> SelectorSum::getSelectorVals(const vector<FieldElement>& x)const {
		vector<FieldElement> selectorVals(selectorNumBound);
		selectorVals[0] = x[selectorVars[0]] + one();
		selectorVals[1] = x[selectorVars[0]];
		long curLength = 2;

		for (auto i = 1; i < selectorVarNum; i++){// in principle can do last  iteration of loop separately to avoid rounding up selectorNum to a power of 2
			for (auto j = 0; j < curLength; j++){//copying the current vector. Then Mult each element of first copy by newVar +1, each element of second copy by newVar
				//doing it in a `crooked' way to do just mult by newVar, and not also the mult by newVar +1
				selectorVals[j + curLength] = selectorVals[j] * x[selectorVars[i]];
				selectorVals[j] += selectorVals[j + curLength];
			}
			curLength *= 2;
		}
		////cout << "selectorVals:";
		//for (auto i = 0; i < selectorVals.size(); i++)
		////	cout << selectorVals[i];
		//cout << endl;
		return selectorVals;
	}

	void SelectorSum::getConsts(map<FieldElement, unsigned long long, Algebra::classCompElements>& FElemMap) const {
		for (unsigned int i = 0; i < constraints.size(); i++){
			constraints[i].getConsts(FElemMap);
		}
	}

	string SelectorSum::asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const {
		string res = "";
		bool startSelectorSum = true;
		for (unsigned int i = 0; i < constraints.size(); i++){
			bool firstSelector = true;
			for (auto j = 0; j < selectorNum; j++) {
				if (selectorRelevant[j] && (selectorToConstraint[j] == i)) {
					if (!firstSelector) {
						res += " + ";
					}
					else {
						if (startSelectorSum) {
							res += "(";
							startSelectorSum = false;
						}
						else {
							res += " + ("; 
						}
						firstSelector = false;
					}
					int k = j;
					for (unsigned int counter = 0; counter < selectorVars.size(); counter++) {
						int lsb = k & 1;
						if (isDegreePrint) {
							res += "(x[" + std::to_string(selectorVars[counter]) + "] + tmp_deg_type(0))";
						}
						else {
							res += "(x[" + std::to_string(selectorVars[counter]) + "] + " + (lsb == 0 ? "FElem1)" : "FElem0)");
						}
						k >>= 1;
						if (counter + 1  < selectorVars.size()){
							res += " * ";
						}
					}
				}
			}
			if (!firstSelector) {
				res += ") * ";
				res += constraints[i].asStringNewIndex(translate, isDegreePrint);
			}
		}
		return res;
	}

	string SelectorSum::asString() const{
		string res = "";
		for (unsigned int i = 0; i < constraints.size(); i++){
			res += "(";//first printing the selectors multiplying the i'th constraint
			for (auto j = 0; j < selectorNum; j++){
				if (selectorRelevant[j] && (selectorToConstraint[j] == i))
					res += "S_" + std::to_string(j);
			}
			res += ")";
			res += constraints[i].asString();
			res += " + ";
		}
		return res;
	}

	//the need for this method, is because variable indices are reset in cs2BREX reduction, before passing constraints to BREX
	void SelectorSum::setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New){
		for (unsigned int i = 0; i < selectorVars.size(); i++)
			selectorVars[i] = old2New[selectorVars[i]];
	
		varSet newSelectorVariables;//this somewhat silly intermediate variable, is because (at least VS) compiler insists there is problem when using auto& below
		for (auto var : selectorVariables){
			var.setNewIndex(old2New[var.getIndex()]);
			newSelectorVariables.insert(var);
		}
		selectorVariables = newSelectorVariables;
		for (auto& constraint : constraints)
			constraint.setNewIndices(old2New);
	}


	PolynomialDegree SelectorSum::getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees)const {
		PolynomialDegree selectorDeg(getSelectorDegreeBound(inputDegrees));
		PolynomialDegree maxDeg(-1);
		for (auto i = 0; i < constraintNum; i++){
			maxDeg = max(maxDeg, constraints[i].getDegreeBound(inputDegrees));
				}
		return degreeOfProduct(selectorDeg,maxDeg);
	}


	PolynomialDegree SelectorSum::getSelectorDegreeBound(const std::vector<PolynomialDegree>& inputDegrees)const {
		PolynomialDegree prodDegree(0);
		for (auto j = 0; j < selectorVarNum; j++){
			prodDegree = degreeOfProduct(prodDegree, inputDegrees[selectorVars[j]]);

		}
		return prodDegree;
	}

	unique_ptr<PolynomialInterface> SelectorSum::clone() const{
		return unique_ptr<PolynomialInterface>(new SelectorSum(*this));

	}





	



}//namespace Algebra
