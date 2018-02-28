#include <memory>
#include "transitionFunction.hpp"
#include "ALU.hpp"
#include "ALUInputConsistency.hpp"
#include "traceConsistency.hpp"



TransitionFunction::TransitionFunction(ProtoboardPtr pb,
									const FollowingTraceVariables& followingTraceVariable,
									const MemoryFollowingTraceVariables& memoryFollowingTraceVariables,
									const TinyRAMProgram& program) :
		Gadget(pb),
        program_(program),
        followingTraceVariable_(followingTraceVariable),
		memoryFollowingTraceVariables_(memoryFollowingTraceVariables),
        inputTraceLine_(followingTraceVariable.first_), 
        outputTraceLine_(followingTraceVariable.second_),
		aluInput(followingTraceVariable.first_.flag_, followingTraceVariable.first_.pc_),
		aluOutput(followingTraceVariable.second_.flag_, memoryFollowingTraceVariables_.first_.isMemOp_,memoryFollowingTraceVariables_.first_.address_,
		memoryFollowingTraceVariables_.first_.isLoad_, memoryFollowingTraceVariables_.first_.value_){}

GadgetPtr TransitionFunction::create(ProtoboardPtr pb,
									const FollowingTraceVariables& followingTraceVariable,
									const MemoryFollowingTraceVariables& memoryFollowingTraceVariables,
									const TinyRAMProgram& program){
	GadgetPtr pGadget(new TransitionFunction(pb, followingTraceVariable, memoryFollowingTraceVariables,program));
	pGadget->init();
	return pGadget;
}

void TransitionFunction::init(){
	aluInputConsistnecy_g_ = ALUInputConsistency::create(pb_, inputTraceLine_, aluInput);
	alu_g_ = ALU_Gadget::create(pb_, aluInput, aluOutput);
	traceConsistency_g_ = TraceConsistency::create(pb_, aluOutput, followingTraceVariable_);
	(::std::dynamic_pointer_cast<ALUInputConsistency>(aluInputConsistnecy_g_))->setProgram(program_);
	(::std::dynamic_pointer_cast<ALU_Gadget>(alu_g_))->setProgram(program_);
	(::std::dynamic_pointer_cast<TraceConsistency>(traceConsistency_g_))->setProgram(program_);
}

void TransitionFunction::generateConstraints(){
	aluInputConsistnecy_g_->generateConstraints();
	alu_g_->generateConstraints();
	traceConsistency_g_->generateConstraints();
};

int TransitionFunction::calcPC(){
	const FElem g = Algebra::xFE();//  Algebra::FElem(getGF2E_X());
	FElem x_i = Algebra::one();
	FElem result = Algebra::zero();
	for (unsigned int i = 0; i < followingTraceVariable_.first_.pc_.size(); i++){
		result += pb_->val(followingTraceVariable_.first_.pc_[i]) * x_i;
		x_i *= g;
	}
	return mapFieldElementToInteger(0, followingTraceVariable_.first_.pc_.size(), result);

}

void TransitionFunction::generateWitness(unsigned int i){
	int codeLineNumber = calcPC();
	GADGETLIB_ASSERT(codeLineNumber < (long)program_.size(), "TransitionFunction: The code line number should be less than program.size()");
	(::std::dynamic_pointer_cast<ALUInputConsistency>(aluInputConsistnecy_g_))->generateWitness(codeLineNumber);
	(::std::dynamic_pointer_cast<ALU_Gadget>(alu_g_))->generateWitness(codeLineNumber);
	(::std::dynamic_pointer_cast<TraceConsistency>(traceConsistency_g_))->generateWitness(codeLineNumber);


	// Update Memory Info
	::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
	MemoryInfo memInfo;
	memInfo.updateSerialNumber(i);
	Opcode opcode = program_.code()[codeLineNumber].opcode_;
	if (Opcode::STOREB == opcode || Opcode::STOREW == opcode || 
		Opcode::LOADB == opcode || Opcode::LOADW == opcode)  {
		// isMemOp = true
		memInfo.updateIsMemOp(true);
		// Update timeStamp;
		FElem timestamp = pb_->val(followingTraceVariable_.first_.timeStamp_);
		int degree = pb_->getDegreeOfFElem(timestamp);
		memInfo.updateTimestamp(timestamp, degree);
		// Update Address
		FElem address;
		if (program_.code()[codeLineNumber].arg2isImmediate_){
			address = mapIntegerToFieldElement(0, params->registerLength(), program_.code()[codeLineNumber].arg2IdxOrImmediate_);
		}
		else{ // Not immediate - The address is what is written in arg2
			int regNum = program_.code()[codeLineNumber].arg2IdxOrImmediate_;
			address = pb_->val(followingTraceVariable_.first_.registers_[regNum]);
		}
		memInfo.updateAddress(address);
		// Update value;
		FElem value = pb_->loadValue(address);
		// Update Is Load Opcode
		bool isLoad = (Opcode::LOADB == opcode || Opcode::LOADW == opcode) ? true : false;
		memInfo.updateIsLoadOp(isLoad);
	} 
	pb_->addMemoryInfo(memInfo);

};
