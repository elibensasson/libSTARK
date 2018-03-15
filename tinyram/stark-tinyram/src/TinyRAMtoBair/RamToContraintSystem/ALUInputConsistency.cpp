#include "ALUInputConsistency.hpp"
#include <algebraLib/variable_operators.hpp>
#include <memory>



ALUInputConsistency::ALUInputConsistency(ProtoboardPtr pb,
										const TraceVariables& input,
										const ALUInput& output) :
		Gadget(pb), input_(input), output_(output),
		program_("program", 
		std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->numRegisters(),
		std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->registerLength()){}


void ALUInputConsistency::init(){};

GadgetPtr ALUInputConsistency::create(ProtoboardPtr pb,
									const TraceVariables& input,
									const ALUInput& output){
	GadgetPtr pGadget(new ALUInputConsistency(pb,input,output));
	pGadget->init();
	return pGadget;
}

void ALUInputConsistency::setProgram(const TinyRAMProgram& program){
	program_ = program;
}


void ALUInputConsistency::generateConstraints(){
	GADGETLIB_ASSERT(program_.size() > 0, "ALUInputConsistency: The program should be initialized");
	::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
	vector<CircuitPolynomial> arg1Polynomials, arg2Polynomials, destPolynomials;
	CircuitPolynomial resArg1, resArg2, resDest;
	vector<long> selectorToConstraint;
	vector<bool> selectorRelevant;

	for (unsigned int i = 0; i < program_.size(); ++i){
		unsigned int arg1 = program_.code()[i].arg1Idx_;
		unsigned int dest = program_.code()[i].destIdx_;
		bool arg2IsImmediate = program_.code()[i].arg2isImmediate_; //If 1 then arg2 is immediate
		CircuitPolynomial arg2Poly;
		if (!arg2IsImmediate){
			unsigned int arg2 = program_.code()[i].arg2IdxOrImmediate_;
			arg2Poly = input_.registers_[arg2] + output_.arg2_val_;
		}
		else{ //Arg2 is immediate
			Algebra::FElem valArg2 = mapIntegerToFieldElement(0, params->registerLength(), program_.code()[i].arg2IdxOrImmediate_);
			arg2Poly = valArg2 + output_.arg2_val_;
		}
		CircuitPolynomial arg1Poly(input_.registers_[arg1] + output_.arg1_val_);	
		CircuitPolynomial destPoly(input_.registers_[dest] + output_.dest_val_);
		arg1Polynomials.emplace_back(arg1Poly);
		arg2Polynomials.emplace_back(arg2Poly);
		destPolynomials.emplace_back(destPoly);
		selectorToConstraint.emplace_back(i);
		selectorRelevant.push_back(true);
	}
	vector<Variable> opcodeVars = getPCVars(input_.pc_);
	CircuitPolynomial SArg1(SelectorSum(arg1Polynomials, opcodeVars, selectorToConstraint, selectorRelevant));
	CircuitPolynomial SArg2(SelectorSum(arg2Polynomials, opcodeVars, selectorToConstraint, selectorRelevant));
	CircuitPolynomial SDest(SelectorSum(destPolynomials, opcodeVars, selectorToConstraint, selectorRelevant));
	pb_->addGeneralConstraint(SArg1, "SelectorSum_ARG1", Opcode::NONE);
	pb_->addGeneralConstraint(SArg2, "SelectorSum_ARG2", Opcode::NONE);
	pb_->addGeneralConstraint(SDest, "SelectorSum_Dest", Opcode::NONE);
};

void ALUInputConsistency::generateWitness(unsigned int i){
	GADGETLIB_ASSERT(i < program_.size(), "ALUInputConsistency: in order to generate witness i should be less the the program size");
	::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
	unsigned int arg1 = program_.code()[i].arg1Idx_;
	unsigned int dest = program_.code()[i].destIdx_;
	bool arg2IsImmediate = program_.code()[i].arg2isImmediate_; //If 1 then arg2 is immediate
	if (!arg2IsImmediate){
		unsigned int arg2 = program_.code()[i].arg2IdxOrImmediate_;
		pb_->val(output_.arg2_val_) = pb_->val(input_.registers_[arg2]);
	}
	else{
		pb_->val(output_.arg2_val_) = mapIntegerToFieldElement(0, params->registerLength(), program_.code()[i].arg2IdxOrImmediate_);
	}
	pb_->val(output_.arg1_val_) = pb_->val(input_.registers_[arg1]);
	pb_->val(output_.dest_val_) = pb_->val(input_.registers_[dest]);
};


