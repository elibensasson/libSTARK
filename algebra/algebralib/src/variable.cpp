#include "algebraLib/variable.hpp"
#include "algebraLib/ErrorHandling.hpp"

#include <limits.h>
#include <algorithm>

using std::vector;

namespace Algebra {
	
	
/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                    class Variable                          ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
VarIndex_t Variable::nextFreeIndex_ = 0;

Variable::Variable(const std::string& name) :
    index_(nextFreeIndex_++),
    newIndex_(-1),
    name_(name){
	ALGEBRALIB_ASSERT(nextFreeIndex_ > 0, ALGEBRALIB_FMT("Variable index overflow has occurred, maximum number of "
		"Variables is %lu", ULONG_MAX));
}

std::string Variable::name() const { return name_; }

::std::string Variable::newIndexName() const {
	ALGEBRALIB_ASSERT(newIndex_ >= 0, "Variable eval: newIndex_ is negative");
	return "x[" + std::to_string(newIndex_) + "]";
}

FElem Variable::eval(const VariableAssignment& assignment) const {
	try {
		return assignment.at(*this);
	}
	catch (::std::out_of_range) {
		ALGEBRALIB_FATAL(ALGEBRALIB_FMT("Attempted to evaluate unassigned Variable \"%s\", idx:%lu", name().c_str(), index_));
	}
}

FElem Variable::eval(const std::vector<FElem>& assignment) const {
	ALGEBRALIB_ASSERT(newIndex_ >= 0, "Variable eval: newIndex_ is negative");
	ALGEBRALIB_ASSERT(newIndex_  < (int)assignment.size(), "Variable eval: not enough FElems in the vector");
	return assignment[newIndex_];
}

VarIndex_t Variable::getNewIndex() const {
	//ALGEBRALIB_ASSERT(newIndex_ < 0, "trying to getNewIndex from variable when it is negative - meaning probably not initialized");
	return newIndex_;
}

void Variable::setNewIndex(VarIndex_t newIndex){
	ALGEBRALIB_ASSERT(newIndex >= 0, "Variable eval: newIndex_ is negative");
	newIndex_ = newIndex;
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 class VariableArray                        ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/


VariableArray::VariableArray(const ::std::string& name) : VariableArrayContents(), name_(name) {}
VariableArray::VariableArray(const int size, const ::std::string& name) : VariableArrayContents() {
	for (int i = 0; i < size; ++i) {
		std::string variableName = name + "[" + std::to_string(i) + "]";
		push_back(Variable(variableName));
	}
}
::std::string VariableArray::name() const {
	return name_;
}


/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                    class LinearTerm                        ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

::std::string LinearTerm::asString() const {
	if (coeff_ == Algebra::one()) { return variable_.name(); }
	else if (coeff_ == Algebra::zero()) { return Algebra::zero().asString(); }
	else { return coeff_.asString() + "*" + variable_.name(); }
}

::std::string LinearTerm::asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const {	
	ALGEBRALIB_ASSERT(translate.find(coeff_) != translate.end(), "LinearTerm: Coeff doesn't exist in the translation vector");
	if (coeff_ == Algebra::one()) { return variable_.newIndexName(); }
	if (coeff_ == Algebra::zero()) { 
		if (isDegreePrint) {
			return "tmp_deg_type(0)";
		}
		return "FElem0"; }
	else { 
		if (isDegreePrint) {
			return "tmp_deg_type(0) * " + variable_.newIndexName();
		}
		return "FElem" + std::to_string(translate[coeff_]) + "*" + variable_.newIndexName(); }
}

FElem LinearTerm::eval(const VariableAssignment& assignment) const {
	return coeff_ * variable_.eval(assignment);
}

FElem LinearTerm::eval(const std::vector<FElem>& assignment) const {
	return coeff_ * variable_.eval(assignment);
}	

void LinearTerm::setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New){
	variable_.setNewIndex(old2New[variable_.getIndex()]);
}

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                  class LinearCombination                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

LinearCombination::LinearCombination(const LinearTerm& linearTerm) : linearTerms_(), constant_(Algebra::zero()) {
	if (linearTerm.getCoeff() != Algebra::zero()) {
		linearTerms_.emplace_back(linearTerm);
	}
}

LinearCombination& LinearCombination::operator+=(const LinearCombination& other) {
	vector<LinearTerm> lt;
	for (const LinearTerm& l : linearTerms_) {
		if (l.getCoeff() != Algebra::zero()) {
			lt.emplace_back(l);
		}
	}
	for (const LinearTerm& l : other.linearTerms_) {
		if (l.getCoeff() != Algebra::zero()) {
			lt.emplace_back(l);
		}
	}
	linearTerms_ = lt;
	constant_ += other.constant_;
	return *this;
}

LinearCombination& LinearCombination::operator*=(const FElem& other) {
	if (other == Algebra::zero()) {
		linearTerms_.clear();
		constant_ = Algebra::zero();
		return *this;
	}
	constant_ *= other;
	for (LinearTerm& lt : linearTerms_) {
		lt *= other;
	}
	return *this;
}

FElem LinearCombination::eval(const VariableAssignment& assignment) const {
	FElem evaluation = constant_;
	for (const LinearTerm& lt : linearTerms_) {
		evaluation += lt.eval(assignment);
	}
	return evaluation;
}

FElem LinearCombination::eval(const std::vector<FElem>& assignment) const{
	FElem evaluation = constant_;
	for (const LinearTerm& lt : linearTerms_) {
		evaluation += lt.eval(assignment);
	}
	return evaluation;
}

std::string LinearCombination::asString() const {
	if (linearTerms_.size() == 0 ) {
		return "(" + constant_.asString() + ")";
	}
	else {
		std::string retval = "(";
		unsigned int i;
		for (i = 0; i+1 < linearTerms_.size(); i++) {
			retval += linearTerms_[i].asString() + "+";
		}
		retval += linearTerms_[i].asString();
		if (constant_ != Algebra::zero()) {
			retval += "+"  + constant_.asString();
		}
		retval += ")";
		return retval;
	}
}

std::string LinearCombination::asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const {
	if (linearTerms_.size() == 0) {
		ALGEBRALIB_ASSERT(translate.find(constant_) != translate.end(), "LinearCombination: Coeff doesn't exist in the translation vector");
		if (isDegreePrint) {
			return "(tmp_deg_type(0))";
		}
		return "(FElem" + std::to_string(translate[constant_])  +")";
	}
	else {
		std::string retval = "(";
		unsigned int i;
		for (i = 0; i+1 < linearTerms_.size(); i++) {
			retval += linearTerms_[i].asStringNewIndex(translate,isDegreePrint) + "+";
		}
		retval += linearTerms_[i].asStringNewIndex(translate, isDegreePrint);
		if (constant_ != Algebra::zero()) {
			if (isDegreePrint) {
				retval += " + tmp_deg_type(0)";
			}
			else {
				retval += " + FElem" + std::to_string(translate[constant_]);
			}
					}
		retval += ")";
		return retval;
	}
}

const Variable::set LinearCombination::getUsedVariables() const {
	Variable::set retSet;
	for (const LinearTerm& lt : linearTerms_) {
		retSet.insert(lt.variable());
	}
	return retSet;
}

void LinearCombination::setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New){
	for (LinearTerm& l : linearTerms_){
		l.setNewIndices(old2New);
	}
}

PolynomialDegree LinearCombination::getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees) const{
	PolynomialDegree maxDegree(0);
	for (const auto& t : linearTerms_){
		maxDegree = std::max(maxDegree, inputDegrees[t.variable().getNewIndex()]);
	}
	return maxDegree;
}
/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/


} //namespace
