#include "lightCircuit.hpp"

#include <cassert>
#include <queue>
#include <set>
#include <functional>

using std::set;
using std::queue;
using std::vector;
using std::function;
using std::unique_ptr;
using std::move;

using Algebra::FieldElement;
using Algebra::PolynomialDegree;

namespace libstark {
namespace LightCircLib {
	
/********************************************************************************/
/********************************************************************************/
/************************ lightCircuit Implementation *************************/
/********************************************************************************/
/********************************************************************************/
lightCircuit::lightCircuit(const lightCircuit& circ){
    constructFromCirc(circ);
}

void lightCircuit::constructFromCirc(const lightCircuit& circ){
    const int num_inputs = circ.getInputsNum();
	const int num_outputs = circ.getOutputsNum();
	
	m_inputs.resize(num_inputs);
	m_outputs.resize(num_outputs);
	
	vector<lightCircGate*> inputsForEmbed;
	for(auto& inpG : m_inputs){
		inputsForEmbed.push_back(&inpG);
	}

	vector<lightCircGate*> outputsForEmbed;
	for(auto& outG : m_outputs){
		outputsForEmbed.push_back(&outG);
	}

	embedCircuit(circ, inputsForEmbed, outputsForEmbed);
}

/***************************************/
/***** Circuit Inputs and Outputs ******/
/***************************************/

lightCircGate& lightCircuit::addGateToList(lightCircGate* newGate) {
    m_innerGates.push_back(unique_ptr<lightCircGate>(newGate));
    return *(m_innerGates[m_innerGates.size()-1]);
}

void lightCircuit::embedCircuit(const lightCircuit& embedded, const vector<lightCircGate*>& inputGates, const vector<lightCircGate*>& outputGates) {

	typedef lightCircGate src_gate;
	typedef lightCircGate dst_gate;
	typedef lightInputGate gateIn;
	typedef lightOutGate gateOut;
	typedef vector<src_gate*> src_gateVec;
	typedef vector<gateIn> gateInVec;
	typedef vector<gateOut> gateOutVec;
	
	/**
	 * We will use BFS from each output gate to go over all the gates that are connected to
	 * the output gates.
	 * We will keep a mapping of address of gates in 'embedded' circuit
	 * to the addresses of the cloned gates in this circuit.
	 * If an address of a gate is a key in the mapping, this
	 * means it was already copied, and the value in the mapping
	 * is the address of the copy.
	 */
	/**
	 * The following is the definition of the address mapping.
	 */
	std::map<src_gate const*,dst_gate*> addressMapping;

	/**
	 * First thing we do is keep the mapping of 'embedded' input
	 * gates addresses to its replacement in the circuit.
	 * It is important to have this done so the order will be
	 * kept as expected (otherwise if we map only when we reach
	 * a gate of input type the order is unpredictable)
	 */
	{
	const gateInVec& embInputGates = embedded.m_inputs;

	_COMMON_ASSERT(embInputGates.size() <= inputGates.size(), "It is imposible to embed more inputs in less");

    for(size_t i=0; i < embInputGates.size(); i++){
		addressMapping.emplace(&(embInputGates[i]),inputGates[i]);
	}
	}

	/**
	 * Now we execute the BFS from each output gate
	 */
	{
	const gateOutVec& embOutputGates = embedded.m_outputs;

	_COMMON_ASSERT(embOutputGates.size() <= outputGates.size(), "It is imposible to embed more outputs in less");

    for(size_t i=0; i< embOutputGates.size(); i++){

		/**
		 * There might be an InputAsOutput gate, check it by checking
		 * if current output gate address is already mapped, the only
		 * case it could be reachable before (except the case the same
		 * output gate appears twice in the vector) is if this gate is
		 * also an input gate.
		 * In such a case all need to be done is to connect them
		 * directly and continue to next output gate.
		 */
		if(addressMapping.count(&(embOutputGates[i])) > 0){
			(outputGates[i])->addInputGate(addressMapping[&(embOutputGates[i])]);
		}

		/** Add mapping between output gate and it replacement */
		addressMapping[&(embOutputGates[i])] = outputGates[i];

		/** Insert the current output gate to the queue, to start BFS from it */
		queue<src_gate const*> BFSQueue;
		BFSQueue.push(&(embOutputGates[i]));

		/** Start BFS from current output gate */
		while(!BFSQueue.empty()){
			src_gate const* curr = BFSQueue.front();
			BFSQueue.pop();

			const src_gateVec& neighbours = curr->getInputsVector();
			for(auto& neighbour : neighbours) {
				if(addressMapping.count(neighbour) == 0){
                    _COMMON_ASSERT((neighbour != NULL),"Null gate in circuit to embed");
					
                    /**
					 * This neighbour was not cloned yet,
					 * we will clone it now
					 */
					dst_gate& cloned =addGateToList(neighbour->shallowCopy());
					addressMapping[neighbour] = &cloned;

					/**
					 * Because it was not cloned before, this
					 * is for sure the first time it is visited.
					 * Push it into the queue to scan its neighbours
					 */
					BFSQueue.push(neighbour);
				}
				/**
				 * This neighbour is cloned for sure
				 * Just connect its clone to current gate
				 */
				addressMapping[curr]->addInputGate(addressMapping[neighbour]);
			}
		}

	}
	}
}

/**
 * @brief returns true if and only if there
 * is a path connecting the input gate in
 * the selected index to some output gate.
 */	
bool lightCircuit::isInputEffective(const size_t inputIndex)const{
    
	typedef lightCircGate src_gate;
	typedef vector<src_gate*> src_gateVec;

    //find the target input gate address
    const lightCircGate* target = &(m_inputs[inputIndex]);
	
	/**
	 * We will use BFS from each output gate to go over all the gates that are connected to
	 * the output gates.
	 * We will keep a set of address of gates we already visited.
	 */
	/**
	 * The following is the definition of the visited gates set.
	 */
	set<src_gate*> visitedGates;

	/**
	 * Now we execute the BFS from each output gate
	 */
	for (const auto& outGate : m_outputs){

		/** Insert the current output gate to the queue, to start BFS from it */
		queue<src_gate const*> BFSQueue;
		BFSQueue.push(&outGate);

		/** Start BFS from current output gate */
		while(!BFSQueue.empty()){
			const src_gate* curr = BFSQueue.front();
			BFSQueue.pop();

			const src_gateVec& neighbours = curr->getInputsVector();
			for(auto& neighbour : neighbours) {
            
                //if the current neighbor is the target gate
                //it is reachable, return "true"
                {
                if(neighbour == target) return true;
                }

            	if(visitedGates.count(neighbour) == 0){
                    
                    /**
					 * This neighbour was not visited yet,
                     * add it to visited set
					 */
					visitedGates.emplace(neighbour);

					/**
					 * Because it was not visited before
					 * push it into the queue to scan its neighbours
					 */
                    {
					const auto toPush = neighbour;
                    BFSQueue.push(toPush);
                    }
				}
			}
		}

	}

    //scan finished and the target gate was not reached
    return false;

}



/***************************************/
/********* Circuit Evaluations *********/
/***************************************/
FieldElement lightCircuit::evaluateCircuitSingleOutput_threadSafe(const vector<FieldElement>& inputValues)const {
	/** Add to evaluation mapping only a mapping from circuit input gates */
	lightCircGate::evaluationMap_t evaluations;
    for (size_t i=0; i < inputValues.size(); i++){
        evaluations[&(m_inputs[i])] = inputValues[i];
    }
	/** Evaluate the output gate */
	return m_outputs[0].evaluateGate(evaluations);
}

PolynomialDegree lightCircuit::getDegree_threadSafe(const vector<PolynomialDegree>& inputDegrees)const {
	/** Add to degrees mapping only a mapping from circuit input gates */
	lightCircGate::degreeCalcMap_t degrees;
    for (size_t i=0; i < inputDegrees.size(); i++){
        degrees.insert(std::pair<const lightCircGate*,PolynomialDegree>(&(m_inputs[i]),inputDegrees[i]));
    }
	/** Evaluate the output gate */
	return m_outputs[0].getDegree(degrees);
}

} // namespace LightCircLib
} // namespace libstark
