#ifndef LIGHT_CIRCUIT_HPP_
#define LIGHT_CIRCUIT_HPP_

#include "lightCircGate.hpp"

#include <memory>

namespace libstark {
namespace LightCircLib {

class lightCircuit {
private:

	std::vector<lightInputGate> m_inputs;				///The input gates (assigned with constant values)
	std::vector<lightOutGate> m_outputs;			///The output gates (assigned with constant values)
	std::vector<std::unique_ptr<lightCircGate>> m_innerGates;		///A list of all the inner gates (not input \ output) in the circuit.
	
    void constructFromCirc(const lightCircuit& circ);

public:

	/*************************/
	/***** Constructors ******/
	/*************************/

	/** Class Default Constructor */
	lightCircuit(size_t numInputs, size_t numOutputs):m_inputs(numInputs),m_outputs(numOutputs){};

    /**
     * Same as above, but for small type of nodes
     * couldn't merge them together because of some bug.
     * (Maybe I do not understand something, or maybe compiler bug)
     */
    lightCircuit(const lightCircuit& circ);
	
    /***************************************/
	/***** Circuit Inputs and Outputs ******/
	/***************************************/

	lightCircGate& addGateToList(lightCircGate* newGate);

	/** Gets the input gates: */
	const std::vector<lightInputGate>& getInputGates() const{return m_inputs;}
	std::vector<lightInputGate>& getInputGates(){return m_inputs;}

	/** Gets amount of input gates: */
	size_t getInputsNum() const{return m_inputs.size();}
	
    /** Gets the output gates: */
	const std::vector<lightOutGate>& getOutputGates() const{return m_outputs;}
	std::vector<lightOutGate>& getOutputGates() {return m_outputs;}

	/** Returns the number of output gates: */
	int getOutputsNum() const{return m_outputs.size();}

    /**
     * @brief   Embeds a copy of a circuit into this circuit
     * @param   embedded the circuit to embed
     * @param   inputGates a vector of pointers to gates. Those gates
     * are used by the function to replace the input gates of the
     * embedded circuit, ordered by the same order of the inputs.
     * It is assumed the input gates are part of the gates of
     * this circuit.
     * @param   outputGates a vector of pointers to gates. Those gates
     * are used by the function to replace the output gates of the
     * embedded circuit, ordered by the same order of the outputs.
     * It is assumed the output gates are part of the gates of
     * this circuit.
     */
    void embedCircuit(const lightCircuit& embedded, const std::vector<lightCircGate*>& inputGates, const std::vector<lightCircGate*>& outputGates);

    /**
     * @brief returns true if and only if there
     * is a path connecting the input gate in
     * the selected index to some output gate.
     */	
    bool isInputEffective(const size_t inputIndex)const;
    
    /***************************************/
	/********* Circuit Evaluations *********/
	/***************************************/
	
	/** 
	 * Evaluates the SINGLE output gate, given assignments for all the input gates.(thread safe) 
	 * @param inputValues are the values assigned to all input gates.
	 */
	Algebra::FieldElement evaluateCircuitSingleOutput_threadSafe(const std::vector<Algebra::FieldElement>& inputValues)const;
	
    Algebra::PolynomialDegree getDegree_threadSafe(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const;

    /** Class Destructor */
	virtual ~lightCircuit(){};
};



}	// LightCircLib namespace
}	// libstark namespace


#endif	//LIGHT_CIRCUIT_HPP_
