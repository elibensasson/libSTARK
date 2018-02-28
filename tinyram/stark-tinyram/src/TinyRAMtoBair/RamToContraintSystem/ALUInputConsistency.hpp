#ifndef ALU_INPUT_CONSISTENCY_HPP
#define ALU_INPUT_CONSISTENCY_HPP

#include <gadgetlib/gadget.hpp>
#include <algebraLib/CircuitPolynomial.hpp>
#include "generalPurpose.hpp"
#include "TinyRAM/TinyRAMInstance.hpp"

using namespace gadgetlib;


	
class ALUInputConsistency : public Gadget{
private:
	TraceVariables input_;
	ALUInput output_;
	TinyRAMProgram program_;
	
	ALUInputConsistency(ProtoboardPtr pb,
						const TraceVariables& input,
						const ALUInput& output);
	virtual void init();


public:
	static GadgetPtr create(ProtoboardPtr pb,
							const TraceVariables& input,
							const ALUInput& output);
	void setProgram(const TinyRAMProgram& program);
	void generateConstraints();
	void generateWitness(unsigned int i);

};



#endif //ALU_INPUT_CONSISTENCY_HPP
