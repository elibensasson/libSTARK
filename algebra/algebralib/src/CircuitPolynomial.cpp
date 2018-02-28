#include <algebraLib/CircuitPolynomial.hpp>
#include <algebraLib/ErrorHandling.hpp>
#include <algorithm>


namespace Algebra{

CircuitPolynomial::CircuitPolynomial(const vector<LinearCombination>& factors) :
								operation_(PolynomialOperation::PROD), input_(), factors_(factors){
	ALGEBRALIB_ASSERT(factors_.size() > 0, "CircuitPolynomial c'tor: Need at least one factor in product");
}

CircuitPolynomial::CircuitPolynomial(const SelectorSum& S) : 
				operation_(PolynomialOperation::SELECTORSUM), S_(new SelectorSum(S)), input_(){}

CircuitPolynomial::CircuitPolynomial(const LinearTerm& linearTerm) :
    operation_(PolynomialOperation::PROD), input_(), factors_() {
	if (linearTerm.getCoeff() == Algebra::zero()) {
		factors_.emplace_back(LinearCombination(Algebra::zero()));
	}
	else {
		factors_.emplace_back(linearTerm);
	}
}


bool CircuitPolynomial::isZero() const {
	return operation_ == PolynomialOperation::PROD &&
		input_.size() == 0 &&
		factors_.size() == 1 && factors_[0].isZero();
}

CircuitPolynomial& CircuitPolynomial::operator+=(const CircuitPolynomial& other) {
	if (other.isZero()) {
		return *this;
	}
	if (isZero()) {
		*this = other;
		return *this;
	}
	if (operation_ == PolynomialOperation::ADD) {
		input_.emplace_back(other);
		return *this;
	}
	if (operation_ == PolynomialOperation::PROD &&
		other.operation_ == PolynomialOperation::PROD &&
		factors_.size() == 1 && other.factors_.size() == 1) {
		ALGEBRALIB_ASSERT(input_.size() == 0 && other.input_.size() == 0, 
					"CircuitPolynomial operator += : When the operation is PROD there should be no inputs ");
		factors_[0] += other.factors_[0];
		return *this;
	}
	CircuitPolynomial input1(*this);
	operation_ = PolynomialOperation::ADD;
	input_.clear();
	factors_.clear();
	input_.emplace_back(input1);
	input_.emplace_back(other);
	return *this;
}

CircuitPolynomial& CircuitPolynomial::operator*=(const CircuitPolynomial& other) {
	if (other.isZero() || isZero()) {
		input_.clear();
		factors_.clear();
		operation_ = PolynomialOperation::PROD;
		factors_.emplace_back(LinearCombination(Algebra::zero()));
		return *this;
	}
	if (operation_ == PolynomialOperation::PROD && operation_ == other.operation_) {
		for (const LinearCombination& l : other.factors_) {
			factors_.emplace_back(l);
		}
		return *this;
	}
	if (operation_ == PolynomialOperation::MUL) {
		input_.emplace_back(other);
		return *this;
	}
	CircuitPolynomial input1(*this);
	operation_ = PolynomialOperation::MUL;
	input_.clear();
	factors_.clear();
	input_.emplace_back(input1);
	input_.emplace_back(other);
	return *this;
}

std::string CircuitPolynomial::asString() const {
	if (operation_ == PolynomialOperation::SELECTORSUM) {
		return "(" + S_->asString() + ")";
	}
	std::string resString = "";
	if (operation_ == PolynomialOperation::PROD){
		ALGEBRALIB_ASSERT(factors_.size() > 0, "CircuitPolynomial asString - factors must have at least one LinearCombination");
		resString = factors_[0].asString();
		for (unsigned int i = 1; i < factors_.size(); i++){
			resString += "*" + factors_[i].asString();
		}
		return resString;
	}
	resString += "(";
	int inputSize = input_.size() - 1;
	for (auto& polynomial : input_){
		resString += polynomial.asString();
		if (inputSize != 0){
			switch (operation_) {
			case(PolynomialOperation::ADD) :
				resString += "+";
				break;
			case(PolynomialOperation::MUL) :
				resString += "*";
				break;
			default:
				ALGEBRALIB_FATAL("asString: Unknown polynomial operation");
				break;
			}
		}
		--inputSize;
	}
	resString += ")";
	return resString;
}

string CircuitPolynomial::asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const {
	if (operation_ == PolynomialOperation::SELECTORSUM) {
		return "(" + S_->asStringNewIndex(translate, isDegreePrint) + ")";
	}
	std::string resString = "";
	if (operation_ == PolynomialOperation::PROD){
		ALGEBRALIB_ASSERT(factors_.size() > 0, "CircuitPolynomial asStringNewIndex - factors must have at least one LinearCombination");
		resString = factors_[0].asStringNewIndex(translate, isDegreePrint);
		for (unsigned int i = 1; i < factors_.size(); i++){
			resString += "*" + factors_[i].asStringNewIndex(translate, isDegreePrint);
		}
		return resString;
	}
	resString += "(";
	int inputSize = input_.size() - 1;
	for (auto& polynomial : input_){
		resString += polynomial.asStringNewIndex(translate, isDegreePrint);
		if (inputSize != 0){
			switch (operation_) {
			case(PolynomialOperation::ADD) :
				resString += "+";
				break;
			case(PolynomialOperation::MUL) :
				resString += "*";
				break;
			default:
				ALGEBRALIB_FATAL("asString: Unknown polynomial operation");
				break;
			}
		}
		--inputSize;
	}
	resString += ")";
	return resString;
}

void CircuitPolynomial::getConsts(map<FElem, unsigned long long, classCompElements>& FElemMap) const {
	if (operation_ == PolynomialOperation::SELECTORSUM) {
		S_->getConsts(FElemMap);
	}
	if (operation_ == PolynomialOperation::PROD) {
		for (unsigned int i = 0; i < factors_.size(); i++) {
			FElem lconst = factors_[i].constant();
			if (FElemMap.find(lconst) == FElemMap.end()) {
				FElemMap[lconst] = mapFieldElementToInteger(0,64,lconst);
			}
			for (unsigned int j = 0; j < factors_[i].linearTerms_.size(); j++) {
				FElem coeff = factors_[i].linearTerms_[j].getCoeff();
				if (FElemMap.find(coeff) == FElemMap.end()) {
					FElemMap[coeff] = mapFieldElementToInteger(0, 64, coeff);
				}
			}
		}
		return;
	}
	if (operation_ == PolynomialOperation::MUL || operation_ == PolynomialOperation::ADD) {
		for (auto& polynomial : input_) {
			polynomial.getConsts(FElemMap);
		}
		return;
	}
}


//#define noSelector // flag to check how much time is taken by selectors
FElem CircuitPolynomial::eval(const std::vector<FElem>& assignment) const{
	if (operation_ == PolynomialOperation::SELECTORSUM) {
		return(S_->eval(assignment));
	}
	{
		FElem retVal = Algebra::one();
		if (operation_ == PolynomialOperation::PROD){
#ifndef noSelector//just added to measure how much time spent here
			for (const auto& factor : factors_)
				retVal *= factor.eval(assignment);
#endif
			return retVal;
		}
	}
	bool first = true;
	FElem retval;
	for (const auto& polynomial : input_){
		if (first){
			retval = polynomial.eval(assignment);
			first = false;
			continue;
		}
		switch (operation_)
		{
		case(PolynomialOperation::ADD) :
			retval += polynomial.eval(assignment);
			break;
		case(PolynomialOperation::MUL) :
			retval *= polynomial.eval(assignment);
			break;
		default:
			ALGEBRALIB_FATAL("DFSEval: Unknown polynomial operation");
			break;
		}
	}
	return retval;
}


FElem CircuitPolynomial::eval(const VariableAssignment& assignment) const{
	if (operation_ == PolynomialOperation::SELECTORSUM) {
		return S_->eval(assignment);
	}
	{
		FElem retVal = Algebra::one();
		if (operation_ == PolynomialOperation::PROD){
#ifndef noSelector//just added to measure how much time spent here
			for (const auto& factor : factors_)
				retVal *= factor.eval(assignment);
#endif
			return retVal;
		}
	}
	bool first = true;
	FElem retval;
	for (const auto& polynomial : input_){
		if (first){
			retval = polynomial.eval(assignment);
			first = false;
			continue;
		}
		switch (operation_) {
		case(PolynomialOperation::ADD) :
			retval += polynomial.eval(assignment);
			break;
		case(PolynomialOperation::MUL) :
			retval *= polynomial.eval(assignment);
			break;
		default:
			ALGEBRALIB_FATAL("DFSEval: Unknown polynomial operation");
			break;
		}
	}
	return retval;
}

size_t CircuitPolynomial::numVars() const{
    ALGEBRALIB_FATAL("Not implemented");
}

bool CircuitPolynomial::isSatisfied(const VariableAssignment& assignment) const{
	FElem val = eval(assignment);
	return (val == Algebra::zero());
}

	
Variable::set CircuitPolynomial::getUsedVariables() const{
	if (operation_ == PolynomialOperation::SELECTORSUM)
		return S_->getUsedVariables();
	Variable::set retSet;
	if (operation_ == PolynomialOperation::PROD) {
		for (const auto& linearComb : factors_){
			Variable::set polySet = linearComb.getUsedVariables();
			retSet.insert(polySet.begin(), polySet.end());
		}
	}
	else{
		for (const auto& poly : input_) {
			Variable::set polySet = poly.getUsedVariables();
            retSet.insert(polySet.begin(), polySet.end());
        }
	}
	return retSet;
}

void CircuitPolynomial::setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New){
	if (operation_ == PolynomialOperation::SELECTORSUM)
		S_->setNewIndices(old2New);

	if (operation_ == PolynomialOperation::PROD){
		for (auto& factor : factors_)
			factor.setNewIndices(old2New);
	}
	for (auto& polynomial : input_){
		polynomial.setNewIndices(old2New);
	}
}


PolynomialDegree CircuitPolynomial::getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees) const{
	if (operation_ == PolynomialOperation::SELECTORSUM) {
		return S_->getDegreeBound(inputDegrees);
	}
	
	PolynomialDegree maxDegree(0);

	if (operation_ == PolynomialOperation::PROD){
		for (const auto& factor : factors_){
			maxDegree = degreeOfProduct(maxDegree, factor.getDegreeBound(inputDegrees));
		}
		return maxDegree;
	}
	for (auto& poly : input_){
		PolynomialDegree polyDegree = poly.getDegreeBound(inputDegrees);
		switch (operation_) {
		case(PolynomialOperation::ADD) :
			maxDegree = std::max(maxDegree, polyDegree);
			break;
		case(PolynomialOperation::MUL) :
			maxDegree = degreeOfProduct(maxDegree, polyDegree);
			break;
		default:
			ALGEBRALIB_FATAL("getDegeeBound: Unknown polynomial operation");
			break;
		}
	}
	return maxDegree;
}

bool CircuitPolynomial::isEffectiveInput(const size_t varId) const {
	//return true;
	Variable::set usedVariables = this->getUsedVariables();
	for (const Variable& v : usedVariables){
		int id = v.getNewIndex();
		ALGEBRALIB_ASSERT(id >= 0, "new ID should be initialized and positive");
		if (varId == (unsigned int)id){
			return true;
		}
	}
	return false;
}
}
