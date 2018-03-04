#include "lightCircGate.hpp"
#include <algorithm>

namespace libstark {
namespace LightCircLib {

using std::vector;
using std::set;
using std::function;
using std::max;

using Algebra::zero;
using Algebra::one;
using Algebra::FieldElement;
using Algebra::PolynomialDegree;

/********************************************************************************/
/********************************************************************************/
/********************** lightCircGate Implementation ***********************/
/********************************************************************************/
/********************************************************************************/

/****************************************/
/******* Constructors and Getters *******/
/****************************************/

/** Adds a new gate as input for this gate. */
void lightCircGate::addInputGate(lightCircGate* newInput) { 
	m_inputs.push_back(newInput);
}

void lightCircGate::setInputGate(lightCircGate* newInput) { 
	vector<lightCircGate*> newInputVec;
    newInputVec.push_back(newInput);
    m_inputs = newInputVec;
}

FieldElement lightCircGate::evaluateGate(lightCircGate::evaluationMap_t& evaluations)const {
    ///Case 1 - The gate has been evaluated already.
    if (evaluations.count(this) > 0) {
		return evaluations[this];
	}

	///Case 2 - Evaluating the gate:
    ///(not really evaluation, use lazy evaluation)
    class lazyAssignment : public LazyVector<FieldElement>{
        public:
            lazyAssignment(lightCircGate::evaluationMap_t& evaluations,const vector<lightCircGate*>& inputs):
                evaluations_(evaluations), inputs_(inputs){};
            
            FieldElement getElementByIndex(Sequence::index_t index) const{
                return inputs_[index]->evaluateGate(evaluations_);
            }

			Sequence::index_t size()const {
                return inputs_.size();
            }

        private:
            lightCircGate::evaluationMap_t& evaluations_;
            const vector<lightCircGate*>& inputs_;
    };
    lazyAssignment lazyAss(evaluations, m_inputs);

	evaluations.emplace(this,this->calculateEvaluation(lazyAss));
	return evaluations[this];
}

PolynomialDegree lightCircGate::getDegree(lightCircGate::degreeCalcMap_t& degrees)const{
    ///Case 1 - The gate has been evaluated already.
    if (degrees.count(this) > 0) {
		return degrees.find(this)->second;
	}

	///Case 2 - Evaluating the gate:
    vector<PolynomialDegree> inputDegrees;
    for(const auto inp_gate : m_inputs){
        inputDegrees.push_back(inp_gate->getDegree(degrees));
    }
	
    degrees.emplace(this,this->calculateDegree(inputDegrees));
	return degrees.find(this)->second;
}

/********************************************************************************/
/********************************************************************************/
/********************* lightAdditionGate Implementation ***********************/
/********************************************************************************/
/********************************************************************************/
FieldElement lightAdditionGate::calculateEvaluation(const LazyVector<FieldElement>& inputVals)const{
	///Case 2 - Evaluating the gate:
	FieldElement res = ::Algebra::zero();
	for (unsigned i=0; i<inputVals.size(); i++){
		res += inputVals.getElementByIndex(i);
	}
	return res;
}

PolynomialDegree lightAdditionGate::calculateDegree(const vector<PolynomialDegree>& inputDegrees)const{
    PolynomialDegree deg(-1); // degree of the zero poly is -infty
    for(const auto& currD : inputDegrees){
        deg = max(deg,currD);
    }
    return deg;
}

/********************************************************************************/
/********************************************************************************/
/********************** lightMultiplicationGate Implementation *************************/
/********************************************************************************/
/********************************************************************************/
FieldElement lightMultiplicationGate::calculateEvaluation(const LazyVector<FieldElement>& inputVals)const{
	const FieldElement ZERO = Algebra::zero();
	FieldElement res = Algebra::one();
    for (size_t i=0; i<inputVals.size(); i++){
        
        //if a factor in the product is zero,
        //no need to check the valued of the rest
        if(res == ZERO){
            break;
        }
        
        res *= inputVals.getElementByIndex(i);
	}
	return res;
}

PolynomialDegree lightMultiplicationGate::calculateDegree(const vector<PolynomialDegree>& inputDegrees)const{
    PolynomialDegree deg(0); // degree of the CONST-one poly is -infty
    for(const auto& currD : inputDegrees){
        deg = degreeOfProduct(deg,currD);
    }
    return deg;
}

/********************************************************************************/
/********************************************************************************/
/*********************** lightExpoGate Implementation *************************/
/********************************************************************************/
/********************************************************************************/
FieldElement lightExpoGate::calculateEvaluation(const LazyVector<FieldElement>& inputVals)const{
    _COMMON_ASSERT(getInputsAmount() == 1, "Exponent gate expect a single input");
    FieldElement res = power(inputVals.getElementByIndex(0), m_expo);
	return res;
}

PolynomialDegree lightExpoGate::calculateDegree(const vector<PolynomialDegree>& inputDegrees)const{
    _COMMON_ASSERT(getInputsAmount() == 1, "Exponent gate expect a single input");
    return PolynomialDegree(PolynomialDegree::integral_t(inputDegrees[0])*m_expo);
}

/********************************************************************************/
/********************************************************************************/
/********************* lightConstGate Implementation *************************/
/********************************************************************************/
/********************************************************************************/
PolynomialDegree lightConstGate::calculateDegree(const vector<PolynomialDegree>& inputDegrees)const{
    if(m_value == zero()){
        return PolynomialDegree(-1); //-infinity
    }

    return PolynomialDegree(0);
}

/********************************************************************************/
/********************************************************************************/
/********************* lightOutGate Implementation *************************/
/********************************************************************************/
/********************************************************************************/

FieldElement lightOutGate::calculateEvaluation(const LazyVector<FieldElement>& inputVals)const{
    _COMMON_ASSERT(getInputsAmount() == 1, "Output gate expect a single input");
	return inputVals.getElementByIndex(0);
}

PolynomialDegree lightOutGate::calculateDegree(const vector<PolynomialDegree>& inputDegrees)const{
    _COMMON_ASSERT(getInputsAmount() == 1, "Output gate expect a single input");
    return inputDegrees[0];
}

} // namespace LightCircLib
} // namespace libstark
