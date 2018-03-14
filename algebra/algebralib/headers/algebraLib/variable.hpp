#ifndef ALGEBRALIB_VARIABLE_HPP_
#define ALGEBRALIB_VARIABLE_HPP_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "PolynomialDegree.hpp"
#include "FieldElement.hpp"

using std::map;

namespace Algebra{

//Forward Declaration
class Variable;
class VariableArray;

// Typedef
typedef FieldElement FElem;
typedef long VarIndex_t;
typedef ::std::vector<Variable> VariableArrayContents;
typedef Variable FlagVariable; //Holds variable whose purpose is to be populated with a boolean value, Field(0) or Field(1)

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                    class Variable                          ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

/**
Each variable is specified by an index. This can be imagined as the index in x_1, x_2,..., x_i
These are formal variables and do not hold an assignment, later the class VariableAssignment
will give each formal variable its own assignment.
*/

class Variable {
private:
	static VarIndex_t nextFreeIndex_;
	VarIndex_t index_; //This index differentiates and identifies Variable instances.
	VarIndex_t newIndex_; // A "renaming" which is done before the reduction to BREX
	::std::string name_;

public:
	explicit Variable(const ::std::string& name = "");
	::std::string name() const;
	::std::string newIndexName() const;
	bool operator==(const Variable& var) const {return this->index_ == var.index_;}
	struct VariableStrictOrder {
		bool operator()(const Variable& first, const Variable& second)const {
			return first.index_ < second.index_;
		}
	};
	typedef ::std::map<Variable, FElem, Variable::VariableStrictOrder> VariableAssignment;
	FElem eval(const VariableAssignment& assignment) const;
	FElem eval(const std::vector<FElem>& assignment) const;
	typedef ::std::set<Variable, VariableStrictOrder> set;
	typedef ::std::multiset<Variable, VariableStrictOrder> multiset;

	VarIndex_t getIndex() const { return index_; }
	VarIndex_t getNewIndex() const;
	void setNewIndex(VarIndex_t newIndex);

	friend class CircuitPolynomial;
}; // class Variable

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/
typedef ::std::map<Variable, FElem, Variable::VariableStrictOrder> VariableAssignment;

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 class VariableArray                        ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class VariableArray : public VariableArrayContents {
private:
	::std::string name_;
public:
	explicit VariableArray(const ::std::string& name = "");
	explicit VariableArray(const int size, const ::std::string& name = "");
	explicit VariableArray(const size_t size, const Variable& contents)
		: VariableArrayContents(size, contents) {}

	using VariableArrayContents::operator[];
	using VariableArrayContents::at;
	using VariableArrayContents::push_back;
	using VariableArrayContents::size;
	using VariableArrayContents::emplace_back;

	::std::string name() const;
}; // class VariableArray

class UnpackedWord : public VariableArray {
public:
	UnpackedWord() : VariableArray() {}
	UnpackedWord(const size_t numBits, const ::std::string& name) : VariableArray(numBits, name) {}
}; // class UnpackedWord

typedef ::std::vector<UnpackedWord> UnpackedWordArray;

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                     class LinearTerm                       ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class LinearTerm {
private:
	Variable variable_;
	FElem coeff_;
public:
	LinearTerm(const Variable& v) : variable_(v), coeff_(Algebra::one()) {}
	LinearTerm(const Variable& v, const FElem& coeff) : variable_(v), coeff_(coeff) {}
	LinearTerm& operator*=(const FElem& other) { coeff_ *= other; return *this; }
	::std::string asString() const;
	::std::string asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const;
	FElem eval(const VariableAssignment& assignment) const;
	FElem eval(const std::vector<FElem>& assignment) const;
	Variable variable() const { return variable_; }
	FElem getCoeff() const { return coeff_; }
	void setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New);

}; // class LinearTerm

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

class LinearCombination {
protected:
	::std::vector<LinearTerm> linearTerms_;
	FElem constant_;
public:
	LinearCombination() : linearTerms_(), constant_(Algebra::zero()) {}
	LinearCombination(const Variable& var) : linearTerms_(1, var), constant_(Algebra::zero()) {}
	LinearCombination(const LinearTerm& linearTerm);
	LinearCombination(const FElem& elem) : linearTerms_(), constant_(elem) {}

	LinearCombination& operator+=(const LinearCombination& other);
	LinearCombination& operator*=(const FElem& other);
	FElem eval(const VariableAssignment& assignment) const;
	FElem eval(const std::vector<FElem>& assignment) const;
	::std::string asString() const;
	::std::string asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const;
	const Variable::set getUsedVariables() const;
	void setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New);
	PolynomialDegree getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees) const;
	bool isZero() const { return linearTerms_.size() == 0 && constant_ == Algebra::zero(); }
	FElem constant() const { return constant_; }

	friend class CircuitPolynomial;
}; // class LinearCombination

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

} //namespace Algebra




#endif //ALGEBRALIB_VARIABLE_HPP_
