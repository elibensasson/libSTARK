#include "lightCircPoly.hpp"

namespace libstark {

using Algebra::FieldElement;
using Algebra::PolynomialInterface;
using Algebra::PolynomialDegree;
using Algebra::UnivariatePolynomialInterface;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::zero;
using Algebra::one;

using std::max;
using std::vector;
using std::unique_ptr;
using std::move;

using namespace LightCircLib;


lightCircPoly::lightCircPoly(const UnivariatePolynomialGeneral& src): model_(new lightCircuit(1,1)){

    //get the input gate
	lightInputGate& input = model_->getInputGates()[0];

    //get the output gate
	lightOutGate& output = model_->getOutputGates()[0];

	/**
	 * The circuit is constructed in the following way:
	 * each monom of the form \f$a_i \cdot x^i\f$ is
	 * represented as passing the input into an exponent gate
	 * and multiplying the result by the constant \f$a_i\f$.
	 * All the monoms results are passed into a single addition gate,
	 * this way we get the wanted result:
	 * \f$ (a_0 \cdot x^0) + (a_1 \cdot x^1) + \dots + (a_n \cdot x^n)\f$
	 *
	 * The following is the construction of the single addition gate
	 */
	lightAdditionGate* sigma = new lightAdditionGate();
    model_->addGateToList(sigma);
    output.addInputGate(sigma);

	
	/**
	 * Now we construct all the monomials
	 *
	 * Optimizations used:
	 * - if the coefficient in the monomial (\f$a_i\f$) is zero,
	 *   there is no need to construct it
	 */
	for (PolynomialDegree::integral_t i=0; i <= PolynomialDegree::integral_t(src.getDegree()); i++) {

		/** fetch coefficient of \f$x^i\f$ */
		elementT coefficient = src.getCoefficient(i);

		/** if the coefficient is zero, continue to next coefficient */
		if (coefficient == zero()){
			continue;
		}

		/** Construct the CONSTANT gate for the coefficient */
		lightConstGate* constant = new lightConstGate(coefficient);
		model_->addGateToList(constant);

		/** Construct the exponent gate */
		lightExpoGate* expo = new lightExpoGate(i);
		model_->addGateToList(expo);
		expo->addInputGate(&input);

		/**
		 * Construct the multiplication gate.
		 * It multiplies the exponent result with the coefficient
		 * and passes the product to the addition gate
		 */
		lightMultiplicationGate* product = new lightMultiplicationGate();
		model_->addGateToList(product);
		product->addInputGate(expo);
		product->addInputGate(constant);
		sigma->addInputGate(product);
	}
}
	
lightCircPoly::lightCircPoly(const LightCircLib::lightCircuit& circ):model_(new lightCircuit(circ)){}
lightCircPoly::lightCircPoly(const lightCircPoly& poly):model_(new lightCircuit(poly.getCircuit())){}

unique_ptr<PolynomialInterface> lightCircPoly::clone()const{
    return unique_ptr<PolynomialInterface>(new lightCircPoly(*model_));
}
	

/**
 * @brief   A constructor to that expand the amount of inputs of a given
 * lightCircPoly
 *
 * It requires a mapping from the original inputs indexes to new inputs indexes
 */
lightCircPoly::lightCircPoly(const lightCircPoly& src, const size_t totalInputs ,const vector<size_t>& originalInputsLocation):
    model_(new lightCircuit(totalInputs,1)){

    //get the output gate
	lightOutGate& output = model_->getOutputGates()[0];

    //initialize embedding inputs
    vector<lightCircGate*> embInputs(originalInputsLocation.size());
    for(size_t i=0; i<originalInputsLocation.size(); i++){
        embInputs[i] = &(model_->getInputGates()[originalInputsLocation[i]]);
    }

    //initialize embedding outputs
    vector<lightCircGate*> embOutputs;
    embOutputs.push_back(&output);

    //embed the source circuit
    model_->embedCircuit(src.getCircuit(), embInputs, embOutputs);
}

lightCircPoly::elementT lightCircPoly::eval(const vector<lightCircPoly::elementT>& x)const {
    return model_->evaluateCircuitSingleOutput_threadSafe(x);
}

size_t lightCircPoly::numVars()const{
    return model_->getInputsNum();
}

void lightCircPoly::addSameInputs(const lightCircPoly& other){
	return mergeSameInputs(other,new lightAdditionGate());
}

void lightCircPoly::multiplyDistinct(const lightCircPoly& other){
	return mergeDistinctInputs(other,new lightMultiplicationGate());
}

void lightCircPoly::addDistinct(const lightCircPoly& other){
	return mergeDistinctInputs(other,new lightAdditionGate());
}

void lightCircPoly::mergeDistinctInputs(const lightCircPoly& other, lightCircGate* mergingGate){
	
    // construct new circuit
    unique_ptr<lightCircuit> newCirc(new lightCircuit(model_->getInputsNum() + other.model_->getInputsNum(),1));
    
    // add the new gate
    auto& finalGate = newCirc->addGateToList(mergingGate);
    newCirc->getOutputGates()[0].addInputGate(&finalGate);

    // construct the embedding output
    vector<lightCircGate*> embOutput = {&finalGate};

    // construct the inputs for current poly
    vector<lightCircGate*> embInputs1;
    for(size_t i=0; i< model_->getInputsNum(); i++){
        embInputs1.push_back(&(newCirc->getInputGates()[i]));
    }

    // construct the inputs for other poly
    vector<lightCircGate*> embInputs2;
    for(size_t i=0; i< other.model_->getInputsNum(); i++){
        embInputs2.push_back(&(newCirc->getInputGates()[i + model_->getInputsNum()]));
    }

    // embed polys in new poly
    newCirc->embedCircuit(*model_,embInputs1, embOutput);
    newCirc->embedCircuit(*(other.model_),embInputs2, embOutput);
    
    // change the model
    model_ = move(newCirc);
}

void lightCircPoly::mergeSameInputs(const lightCircPoly& other, lightCircGate* mergingGate){
	
    // construct new circuit
    unique_ptr<lightCircuit> newCirc(new lightCircuit(max(model_->getInputsNum(),other.model_->getInputsNum()),1));
    
    // add the new gate
    auto& finalGate = newCirc->addGateToList(mergingGate);
    newCirc->getOutputGates()[0].addInputGate(&finalGate);

    // construct the embedding output
    vector<lightCircGate*> embOutput = {&finalGate};

    // construct the inputs for current poly
    vector<lightCircGate*> embInputs1;
    for(size_t i=0; i< model_->getInputsNum(); i++){
        embInputs1.push_back(&(newCirc->getInputGates()[i]));
    }

    // construct the inputs for other poly
    vector<lightCircGate*> embInputs2;
    for(size_t i=0; i< other.model_->getInputsNum(); i++){
        embInputs2.push_back(&(newCirc->getInputGates()[i]));
    }

    // embed polys in new poly
    newCirc->embedCircuit(*model_,embInputs1, embOutput);
    newCirc->embedCircuit(*(other.model_),embInputs2, embOutput);
    
    // change the model
    model_ = move(newCirc);
}
	
PolynomialDegree lightCircPoly::getDegreeBound(const vector< PolynomialDegree>& inputDegreesBound)const {
    return model_->getDegree_threadSafe(inputDegreesBound);
}

PolynomialDegree lightCircPoly::getDegree()const {
	vector< PolynomialDegree> degBounds;
	for (size_t i=0; i< model_->getInputsNum(); i++){
		degBounds.push_back(PolynomialDegree(1));
	}
	return getDegreeBound(degBounds);
}

/**
 * return whether a specified input is effective:
 * it returns false only if the result
 * can not be affected by the specified input
 */
bool lightCircPoly::isEffectiveInput(const size_t inputIndex)const{
    return model_->isInputEffective(inputIndex);
}

} // namespace libstark 
