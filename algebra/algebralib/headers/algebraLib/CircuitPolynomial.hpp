#ifndef ALGEBRA_CIRCUITPOLYNOMIAL_HPP_
#define ALGEBRA_CIRCUITPOLYNOMIAL_HPP_

#include <algebraLib/variable.hpp>
#include <algebraLib/PolynomialDegree.hpp>
#include <algebraLib/PolynomialInterface.hpp>
#include <algebraLib/SelectorSum.hpp>
#include <map>

using std::vector;
using std::string;

namespace Algebra{

class CircuitPolynomial;
class SelectorSum;

enum class PolynomialOperation{
	ADD,
	MUL,
	PROD,
	SELECTORSUM
};

class CircuitPolynomial : public PolynomialInterface{
private:
	PolynomialOperation operation_;
	std::shared_ptr<SelectorSum> S_;
	vector<CircuitPolynomial > input_;
	vector<LinearCombination> factors_; 
public:
	CircuitPolynomial() : operation_(PolynomialOperation::PROD), input_(), factors_(1, LinearCombination(Algebra::zero())){}
	CircuitPolynomial(const FElem& val) : operation_(PolynomialOperation::PROD), input_(), factors_(1, LinearCombination(val)) {}
	CircuitPolynomial(const Variable& var) : operation_(PolynomialOperation::PROD), input_(), factors_(1, LinearCombination(var)) {}
	CircuitPolynomial(const LinearTerm& linearTerm);
	CircuitPolynomial(const LinearCombination& linearCombination) : operation_(PolynomialOperation::PROD), input_(), factors_(1, linearCombination) {}
	CircuitPolynomial(const SelectorSum& S);
	CircuitPolynomial(const vector<LinearCombination>& factors);
		
	bool isZero() const;
	CircuitPolynomial& operator+=(const CircuitPolynomial& other);
	CircuitPolynomial& operator*=(const CircuitPolynomial& other);
	PolynomialOperation getOperation() const{ return operation_; }
	string asString() const;
	string asStringNewIndex(map<FElem, unsigned long long, classCompElements>& translate, bool isDegreePrint) const;
	void getConsts(std::map<FElem, unsigned long long, classCompElements>& FElemMap) const;
	FElem eval(const std::vector<FElem>& assignment) const;
	FElem eval(const VariableAssignment& assignment) const;
	bool isSatisfied(const VariableAssignment& assignment) const;
	Variable::set getUsedVariables() const;
	void setNewIndices(std::map<VarIndex_t, VarIndex_t>& old2New);
	PolynomialDegree getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees) const;
	bool isEffectiveInput(const size_t varId) const; 
	std::unique_ptr<PolynomialInterface> clone() const { return std::unique_ptr<PolynomialInterface>(new CircuitPolynomial(*this)); }
	size_t numVars()const;
    ~CircuitPolynomial(){};	
};

} 
#endif // ALGEBRA_CIRCUITPOLYNOMIAL_HPP_
