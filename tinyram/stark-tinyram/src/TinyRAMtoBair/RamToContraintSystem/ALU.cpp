#include <memory>
#include <algebraLib/variable_operators.hpp>
#include <algebraLib/FieldElement.hpp>
#include "ALU.hpp"
#include <algebraLib/BitExtract.hpp>
#include <bitset>

//#include <algebraLib/SelectorSum.hpp>
#define EXTDIM Algebra::ExtensionDegree
using namespace std;

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         General Variables                  ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
Algebra::Variable pInverse_("pInverse");
UnpackedWord unpackedArg1_(REGISTER_LENGTH, "unpackedArg1_");
UnpackedWord unpackedArg2_(REGISTER_LENGTH, "unpackedArg2_");
UnpackedWord opcodeResult_(REGISTER_LENGTH, "opcodeResult_");
UnpackedWord opcodeCarry_(REGISTER_LENGTH, "opcodeCarry_");
UnpackedWord & opcodeAux1_ = opcodeResult_;
UnpackedWord & opcodeAux2_ = opcodeCarry_;
UnpackedWord opcodeAux3_(REGISTER_LENGTH, "opcodeAux3_");
UnpackedWord opcodeAux4_(REGISTER_LENGTH, "opcodeAux4_");
UnpackedWord opcodeAux5_(REGISTER_LENGTH, "opcodeAux5_");
UnpackedWord opcodeAux6_(REGISTER_LENGTH, "opcodeAux6_");
UnpackedWord opcodeAux7_(REGISTER_LENGTH, "opcodeAux7_");

//lame, move all to TinyRAMProtoboardParams?
unsigned int prngseed;
bool standAlone_ = false;
FElem program_output = Algebra::one(); //use any incorrect != -1 value to test soundness
int max_timestep = 1;
unsigned int ROMSIZE = 0;

void resetALU_GadgetGlobalState(){
    max_timestep = 1;
}

void initMemResult(ProtoboardPtr pb, const ALUOutput& res){
	pb->val(res.isMemOp_) = Algebra::zero();
	pb->val(res.isLoadOp_) = Algebra::zero();
	pb->val(res.value_) = Algebra::zero();
	pb->val(res.address_) = Algebra::zero();

}

void initGeneralOpcodes(ProtoboardPtr pb){
	pb->val(pInverse_) = Algebra::zero();
	for (int i = 0; i < REGISTER_LENGTH; i++){
		// Other opcodes use them and we multiply the constraint by selector
		// In order to be evaluated correctly the must be initialized as 0
		pb->val(unpackedArg1_[i]) = Algebra::zero();
		pb->val(unpackedArg2_[i]) = Algebra::zero();
		pb->val(opcodeResult_[i]) = Algebra::zero();
		pb->val(opcodeCarry_[i]) = Algebra::zero();
		pb->val(opcodeAux3_[i]) = Algebra::zero();
		pb->val(opcodeAux4_[i]) = Algebra::zero();
		pb->val(opcodeAux5_[i]) = Algebra::zero();
		pb->val(opcodeAux6_[i]) = Algebra::zero();
		pb->val(opcodeAux7_[i]) = Algebra::zero();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      ALU_Component_Gadget                  ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
const ALU_Component_Gadget::TRParamsPtr ALU_Component_Gadget::tinyRAMparams() const {
	return std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                            ALU_Gadget                      ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

GadgetPtr ALU_Gadget::create(ProtoboardPtr pb,
							const ALUInput& inputVariables,
							const ALUOutput& resultVariables) {
	GadgetPtr pGadget(new ALU_Gadget(pb, inputVariables, resultVariables));
	pGadget->init();
	return pGadget;
}

ALU_Gadget::ALU_Gadget(ProtoboardPtr pb,
					const ALUInput& inputVariables,
					const ALUOutput& resultVariables)
	: 
        Gadget(pb),
	program_("program",
	std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->numRegisters(),
	std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->registerLength()),
        inputVariables_(inputVariables),
        resultVariables_(resultVariables){}

void ALU_Gadget::init() {
	createInternalComponents();
	//this used to appear in internal inits. Moved it here so won't be repetition of booleanity checks multiplied by selectors
	 	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputVariables_.arg1_val_, PackingMode::UNPACK, Opcode::NONE);
		unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputVariables_.arg2_val_, PackingMode::UNPACK, Opcode::NONE);

}

void ALU_Gadget::createInternalComponents() {
	components_[Opcode::XOR] = ALU_XOR_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::AND] = ALU_AND_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::OR] = ALU_OR_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::NOT] = ALU_NOT_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::ADD] = ALU_ADD_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::SUB] = ALU_SUB_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::MULL] = ALU_MULL_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::UMULH] = ALU_UMULH_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::SMULH] = ALU_SMULH_Gadget::create(pb_, inputVariables_,
		resultVariables_);
	components_[Opcode::UDIV] = ALU_UDIV_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::UMOD] = ALU_UDIV_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::CMPE] = ALU_CMPE_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::CMPA] = ALU_CMPA_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::CMPAE] = ALU_CMPAE_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::CMPG] = ALU_CMPG_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::CMPGE] = ALU_CMPGE_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::SHL] = ALU_SHL_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::SHR] = ALU_SHR_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::LOADW] = ALU_LOADW_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::STOREW] = ALU_STOREW_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::JMP] = ALU_JMP_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::ANSWER] = ALU_ANSWER_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::CJMP] = ALU_CJMP_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::CNJMP] = ALU_CNJMP_Gadget::create(pb_, inputVariables_,
															resultVariables_);
	components_[Opcode::RESERVED_OPCODE_24] = 
		ALU_RESERVED_OPCODE_24_Gadget::create(pb_, inputVariables_,resultVariables_);
	components_[Opcode::MOV] = ALU_MOV_Gadget::create(pb_, inputVariables_,
															resultVariables_);
}

void ALU_Gadget::setProgram(const TinyRAMProgram& program){
	program_ = program;
}




set<Opcode> getUsedOpcodes(const TinyRAMProgram& program){
	set<Opcode> retVal;
	for (auto inst : program.code()) {
		retVal.insert(inst.opcode_);
	}
	return retVal;
}

map<Opcode, Polynomial> getRelevantConstraints(const TinyRAMProgram& program, const size_t& constraintIndex, const ProtoboardPtr& pb){
	map<Opcode, Polynomial> retVal;
	set<Opcode> opcodes = getUsedOpcodes(program);
	for (auto opcode : opcodes) {
		if (pb->constraintSystem(opcode).getNumberOfConstraints() > constraintIndex) {
			retVal[opcode] = pb->constraintSystem(opcode).getConstraints()[constraintIndex].constraint();
		}
	}
	return retVal;
}

size_t getMaxConstraintNum(const TinyRAMProgram& program,const ProtoboardPtr& pb){
	set<Opcode> opcodes = getUsedOpcodes(program);
	size_t retVal = 0;
	for (const auto& opcode : opcodes) {
		retVal = max(retVal, pb->constraintSystem(opcode).getNumberOfConstraints());
	}
	return retVal;
}

void ALU_Gadget::generateConstraints() {
	unpackArg1_g_->generateConstraints();
	unpackArg2_g_->generateConstraints();
	//standAlone_ = false; //uncomment if needed
	for (auto& componentPair : components_) {
		componentPair.second->generateConstraints();
	}
	GADGETLIB_ASSERT(program_.size() > 0, "The program should be initalized");
	
	unsigned int programLength = (unsigned int)(program_.size());
	size_t maxConstraintNum = getMaxConstraintNum(program_, pb_);
	for (unsigned int i = 0; i < maxConstraintNum; i++) {
		map<Opcode, Polynomial> opToConstraints = getRelevantConstraints(program_, i, pb_);
		vector<Polynomial> constraints;
		map<Opcode, long> M;
		for (const auto& a : opToConstraints){
			constraints.push_back(a.second);
			M[a.first] = constraints.size() - 1;
		}
		vector<Variable> opcodeVars = getPCVars(inputVariables_.pc_);

		vector<long> selectorToConstraint(programLength);
		vector<bool> selectorRelevant(programLength);
		for (unsigned int j = 0; j < programLength; j++) {
			Opcode currentOpcode = program_.code()[j].opcode_;
			if (opToConstraints.find(currentOpcode) != opToConstraints.end()){
				selectorRelevant[j] = true;
				selectorToConstraint[j] = M[currentOpcode];
			}
			else {
				selectorRelevant[j] = false;
			}
		}
		CircuitPolynomial S(SelectorSum(constraints, opcodeVars, selectorToConstraint, selectorRelevant));
		pb_->addGeneralConstraint(S, "SelectorSum_" + i, Opcode::NONE);
	}
}

void ALU_Gadget::generateWitness(unsigned int i) {
	GADGETLIB_ASSERT(i < program_.size(), "The value should be less than the program length");
	Opcode opcode = program_.code()[i].opcode_;
	switch (opcode)
	{
	case gadgetlib::Opcode::NONE:
		GADGETLIB_FATAL("The opcode can't be NONE in the program");
		break;
	case gadgetlib::Opcode::AND:
		components_[Opcode::AND]->generateWitness();
		break;
	case gadgetlib::Opcode::OR:
		components_[Opcode::OR]->generateWitness();
		break;
	case gadgetlib::Opcode::XOR:
		components_[Opcode::XOR]->generateWitness();
		break;
	case gadgetlib::Opcode::NOT:
		components_[Opcode::NOT]->generateWitness();
		break;
	case gadgetlib::Opcode::ADD:
		components_[Opcode::ADD]->generateWitness();
		break;
	case gadgetlib::Opcode::SUB:
		components_[Opcode::SUB]->generateWitness();
		break;
	case gadgetlib::Opcode::MULL:
		components_[Opcode::MULL]->generateWitness();
		break;
	case gadgetlib::Opcode::UMULH:
		components_[Opcode::UMULH]->generateWitness();
		break;
	case gadgetlib::Opcode::SMULH:
		components_[Opcode::SMULH]->generateWitness();
		break;
	case gadgetlib::Opcode::UDIV:
		components_[Opcode::UDIV]->generateWitness();
		break;
	case gadgetlib::Opcode::UMOD:
		components_[Opcode::UMOD]->generateWitness();
		break;
	case gadgetlib::Opcode::SHL:
		components_[Opcode::SHL]->generateWitness();
		break;
	case gadgetlib::Opcode::SHR:
		components_[Opcode::SHR]->generateWitness();
		break;
	case gadgetlib::Opcode::SHAR:
		break;
	case gadgetlib::Opcode::CMPE:
		components_[Opcode::CMPE]->generateWitness();
		break;
	case gadgetlib::Opcode::CMPA:
		components_[Opcode::CMPA]->generateWitness();
		break;
	case gadgetlib::Opcode::CMPAE:
		components_[Opcode::CMPAE]->generateWitness();
		break;
	case gadgetlib::Opcode::CMPG:
		components_[Opcode::CMPG]->generateWitness();
		break;
	case gadgetlib::Opcode::CMPGE:
		components_[Opcode::CMPGE]->generateWitness();
		break;
	case gadgetlib::Opcode::MOV:
		components_[Opcode::MOV]->generateWitness();
		break;
	case gadgetlib::Opcode::CMOV:
		break;
	case gadgetlib::Opcode::JMP:
		components_[Opcode::JMP]->generateWitness();
		break;
	case gadgetlib::Opcode::CJMP:
		components_[Opcode::CJMP]->generateWitness();
		break;
	case gadgetlib::Opcode::CNJMP:
		components_[Opcode::CNJMP]->generateWitness();
		break;
	case gadgetlib::Opcode::RESERVED_OPCODE_24:
		components_[Opcode::RESERVED_OPCODE_24]->generateWitness();
		break;
	case gadgetlib::Opcode::RESERVED_OPCODE_25:
		break;
	case gadgetlib::Opcode::STOREB:
		break;
	case gadgetlib::Opcode::LOADB:
		break;
	case gadgetlib::Opcode::STOREW:
		components_[Opcode::STOREW]->generateWitness();
		break;
	case gadgetlib::Opcode::LOADW:
		components_[Opcode::LOADW]->generateWitness();
		break;
	case gadgetlib::Opcode::READ:
		break;
	case gadgetlib::Opcode::ANSWER:
		components_[Opcode::ANSWER]->generateWitness();
		return; //break;
	case gadgetlib::Opcode::NUM_OPCODES:
		break;
	default:
		break;
	}
	++max_timestep;
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_XOR_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
ALU_XOR_Gadget::ALU_XOR_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
								:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

void ALU_XOR_Gadget::init(){}

GadgetPtr ALU_XOR_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_XOR_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_XOR_Gadget::generateConstraints(){
	CircuitPolynomial poly(inputs_.arg1_val_ + inputs_.arg2_val_ + results_.result_);
	pb_->addGeneralConstraint(poly,"arg1_val + arg2_val + dest_val = 0",Opcode::XOR);
	// Flag constraint
	CircuitPolynomial flagConstarint1(results_.result_ *  results_.flag_);
	CircuitPolynomial flagConstarint2(results_.result_ *  pInverse_);
	CircuitPolynomial flagConstraint3(Algebra::one() + results_.flag_);
 	pb_->addGeneralConstraint(flagConstarint1, "flag * result= 0", Opcode::XOR);
	pb_->addGeneralConstraint(flagConstarint2 + flagConstraint3, "result * invResult = 1 - flag", Opcode::XOR);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::XOR);
}

void ALU_XOR_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	Algebra::FElem res = pb_->val(inputs_.arg1_val_) + pb_->val(inputs_.arg2_val_);
	pb_->val(results_.result_) = res;
	pb_->val(results_.flag_) = (res == Algebra::zero()) ? Algebra::one() : Algebra::zero();
	pb_->val(pInverse_) = (res != Algebra::zero()) ? res.inverse() : Algebra::one();
	GADGETLIB_ASSERT(unpackedArg1_.size() == unpackedArg2_.size(), "XOR_GADGET: Unpacked1.size() == Unpacked2.size()");
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_AND_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
ALU_AND_Gadget::ALU_AND_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

void ALU_AND_Gadget::init(){
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::AND);
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::AND);	
}

GadgetPtr ALU_AND_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_AND_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_AND_Gadget::generateConstraints(){
	if (standAlone_){
		unpackArg1_g_->generateConstraints();
		unpackArg2_g_->generateConstraints();
	}
	size_t registerLength = tinyRAMparams()->registerLength();
	CircuitPolynomial polynomial;
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	for (unsigned int i = 0; i < registerLength; ++i){
		polynomial = polynomial + (unpackedArg1_[i] * unpackedArg2_[i] * x_i);
		x_i *= x;
	}
	pb_->addGeneralConstraint(polynomial + results_.result_, "sum_{i=0}^{registerLength-1} uinput1[i] * uinput2[i] * X^i + result = 0", Opcode::AND);
	// Flag constraint
	CircuitPolynomial flagConstraint1({ results_.result_, results_.flag_ });//{a,b} means the product of A and B - and in this notation uses the faster (prod of) LinearCombination eval function
	CircuitPolynomial flagConstraint2({ results_.result_, pInverse_ });
	CircuitPolynomial flagConstraint3(Algebra::one() + results_.flag_);
	pb_->addGeneralConstraint(flagConstraint1, "flag * result= 0", Opcode::AND);
	pb_->addGeneralConstraint(flagConstraint2 + flagConstraint3, "result * invResult = 1 - flag", Opcode::AND);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::AND);
}

void ALU_AND_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpackArg1_g_->generateWitness();
	unpackArg2_g_->generateWitness();
	size_t registerLength = tinyRAMparams()->registerLength();
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	Algebra::FElem res = Algebra::zero();
	for (unsigned int i = 0; i < registerLength; i++){
		res += pb_->val(unpackedArg1_[i]) * pb_->val(unpackedArg2_[i]) * x_i;
		x_i *= x;
	}
	pb_->val(results_.result_) = res;
	pb_->val(results_.flag_) = (res == Algebra::zero()) ? Algebra::one() : Algebra::zero();
	pb_->val(pInverse_) = (res != Algebra::zero()) ? res.inverse() : Algebra::one();
	for (int i = 0; i < REGISTER_LENGTH; i++){
		// Other opcodes use them and we multiply the constraint by selector
		// In order to be evaluated correctly the must be initialized as 0
		pb_->val(opcodeResult_[i]) = Algebra::zero();
		pb_->val(opcodeCarry_[i]) = Algebra::zero();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_OR_Gadget                      ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
ALU_OR_Gadget::ALU_OR_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}


void ALU_OR_Gadget::init(){
	//size_t registerLength = tinyRAMparams()->registerLength();
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::OR);
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::OR);
	
}

GadgetPtr ALU_OR_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_OR_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_OR_Gadget::generateConstraints(){
	if (standAlone_){
		unpackArg1_g_->generateConstraints();
		unpackArg2_g_->generateConstraints();
	}
	size_t registerLength = tinyRAMparams()->registerLength();
	CircuitPolynomial polynomial;
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	for (unsigned int i = 0; i < registerLength; ++i){
		LinearCombination first = Algebra::one() + unpackedArg1_[i];
		LinearCombination second = Algebra::one() + unpackedArg2_[i];
		CircuitPolynomial tmpPoly(first * second);
		polynomial = polynomial + (tmpPoly + Algebra::one()) * x_i;
		x_i *= x;
	}
	pb_->addGeneralConstraint(polynomial + results_.result_, "sum_{i=0}^{registerLength-1} (1+(1+uinput1[i]) * (1+uinput2[i])) * X^i + result = 0", Opcode::OR);
	// Flag constraint
	CircuitPolynomial flagConstarint1(results_.result_ *  results_.flag_);
	CircuitPolynomial flagConstarint2(results_.result_ *  pInverse_);
	CircuitPolynomial flagConstraint3(Algebra::one() + results_.flag_);
	pb_->addGeneralConstraint(flagConstarint1, "flag * result= 0", Opcode::OR);
	pb_->addGeneralConstraint(flagConstarint2 + flagConstraint3, "result * invResult = 1 - flag", Opcode::OR);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::OR);
}

void ALU_OR_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
			unpackArg1_g_->generateWitness();
			unpackArg2_g_->generateWitness();
	size_t registerLength = tinyRAMparams()->registerLength();
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	Algebra::FElem res=Algebra::zero();
	for (unsigned int i = 0; i < registerLength; i++){
		Algebra::FElem first = Algebra::one() + pb_->val(unpackedArg1_[i]);
		Algebra::FElem second = Algebra::one() + pb_->val(unpackedArg2_[i]);
		res += (Algebra::one() + first*second) * x_i;
		x_i *= x;
	}
	pb_->val(results_.result_) = res;
	pb_->val(results_.flag_) = (res == Algebra::zero()) ? Algebra::one() : Algebra::zero();
	pb_->val(pInverse_) = (res != Algebra::zero()) ? res.inverse() : Algebra::one();
	for (int i = 0; i < REGISTER_LENGTH; i++){
		// Other opcodes use them and we multiply the constraint by selector
		// In order to be evaluated correctly the must be initialized as 0
		pb_->val(opcodeResult_[i]) = Algebra::zero();
		pb_->val(opcodeCarry_[i]) = Algebra::zero();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_NOT_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
ALU_NOT_Gadget::ALU_NOT_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}


void ALU_NOT_Gadget::init(){}

GadgetPtr ALU_NOT_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_NOT_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_NOT_Gadget::generateConstraints(){
	size_t registerLength = tinyRAMparams()->registerLength();
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	Algebra::FElem allOnes = Algebra::zero();
	for (unsigned int i = 0; i < registerLength; i++){
		allOnes += x_i;
		x_i *= x;
	}
	CircuitPolynomial poly(allOnes + inputs_.arg1_val_ + results_.result_);
	pb_->addGeneralConstraint(poly, "(111...1) + arg1 + result = 0", Opcode::NOT);
	// Flag constraint
	CircuitPolynomial flagConstraint1(results_.result_ *  results_.flag_);
	CircuitPolynomial flagConstraint2(results_.result_ *  pInverse_);
	CircuitPolynomial flagConstraint3(Algebra::one() + results_.flag_);
	pb_->addGeneralConstraint(flagConstraint1, "flag * result= 0", Opcode::NOT);
	pb_->addGeneralConstraint(flagConstraint2 + flagConstraint3, "result * invResult = 1 - flag", Opcode::NOT);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::NOT);
}

void ALU_NOT_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	size_t registerLength = tinyRAMparams()->registerLength();
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	Algebra::FElem allOnes = Algebra::zero();
	for (unsigned int i = 0; i < registerLength; i++){
		allOnes += x_i;
		x_i *= x;
	}
	Algebra::FElem res = allOnes + pb_->val(inputs_.arg1_val_);
	pb_->val(results_.result_) = res ;
	pb_->val(results_.flag_) = (res == Algebra::zero()) ? Algebra::one() : Algebra::zero();
	pb_->val(pInverse_) = (res != Algebra::zero()) ? res.inverse() : Algebra::one();
	for (int i = 0; i < REGISTER_LENGTH; i++){
		// Other opcodes use them and we multiply the constraint by selector
		// In order to be evaluated correctly the must be initialized as 0
		pb_->val(unpackedArg1_[i]) = Algebra::zero();
		pb_->val(unpackedArg2_[i]) = Algebra::zero();
		pb_->val(opcodeResult_[i]) = Algebra::zero();
		pb_->val(opcodeCarry_[i]) = Algebra::zero();
	}
}


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_ADD_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_ADD_Gadget::ALU_ADD_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
									:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_ADD_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_ADD_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_ADD_Gadget::init(){
	//size_t registerLength = tinyRAMparams()->registerLength();
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::ADD);
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::ADD);
	packResult_g_ = CompressionPacking_Gadget::create(pb_, opcodeResult_, results_.result_, PackingMode::PACK, Opcode::ADD);
	add_g_ = Addition_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, opcodeResult_, opcodeCarry_, results_.flag_, Opcode::ADD);
}

void ALU_ADD_Gadget::generateConstraints(){
	//size_t registerLength = tinyRAMparams()->registerLength();
	if (standAlone_){
		unpackArg1_g_->generateConstraints();
		unpackArg2_g_->generateConstraints();
	}
	add_g_->generateConstraints();
	packResult_g_->generateConstraints();
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::SUB);
}
void ALU_ADD_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpackArg1_g_->generateWitness();
	unpackArg2_g_->generateWitness();
	add_g_->generateWitness();
	packResult_g_->generateWitness();
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SUB_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_SUB_Gadget::ALU_SUB_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
									:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_SUB_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_SUB_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_SUB_Gadget::init(){
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::SUB);	
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, results_.result_, PackingMode::UNPACK, Opcode::SUB);
	packResult_g_ = CompressionPacking_Gadget::create(pb_, opcodeResult_, inputs_.arg1_val_, PackingMode::PACK, Opcode::SUB);
	add_g_ = Addition_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, opcodeResult_, opcodeCarry_, results_.flag_, Opcode::SUB);
}

void ALU_SUB_Gadget::generateConstraints(){
	if (standAlone_){
		unpackArg1_g_->generateConstraints();
		unpackArg2_g_->generateConstraints();
	}
	add_g_->generateConstraints();
	packResult_g_->generateConstraints();
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::SUB);
}
void ALU_SUB_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpackArg1_g_->generateWitness();
	size_t registerLength = tinyRAMparams()->registerLength();
	size_t operand1 = mapFieldElementToInteger(0, registerLength, val(inputs_.arg1_val_));
	size_t operand2 = mapFieldElementToInteger(0, registerLength, val(inputs_.arg2_val_));
	val(results_.result_) = mapIntegerToFieldElement(0, registerLength, /* 0x100000000ULL + */ operand1 - operand2);
	unpackArg2_g_->generateWitness();
	add_g_->generateWitness();
	//don't do: packResult_g_->generateWitness();
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_MULL_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_MULL_Gadget::ALU_MULL_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
	:Gadget(pb), ALU_Component_Gadget(pb, inputs, results),
	multPartials1_(opcodeAux1_), multPartials2_(opcodeAux2_), unpackedResult_(opcodeAux3_),
	witnessHighBits_(opcodeAux4_), dmultPartials1_(opcodeAux5_), dmultPartials2_(opcodeAux6_){}

GadgetPtr ALU_MULL_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_MULL_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_MULL_Gadget::init(){
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::MULL);
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::MULL);
	mult_g_ = Multiplication_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, multPartials1_, multPartials2_, false, Opcode::MULL);
	unpackResult_g_ = CompressionPacking_Gadget::create(pb_, unpackedResult_, results_.result_, PackingMode::UNPACK, Opcode::MULL);
	dmultPack_g_ = DoubleMultPack_Gadget::create(pb_, unpackedResult_, witnessHighBits_, dmultPartials1_, dmultPartials2_, multPartials2_[0], false, Opcode::MULL);
	packHighBits_g_ = CompressionPacking_Gadget::create(pb_, witnessHighBits_, multPartials1_[0], PackingMode::PACK, Opcode::MULL);
}

void ALU_MULL_Gadget::generateConstraints(){
	if (standAlone_){
		unpackArg1_g_->generateConstraints();
		unpackArg2_g_->generateConstraints();
	}
	mult_g_->generateConstraints();
	unpackResult_g_->generateConstraints();
	const size_t & registerLength = tinyRAMparams()->registerLength();
	for (unsigned int i = 0; i < registerLength; ++i) {
		enforceBooleanity(witnessHighBits_[i], Opcode::MULL);
	}
	dmultPack_g_->generateConstraints();
	//flag:
	packHighBits_g_->generateConstraints();
	pb_->addGeneralConstraint(multPartials1_[0] * (multPartials1_[0] * pInverse_ + Algebra::one()), "flag inv", Opcode::MULL);
	pb_->addGeneralConstraint(results_.flag_ + multPartials1_[0] * pInverse_, "mull flag", Opcode::MULL);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::MULL);
}

void ALU_MULL_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	const size_t & registerLength = tinyRAMparams()->registerLength();
	unpackArg1_g_->generateWitness();
	unpackArg2_g_->generateWitness();
	mult_g_->generateWitness();
	const size_t v = mapFieldElementToInteger(0, registerLength, val(inputs_.arg1_val_)) *
		mapFieldElementToInteger(0, registerLength, val(inputs_.arg2_val_));
	pb_->val(results_.result_) = mapIntegerToFieldElement(0, registerLength, v);
	unpackResult_g_->generateWitness();
	for (size_t i = 0; i < registerLength; i++) {
		pb_->val(witnessHighBits_[i]) = (v >> registerLength) & (size_t(1) << i) ? Algebra::one() : Algebra::zero();
	}
	dmultPack_g_->generateWitness();
	//flag:
	packHighBits_g_->generateWitness();
	if (Algebra::zero() == val(multPartials1_[0])) {
		pb_->val(pInverse_) = Algebra::zero();
		pb_->val(results_.flag_) = Algebra::zero();
	}
	else {
		pb_->val(pInverse_) = (val(multPartials1_[0])).inverse();
		pb_->val(results_.flag_) = Algebra::one();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_UMULH_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_UMULH_Gadget::ALU_UMULH_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
	:Gadget(pb), ALU_Component_Gadget(pb, inputs, results),
	multPartials1_(opcodeAux1_), multPartials2_(opcodeAux2_), unpackedResult_(opcodeAux4_),
	witnessLowBits_(opcodeAux3_), dmultPartials1_(opcodeAux5_), dmultPartials2_(opcodeAux6_){}

GadgetPtr ALU_UMULH_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_UMULH_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_UMULH_Gadget::init(){
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::UMULH);
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::UMULH);
	mult_g_ = Multiplication_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, multPartials1_, multPartials2_, false, Opcode::UMULH);
	unpackResult_g_ = CompressionPacking_Gadget::create(pb_, unpackedResult_, results_.result_, PackingMode::UNPACK, Opcode::UMULH);
	dmultPack_g_ = DoubleMultPack_Gadget::create(pb_, witnessLowBits_, unpackedResult_, dmultPartials1_, dmultPartials2_, multPartials2_[0], false, Opcode::UMULH);
}

void ALU_UMULH_Gadget::generateConstraints(){
	if (standAlone_){
		unpackArg1_g_->generateConstraints();
		unpackArg2_g_->generateConstraints();
	}
	mult_g_->generateConstraints();
	unpackResult_g_->generateConstraints();
	const size_t & registerLength = tinyRAMparams()->registerLength();
	for (unsigned int i = 0; i < registerLength; ++i) {
		enforceBooleanity(witnessLowBits_[i], Opcode::UMULH);
	}
	dmultPack_g_->generateConstraints();
	//flag:
	pb_->addGeneralConstraint(results_.result_ * (results_.result_ * pInverse_ + Algebra::one()), "flag inv", Opcode::UMULH);
	pb_->addGeneralConstraint(results_.flag_ + results_.result_ * pInverse_, "umulh flag", Opcode::UMULH);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::UMULH);
}

void ALU_UMULH_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	const size_t & registerLength = tinyRAMparams()->registerLength();
	unpackArg1_g_->generateWitness();
	unpackArg2_g_->generateWitness();
	mult_g_->generateWitness();
	const size_t v = mapFieldElementToInteger(0, registerLength, val(inputs_.arg1_val_)) *
			mapFieldElementToInteger(0, registerLength, val(inputs_.arg2_val_));
	pb_->val(results_.result_) = mapIntegerToFieldElement(0, registerLength, v >> registerLength);
	unpackResult_g_->generateWitness();
	for (size_t i = 0; i < registerLength; i++) {
		pb_->val(witnessLowBits_[i]) = (v >> i) & size_t(1) ? Algebra::one() : Algebra::zero();
	}
	dmultPack_g_->generateWitness();
	//flag:
	if (Algebra::zero() == val(results_.result_)) {
		pb_->val(pInverse_) = Algebra::zero();
		pb_->val(results_.flag_) = Algebra::zero();
	}
	else {
		pb_->val(pInverse_) = (val(results_.result_)).inverse();
		pb_->val(results_.flag_) = Algebra::one();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SMULH_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_SMULH_Gadget::ALU_SMULH_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
	:Gadget(pb), ALU_Component_Gadget(pb, inputs, results),
	multPartials1_(opcodeAux1_), multPartials2_(opcodeAux2_), unpackedResult_(opcodeAux4_),
	witnessLowBits_(opcodeAux3_), dmultPartials1_(opcodeAux5_), dmultPartials2_(opcodeAux6_){}

GadgetPtr ALU_SMULH_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_SMULH_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_SMULH_Gadget::init(){
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::SMULH);
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::SMULH);
	mult_g_ = Multiplication_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, multPartials1_, multPartials2_, true, Opcode::SMULH);
	unpackResult_g_ = CompressionPacking_Gadget::create(pb_, unpackedResult_, results_.result_, PackingMode::UNPACK, Opcode::SMULH);
	dmultPack_g_ = DoubleMultPack_Gadget::create(pb_, witnessLowBits_, unpackedResult_, dmultPartials1_, dmultPartials2_, multPartials2_[0], true, Opcode::SMULH);
}

void ALU_SMULH_Gadget::generateConstraints(){
	if (standAlone_){
		unpackArg1_g_->generateConstraints();
		unpackArg2_g_->generateConstraints();
	}
	mult_g_->generateConstraints();
	unpackResult_g_->generateConstraints();
	const size_t & registerLength = tinyRAMparams()->registerLength();
	for (unsigned int i = 0; i < registerLength; ++i) {
		enforceBooleanity(witnessLowBits_[i], Opcode::SMULH);
	}
	dmultPack_g_->generateConstraints();
	//flag:
	const Algebra::Variable invNegative = dmultPartials1_[0];
	const Algebra::FElem allOnes = mapIntegerToFieldElement(0, registerLength, 0xFFFF);
	pb_->addGeneralConstraint(results_.result_ * (results_.result_ * pInverse_ + Algebra::one()), "flag positive inv", Opcode::SMULH);
	pb_->addGeneralConstraint((results_.result_+allOnes) * ((results_.result_+allOnes) * invNegative + Algebra::one()), "flag negative inv", Opcode::SMULH);
	pb_->addGeneralConstraint(results_.flag_ + results_.result_*pInverse_ + (results_.result_+allOnes)*invNegative + Algebra::one(), "flag", Opcode::SMULH);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::SMULH);
}

void ALU_SMULH_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	const size_t & registerLength = tinyRAMparams()->registerLength();
	unpackArg1_g_->generateWitness();
	unpackArg2_g_->generateWitness();
	mult_g_->generateWitness();
	const int32_t v = int16_t(mapFieldElementToInteger(0, registerLength, val(inputs_.arg1_val_))) *
		int16_t(mapFieldElementToInteger(0, registerLength, val(inputs_.arg2_val_)));
	pb_->val(results_.result_) = mapIntegerToFieldElement(0, registerLength, v >> registerLength);
	unpackResult_g_->generateWitness();
	for (unsigned int i = 0; i < registerLength; i++) {
		pb_->val(witnessLowBits_[i]) = (v >> i) & 1 ? Algebra::one() : Algebra::zero();
	}
	dmultPack_g_->generateWitness();
	//flag:
	const Algebra::Variable invNegative = dmultPartials1_[0];
	const Algebra::FElem allOnes = mapIntegerToFieldElement(0, registerLength, 0xFFFF);
	if (Algebra::zero() != val(results_.result_)) {
		pb_->val(pInverse_) = (val(results_.result_)).inverse();
		if (allOnes != val(results_.result_)) {
			pb_->val(invNegative) = (val(results_.result_) + allOnes).inverse();
			pb_->val(results_.flag_) = Algebra::one();
			return;
		}
		pb_->val(results_.flag_) = Algebra::zero();
		return;
	}
	pb_->val(invNegative) = (val(results_.result_) + allOnes).inverse();
	pb_->val(results_.flag_) = Algebra::zero();
}


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_UDIV_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_UDIV_Gadget::ALU_UDIV_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results) :
    Gadget(pb),
    ALU_Component_Gadget(pb, inputs, results),
    cmpFlags_(opcodeAux7_),
    isGEQ_(opcodeAux7_[0]),
    unpackedResult_(opcodeAux3_),
    multPartials1_(opcodeAux1_),
    multPartials2_(opcodeAux2_),
    mpartialsArg1_(opcodeAux4_),
    mpackArg1_(opcodeAux4_[REGISTER_LENGTH - 1]),
    mpartialsRemainder_(opcodeAux6_),
    mpackRemainder_(opcodeAux6_[REGISTER_LENGTH - 1]),
    witnessRemainder_(opcodeAux5_){};

GadgetPtr ALU_UDIV_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_UDIV_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_UDIV_Gadget::init(){
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::UDIV);
	unpackResult_g_ = CompressionPacking_Gadget::create(pb_, unpackedResult_, results_.result_, PackingMode::UNPACK, Opcode::UDIV);
	mult_g_ = Multiplication_Gadget::create(pb_, unpackedArg2_, unpackedResult_, multPartials1_, multPartials2_, false, Opcode::UDIV);
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::UDIV);
	multPackArg1_g_ = MultiplicationPacking_Gadget::create(pb_, unpackedArg1_, mpartialsArg1_, mpackArg1_, false, PackingMode::PACK, Opcode::UDIV);
	multPackRemainder_g_ = MultiplicationPacking_Gadget::create(pb_, witnessRemainder_, mpartialsRemainder_, mpackRemainder_, false, PackingMode::PACK, Opcode::UDIV);
	GEQ_g_ = GreaterEqual_Gadget::create(pb_, unpackedArg2_, witnessRemainder_, cmpFlags_, isGEQ_, false, Opcode::UDIV);
}

void ALU_UDIV_Gadget::generateConstraints(){
	if (standAlone_)
	unpackArg2_g_->generateConstraints();
	unpackResult_g_->generateConstraints();
 	const size_t & registerLength = tinyRAMparams()->registerLength();
	for (size_t i = 0; i < registerLength; i++)
		enforceBooleanity(witnessRemainder_[i], Opcode::UMOD);
	mult_g_->generateConstraints();
	if (standAlone_)
	unpackArg1_g_->generateConstraints();
	multPackArg1_g_->generateConstraints();
	multPackRemainder_g_->generateConstraints();
	pb_->addGeneralConstraint(inputs_.arg2_val_ * (inputs_.arg2_val_ * pInverse_ + Algebra::one()), "divisor inv", Opcode::UDIV);
	pb_->addGeneralConstraint(
		inputs_.arg2_val_ * (multPartials2_[0] + mpackArg1_ * mpackRemainder_) +
		(inputs_.arg2_val_ * pInverse_ + Algebra::one()) * results_.result_,
		"dividor!=0 -> res*divisor + remainder == arg1", Opcode::UDIV);
	pb_->addGeneralConstraint(inputs_.arg2_val_ * (isGEQ_ + Algebra::one()), "divisor!=0 -> remainder<divisor", Opcode::UDIV);
	pb_->addGeneralConstraint(results_.flag_ + inputs_.arg2_val_ * pInverse_ + Algebra::one(), "udiv flag", Opcode::UDIV);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::UDIV);
}

void ALU_UDIV_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	const size_t & registerLength = tinyRAMparams()->registerLength();
	const size_t operand2 = mapFieldElementToInteger(0, registerLength, val(inputs_.arg2_val_));
	if (0 == operand2){
		pb_->val(pInverse_) = Algebra::zero(); //needed?
		pb_->val(results_.result_) = Algebra::zero();
		pb_->val(results_.flag_) = Algebra::one();
	}
	else {
		pb_->val(pInverse_) = (val(inputs_.arg2_val_)).inverse();
		const size_t operand1 = mapFieldElementToInteger(0, registerLength, val(inputs_.arg1_val_));
		val(results_.result_) = mapIntegerToFieldElement(0, registerLength, operand1 / operand2);
		const size_t rem = operand1 % operand2;
		for (size_t i = 0; i < registerLength; i++)
			pb_->val(witnessRemainder_[i]) = (rem >> i) & size_t(1) ? Algebra::one() : Algebra::zero();
		pb_->val(results_.flag_) = Algebra::zero();
	}
		unpackArg1_g_->generateWitness();
		unpackArg2_g_->generateWitness();
	unpackResult_g_->generateWitness();
	mult_g_->generateWitness();
	std::dynamic_pointer_cast<MultiplicationPacking_Gadget>(multPackArg1_g_)->generateWitness();
	std::dynamic_pointer_cast<MultiplicationPacking_Gadget>(multPackRemainder_g_)->generateWitness();
	GEQ_g_->generateWitness();
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_UMOD_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_UMOD_Gadget::ALU_UMOD_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results) :
    Gadget(pb),
    ALU_Component_Gadget(pb, inputs, results),
    cmpFlags_(opcodeAux7_),
    isGEQ_(opcodeAux7_[0]),
    multPartials1_(opcodeAux1_),
    multPartials2_(opcodeAux2_),
    mpartialsArg1_(opcodeAux4_),
    mpackArg1_(opcodeAux4_[REGISTER_LENGTH - 1]),
    mpartialsRemainder_(opcodeAux6_),
    mpackRemainder_(opcodeAux6_[REGISTER_LENGTH - 1]),
    witnessRemainder_(opcodeAux5_),
    witnessResult_(opcodeAux3_){};

GadgetPtr ALU_UMOD_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_UMOD_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_UMOD_Gadget::init(){
	unpackArg2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::UMOD);
	mult_g_ = Multiplication_Gadget::create(pb_, unpackedArg2_, witnessResult_, multPartials1_, multPartials2_, false, Opcode::UMOD);
	 unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK, Opcode::UMOD);
	multPackArg1_g_ = MultiplicationPacking_Gadget::create(pb_, unpackedArg1_, mpartialsArg1_, mpackArg1_, false, PackingMode::PACK, Opcode::UMOD);
	multPackRemainder_g_ = MultiplicationPacking_Gadget::create(pb_, witnessRemainder_, mpartialsRemainder_, mpackRemainder_, false, PackingMode::PACK, Opcode::UMOD);
	GEQ_g_ = GreaterEqual_Gadget::create(pb_, unpackedArg2_, witnessRemainder_, cmpFlags_, isGEQ_, false, Opcode::UMOD);
	packResult_g_ = CompressionPacking_Gadget::create(pb_, witnessRemainder_, results_.result_, PackingMode::PACK, Opcode::UMOD);
}

void ALU_UMOD_Gadget::generateConstraints(){
	if (standAlone_)
	unpackArg2_g_->generateConstraints();
	const size_t & registerLength = tinyRAMparams()->registerLength();
	for (size_t i = 0; i < registerLength; i++) {
		enforceBooleanity(witnessResult_[i], Opcode::UMOD);
		enforceBooleanity(witnessRemainder_[i], Opcode::UMOD);
	}
	mult_g_->generateConstraints();
	if (standAlone_)
	unpackArg1_g_->generateConstraints();
	multPackArg1_g_->generateConstraints();
	multPackRemainder_g_->generateConstraints();
	pb_->addGeneralConstraint(inputs_.arg2_val_ * (inputs_.arg2_val_ * pInverse_ + Algebra::one()), "divisor inv", Opcode::UMOD);
	pb_->addGeneralConstraint(
		inputs_.arg2_val_ * (multPartials2_[0] + mpackArg1_ * mpackRemainder_) +
		(inputs_.arg2_val_ * pInverse_ + Algebra::one()) * results_.result_,
		"dividor!=0 -> res*divisor + remainder == arg1", Opcode::UMOD);
	pb_->addGeneralConstraint(inputs_.arg2_val_ * (isGEQ_ + Algebra::one()), "divisor!=0 -> remainder<divisor", Opcode::UMOD);
	pb_->addGeneralConstraint(results_.flag_ + inputs_.arg2_val_ * pInverse_ + Algebra::one(), "umod flag", Opcode::UMOD);
	packResult_g_->generateConstraints();
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::UMOD);
}

void ALU_UMOD_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	const size_t & registerLength = tinyRAMparams()->registerLength();
	const size_t operand2 = mapFieldElementToInteger(0, registerLength, val(inputs_.arg2_val_));
	if (0 == operand2){
		pb_->val(pInverse_) = Algebra::zero(); //needed?
		pb_->val(results_.result_) = Algebra::zero();
		pb_->val(results_.flag_) = Algebra::one();
	}
	else {
		pb_->val(pInverse_) = (val(inputs_.arg2_val_)).inverse();
		const size_t operand1 = mapFieldElementToInteger(0, registerLength, val(inputs_.arg1_val_));
		const size_t result = operand1 / operand2;
		const size_t rem = operand1 % operand2;
		val(results_.result_) = mapIntegerToFieldElement(0, registerLength, rem);
		for (size_t i = 0; i < registerLength; i++) {
			pb_->val(witnessResult_[i]) = (result >> i) & size_t(1) ? Algebra::one() : Algebra::zero();
			pb_->val(witnessRemainder_[i]) = (rem >> i) & size_t(1) ? Algebra::one() : Algebra::zero();
		}
		pb_->val(results_.flag_) = Algebra::zero();
	}
		unpackArg1_g_->generateWitness();
		unpackArg2_g_->generateWitness();
	mult_g_->generateWitness();
	std::dynamic_pointer_cast<MultiplicationPacking_Gadget>(multPackArg1_g_)->generateWitness();
	std::dynamic_pointer_cast<MultiplicationPacking_Gadget>(multPackRemainder_g_)->generateWitness();
	GEQ_g_->generateWitness();
	packResult_g_->generateWitness();
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPE_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_CMPE_Gadget::ALU_CMPE_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
										:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_CMPE_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_CMPE_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_CMPE_Gadget::init(){}

void ALU_CMPE_Gadget::generateConstraints(){
	CircuitPolynomial flagConstarint1((inputs_.arg1_val_ + inputs_.arg2_val_) *  results_.flag_);
	CircuitPolynomial flagConstarint2((inputs_.arg1_val_ + inputs_.arg2_val_)*  pInverse_);
	CircuitPolynomial flagConstraint3(Algebra::one() + results_.flag_);
	pb_->addGeneralConstraint(flagConstarint1, "flag * (arg1_val + arg2_val)= 0", Opcode::NOT);
	pb_->addGeneralConstraint(flagConstarint2 + flagConstraint3, "(arg1_val + arg2_val) * invResult = 1 - flag", Opcode::CMPE);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::CMPE);
}

void ALU_CMPE_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	FElem arg1Val = pb_->val(inputs_.arg1_val_);
	FElem arg2Val = pb_->val(inputs_.arg2_val_);
	pb_->val(results_.flag_) = arg1Val == arg2Val ? Algebra::one() : Algebra::zero();
	pb_->val(pInverse_) = arg1Val == arg2Val ? Algebra::one() : (arg1Val + arg2Val).inverse();
	pb_->val(results_.result_) = Algebra::zero(); // We don't care which value result_ holds - needed for the coloring
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPA_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_CMPA_Gadget::ALU_CMPA_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
										:Gadget(pb), ALU_Component_Gadget(pb, inputs, results),
										 cmpFlags_(opcodeAux2_), isGEQ_(opcodeAux2_[0]){}

GadgetPtr ALU_CMPA_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_CMPA_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_CMPA_Gadget::init(){
			unpack1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK ,Opcode::CMPA);
			unpack2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK ,Opcode::CMPA);
	compareArgs_ = GreaterEqual_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, cmpFlags_, isGEQ_, false, Opcode::CMPA);
}

void ALU_CMPA_Gadget::generateConstraints(){
	if (standAlone_){
		unpack1_g_->generateConstraints();
		unpack2_g_->generateConstraints();
	}
	compareArgs_->generateConstraints();
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	const Algebra::FElem inv = (Algebra::one() + g).inverse();
	CircuitPolynomial c(results_.flag_ + isGEQ_*(isGEQ_ + g)*inv);
	pb_->addGeneralConstraint(c, "flag=isGEQ*(isGEQ+g)*(1+g)^{-1}", Opcode::CMPA);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::CMPA);
}

void ALU_CMPA_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpack1_g_->generateWitness();
	unpack2_g_->generateWitness();
	compareArgs_->generateWitness();
	FElem arg1Val = pb_->val(inputs_.arg1_val_);
	FElem arg2Val = pb_->val(inputs_.arg2_val_);
	pb_->val(results_.flag_) = (Algebra::one()==val(isGEQ_)) ? Algebra::one() : Algebra::zero();
	pb_->val(results_.result_) = Algebra::zero(); // We don't care which value result_ holds - needed for the coloring

}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPAE_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_CMPAE_Gadget::ALU_CMPAE_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
										:Gadget(pb), ALU_Component_Gadget(pb, inputs, results),
										 cmpFlags_(opcodeAux2_), isGEQ_(opcodeAux2_[0]){}

GadgetPtr ALU_CMPAE_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_CMPAE_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_CMPAE_Gadget::init(){
		unpack1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK ,Opcode::CMPAE);
		unpack2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK ,Opcode::CMPAE);
	compareArgs_ = GreaterEqual_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, cmpFlags_, isGEQ_, false, Opcode::CMPAE);
}

void ALU_CMPAE_Gadget::generateConstraints(){
	if (standAlone_){
		unpack1_g_->generateConstraints();
		unpack2_g_->generateConstraints();
	}
	compareArgs_->generateConstraints();
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	const Algebra::FElem inv1 = (Algebra::one() + g).inverse();
	const Algebra::FElem inv2 = g.inverse() * inv1;
	CircuitPolynomial c(results_.flag_ + isGEQ_*(isGEQ_ + g)*inv1 + isGEQ_*(isGEQ_ + Algebra::one())*inv2);
	pb_->addGeneralConstraint(c, "flag=isGEQ*(isGEQ+g)*(1+g)^{-1}+isGEQ*(isGEQ+1)*(g(1+g))^{-1}", Opcode::CMPAE);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::CMPAE);
}

void ALU_CMPAE_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpack1_g_->generateWitness();
	unpack2_g_->generateWitness();
	compareArgs_->generateWitness();
	FElem flag = pb_->val(isGEQ_);
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	pb_->val(results_.flag_) = ((Algebra::one()==flag)||(g==flag)) ? Algebra::one() : Algebra::zero();
	pb_->val(results_.result_) = Algebra::zero(); // We don't care which value result_ holds - needed for the coloring
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPG_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_CMPG_Gadget::ALU_CMPG_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
										:Gadget(pb), ALU_Component_Gadget(pb, inputs, results),
										 cmpFlags_(opcodeAux2_), isGEQ_(opcodeAux2_[0]){}

GadgetPtr ALU_CMPG_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_CMPG_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_CMPG_Gadget::init(){
		unpack1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK ,Opcode::CMPG);
		unpack2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK ,Opcode::CMPG);
	compareArgs_ = GreaterEqual_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, cmpFlags_, isGEQ_, true, Opcode::CMPG);
}

void ALU_CMPG_Gadget::generateConstraints(){
	if (standAlone_){
		unpack1_g_->generateConstraints();
		unpack2_g_->generateConstraints();
	}
	compareArgs_->generateConstraints();
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	const Algebra::FElem inv = (Algebra::one() + g).inverse();
	CircuitPolynomial c(results_.flag_ + isGEQ_*(isGEQ_ + g)*inv);
	pb_->addGeneralConstraint(c, "flag=isGEQ*(isGEQ+g)*(1+g)^{-1}", Opcode::CMPG);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::CMPG);
}

void ALU_CMPG_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpack1_g_->generateWitness();
	unpack2_g_->generateWitness();
	compareArgs_->generateWitness();
	FElem arg1Val = pb_->val(inputs_.arg1_val_);
	FElem arg2Val = pb_->val(inputs_.arg2_val_);
	pb_->val(results_.flag_) = (Algebra::one()==val(isGEQ_)) ? Algebra::one() : Algebra::zero();
	pb_->val(results_.result_) = Algebra::zero(); // We don't care which value result_ holds - needed for the coloring

}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPGE_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_CMPGE_Gadget::ALU_CMPGE_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
										:Gadget(pb), ALU_Component_Gadget(pb, inputs, results),
										 cmpFlags_(opcodeAux2_), isGEQ_(opcodeAux2_[0]){}

GadgetPtr ALU_CMPGE_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_CMPGE_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_CMPGE_Gadget::init(){
	unpack1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg1_val_, PackingMode::UNPACK ,Opcode::CMPGE);
	unpack2_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, inputs_.arg2_val_, PackingMode::UNPACK ,Opcode::CMPGE);
	compareArgs_ = GreaterEqual_Gadget::create(pb_, unpackedArg1_, unpackedArg2_, cmpFlags_, isGEQ_, true, Opcode::CMPGE);
}

void ALU_CMPGE_Gadget::generateConstraints(){
	if (standAlone_){
		unpack1_g_->generateConstraints();
		unpack2_g_->generateConstraints();
	}
	compareArgs_->generateConstraints();
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	const Algebra::FElem inv1 = (Algebra::one() + g).inverse();
	const Algebra::FElem inv2 = g.inverse() * inv1;
	CircuitPolynomial c(results_.flag_ + isGEQ_*(isGEQ_ + g)*inv1 + isGEQ_*(isGEQ_ + Algebra::one())*inv2);
	pb_->addGeneralConstraint(c, "flag=isGEQ*(isGEQ+g)*(1+g)^{-1}+isGEQ*(isGEQ+1)*(g(1+g))^{-1}", Opcode::CMPGE);
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::CMPGE);
}

void ALU_CMPGE_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpack1_g_->generateWitness();
	unpack2_g_->generateWitness();
	compareArgs_->generateWitness();
	FElem flag = pb_->val(isGEQ_);
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	pb_->val(results_.flag_) = ((Algebra::one()==flag)||(g==flag)) ? Algebra::one() : Algebra::zero();
	pb_->val(results_.result_) = Algebra::zero(); // We don't care which value result_ holds - needed for the coloring
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SHL_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_SHL_Gadget::ALU_SHL_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
	:Gadget(pb), ALU_Component_Gadget(pb, inputs, results), auxArr_(opcodeAux1_){}

GadgetPtr ALU_SHL_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_SHL_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_SHL_Gadget::init(){
	size_t registerLength = tinyRAMparams()->registerLength();
	unpackDouble_g_ = DoubleUnpack_Gadget::create(pb_, unpackedArg1_, unpackedArg2_,
		auxArr_[registerLength - ALU_SHL_Gadget::idxs::dRes], Opcode::SHL);
	packResult_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, results_.result_, PackingMode::PACK, Opcode::SHL);
}

void ALU_SHL_Gadget::generateConstraints(){
	const long registerLength = long((tinyRAMparams()->registerLength()));
	const long registerLogLen = long(log2(registerLength));
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::CircuitPolynomial tmpPoly, witnessPoly;
	Algebra::LinearCombination tmpLC;
	const Variable remainder = auxArr_[registerLength-ALU_SHL_Gadget::idxs::rem];
	Algebra::FElem x_i = Algebra::one();
	for (int i = 0; i < registerLogLen; ++i){
		const Variable v = auxArr_[i];
		const Variable w = auxArr_[i+registerLogLen];
		tmpLC = tmpLC + x_i * v;
		x_i *= x;
		enforceBooleanity(v, Opcode::SHL);
		witnessPoly = Algebra::invExtrConsts[i] * (w*w + w) + remainder;
		pb_->addGeneralConstraint(witnessPoly, "traceWitness", Opcode::SHL);
	}
	tmpLC = tmpLC + remainder;
	pb_->addGeneralConstraint(tmpLC + inputs_.arg2_val_, "[A] = tmpPoly", Opcode::SHL);

	const Variable wFlag = auxArr_[registerLength-ALU_SHL_Gadget::idxs::witnessFlag];
	witnessPoly = Algebra::invExtrConsts[registerLength-1] * (wFlag*wFlag + wFlag) +
		inputs_.arg1_val_ + Algebra::power(x, registerLength-1) * results_.flag_;
	pb_->addGeneralConstraint(witnessPoly, "flagWitness", Opcode::SHL);
	enforceBooleanity(results_.flag_, Opcode::SHL);

	tmpPoly = inputs_.arg1_val_;
	x_i = x;
	for (int i = 0; i < registerLogLen; ++i){
		const Variable v = auxArr_[i];
		tmpPoly = tmpPoly * (Algebra::one() + v + v * x_i);
		x_i *= x_i;
	}
	const Variable invRem = auxArr_[registerLength-ALU_SHL_Gadget::idxs::invRem];
	tmpPoly = tmpPoly * (remainder * invRem + Algebra::one());
	const Variable doubleResult = auxArr_[registerLength-ALU_SHL_Gadget::idxs::dRes];
	pb_->addGeneralConstraint(tmpPoly + doubleResult, "doubleResult = tmpPoly", Opcode::SHL);

	tmpPoly = remainder * (remainder * invRem + Algebra::one());
	pb_->addGeneralConstraint(tmpPoly, "rem*(rem * invRem + 1)", Opcode::SHL);
	if (standAlone_)
	unpackDouble_g_->generateConstraints();
	packResult_g_->generateConstraints();

	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::SHL);
}

void ALU_SHL_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	const long registerLength = long((tinyRAMparams()->registerLength()));
	const long registerLogLen = long(log2(registerLength));

	size_t operand2 = Algebra::mapFieldElementToInteger(0, EXTDIM, pb_->val(inputs_.arg2_val_));
	size_t r = operand2;
	for (int i = 0; i < registerLogLen; ++i){
		const Variable v = auxArr_[i];
		size_t mask = size_t(1) << i;
		pb_->val(v) = (r & mask) ? Algebra::one() : Algebra::zero();
		r = r & (~mask);
	}
	const Algebra::FElem remainder = Algebra::mapIntegerToFieldElement(0, EXTDIM, r);
	pb_->val(auxArr_[registerLength - ALU_SHL_Gadget::idxs::rem]) = remainder;

	for (int i = 0; i < registerLogLen; ++i){
		const Variable w = auxArr_[i+registerLogLen];
		pb_->val(w) = extractBit(remainder, i);
	}

	const FElem operand1 = pb_->val(inputs_.arg1_val_);
	size_t doubleRes = Algebra::mapFieldElementToInteger(0, EXTDIM, operand1);
	const FElem fToggled = Algebra::mapIntegerToFieldElement(0, EXTDIM,
		doubleRes & (~(size_t(1) << (registerLength - 1))));
	//const FElem fRes = Algebra::mapIntegerToFieldElement(0, 1, doubleRes >> (registerLength - 1));
	const FElem fRes = (doubleRes >> (registerLength - 1)) ? Algebra::one() : Algebra::zero();
	pb_->val(results_.flag_) = fRes;
	pb_->val(auxArr_[registerLength - ALU_SHL_Gadget::idxs::witnessFlag]) =
		extractBit(fToggled, registerLength-1 /* MSB */); 

	if (r) {
		pb_->val(auxArr_[registerLength - ALU_SHL_Gadget::idxs::invRem]) = remainder.inverse();
	   pb_->val(auxArr_[registerLength - ALU_SHL_Gadget::idxs::dRes]) = Algebra::zero();
	}
	else {
		pb_->val(auxArr_[registerLength - ALU_SHL_Gadget::idxs::invRem]) = Algebra::zero();
		doubleRes = ((long)operand2 >= registerLength ? 0 : (doubleRes << operand2));
		pb_->val(auxArr_[registerLength - ALU_SHL_Gadget::idxs::dRes]) = Algebra::mapIntegerToFieldElement(0, EXTDIM, doubleRes);
	}
	unpackDouble_g_->generateWitness();
	packResult_g_->generateWitness();

}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SHR_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_SHR_Gadget::ALU_SHR_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
	:Gadget(pb), ALU_Component_Gadget(pb, inputs, results), auxArr_(opcodeAux1_){}

GadgetPtr ALU_SHR_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_SHR_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_SHR_Gadget::init(){
	size_t registerLength = tinyRAMparams()->registerLength();
	unpackDouble_g_ = DoubleUnpack_Gadget::create(pb_, unpackedArg1_, unpackedArg2_,
		auxArr_[registerLength - ALU_SHR_Gadget::idxs::dRes], Opcode::SHR);
	packResult_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg2_, results_.result_, PackingMode::PACK, Opcode::SHR);
}

void ALU_SHR_Gadget::generateConstraints(){
	const long registerLength = long((tinyRAMparams()->registerLength()));
	const long registerLogLen = long(log2(registerLength));
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	const Algebra::FElem invx = x.inverse();
	Algebra::CircuitPolynomial tmpPoly, witnessPoly;

	const Variable remainder = auxArr_[registerLength-ALU_SHR_Gadget::idxs::rem];
	Algebra::FElem x_i = Algebra::one();
	for (int i = 0; i < registerLogLen; ++i){
		const Variable v = auxArr_[i];
		const Variable w = auxArr_[i+registerLogLen];
		tmpPoly = tmpPoly + x_i * v;
		x_i *= x;
		enforceBooleanity(v, Opcode::SHR);
		witnessPoly = Algebra::invExtrConsts[i] * (w*w + w) + remainder;
		pb_->addGeneralConstraint(witnessPoly, "traceWitness", Opcode::SHR);
	}
	tmpPoly = tmpPoly + remainder;
	pb_->addGeneralConstraint(tmpPoly + inputs_.arg2_val_, "[A] = tmpPoly", Opcode::SHR);

	const Variable wFlag = auxArr_[registerLength-ALU_SHR_Gadget::idxs::witnessFlag];
	witnessPoly = Algebra::invExtrConsts[0] * (wFlag*wFlag + wFlag) + inputs_.arg1_val_ + results_.flag_;
	pb_->addGeneralConstraint(witnessPoly, "flagWitness", Opcode::SHR);
	enforceBooleanity(results_.flag_, Opcode::SHR);

	tmpPoly = Algebra::power(x, registerLength) * inputs_.arg1_val_;
	x_i = invx;
	for (int i = 0; i < registerLogLen; ++i){
		const Variable v = auxArr_[i];
		tmpPoly = tmpPoly * (Algebra::one() + v + v * x_i);
		x_i *= x_i;
	}
	const Variable invRem = auxArr_[registerLength-ALU_SHR_Gadget::idxs::invRem];
	tmpPoly = tmpPoly * (remainder * invRem + Algebra::one());
	const Variable doubleResult = auxArr_[registerLength-ALU_SHR_Gadget::idxs::dRes];
	pb_->addGeneralConstraint(tmpPoly + doubleResult, "doubleResult = tmpPoly", Opcode::SHR);

	tmpPoly = remainder * (remainder * invRem + Algebra::one());
	pb_->addGeneralConstraint(tmpPoly, "rem*(rem * invRem + 1)", Opcode::SHR);
	
	if (standAlone_)
	 unpackDouble_g_->generateConstraints(); 
	packResult_g_->generateConstraints();
	// add isMemOp = 0
	pb_->addGeneralConstraint(results_.isMemOp_, "isMemOp = 0", Opcode::SHR);
}

void ALU_SHR_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	const long registerLength = long((tinyRAMparams()->registerLength()));
	const long registerLogLen = long(log2(registerLength));
	
	size_t operand2 = Algebra::mapFieldElementToInteger(0, EXTDIM, pb_->val(inputs_.arg2_val_));
	size_t r = operand2;
	for (int i = 0; i < registerLogLen; ++i){
		const Variable v = auxArr_[i];
		size_t mask = size_t(1) << i;
		pb_->val(v) = (r & mask) ? Algebra::one() : Algebra::zero();
		r = r & (~mask);
	}
	const Algebra::FElem remainder = Algebra::mapIntegerToFieldElement(0, EXTDIM, r);
	pb_->val(auxArr_[registerLength - ALU_SHR_Gadget::idxs::rem]) = remainder;

	for (int i = 0; i < registerLogLen; ++i){
		const Variable w = auxArr_[i+registerLogLen];
		pb_->val(w) = extractBit(remainder, i);
	}

	const FElem operand1 = pb_->val(inputs_.arg1_val_);
	size_t doubleRes = Algebra::mapFieldElementToInteger(0, EXTDIM, operand1);
	
	const FElem fRes = Algebra::mapIntegerToFieldElement(0, 1, doubleRes);
	pb_->val(results_.flag_) = fRes;
	pb_->val(auxArr_[registerLength - ALU_SHR_Gadget::idxs::witnessFlag]) =
		extractBit(operand1+fRes, 0 /* LSB */); 

	if (r) {
		pb_->val(auxArr_[registerLength - ALU_SHR_Gadget::idxs::invRem]) = remainder.inverse();
	   pb_->val(auxArr_[registerLength - ALU_SHR_Gadget::idxs::dRes]) = Algebra::zero();
	}
	else {
		pb_->val(auxArr_[registerLength - ALU_SHR_Gadget::idxs::invRem]) = Algebra::zero();
		doubleRes = ((long)operand2 >= registerLength ? 0 : (doubleRes << (registerLength - operand2)));
		pb_->val(auxArr_[registerLength - ALU_SHR_Gadget::idxs::dRes]) = Algebra::mapIntegerToFieldElement(0, EXTDIM, doubleRes);
	}

	 unpackDouble_g_->generateWitness();
	packResult_g_->generateWitness();

}


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_JMP_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_JMP_Gadget::ALU_JMP_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
									: Gadget(pb), ALU_Component_Gadget(pb, inputs, results) {}

GadgetPtr ALU_JMP_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_JMP_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_JMP_Gadget::init(){}

void ALU_JMP_Gadget::generateConstraints(){
	pb_->addGeneralConstraint(results_.flag_ + inputs_.flag_, "inputs.flag_ = results.flag_", Opcode::JMP);
	pb_->addGeneralConstraint(results_.result_ + inputs_.arg2_val_, "inputs.arg2_val = result.result", Opcode::JMP);
}

void ALU_JMP_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	pb_->val(results_.flag_) = pb_->val(inputs_.flag_);
	pb_->val(results_.result_) = pb_->val(inputs_.arg2_val_);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CJMP_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_CJMP_Gadget::ALU_CJMP_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
								: Gadget(pb), ALU_Component_Gadget(pb, inputs, results) {}

GadgetPtr ALU_CJMP_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_CJMP_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_CJMP_Gadget::init(){}

void ALU_CJMP_Gadget::generateConstraints(){
	pb_->addGeneralConstraint(results_.flag_ + inputs_.flag_, "inputs.flag_ = results.flag_", Opcode::CJMP);
	pb_->addGeneralConstraint(results_.result_ + inputs_.arg2_val_, "inputs.arg2_val = result.result", Opcode::CJMP);
}

void ALU_CJMP_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	pb_->val(results_.flag_) = pb_->val(inputs_.flag_);
	pb_->val(results_.result_) = pb_->val(inputs_.arg2_val_);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************						ALU_CNJMP_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_CNJMP_Gadget::ALU_CNJMP_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
									: Gadget(pb), ALU_Component_Gadget(pb, inputs, results) {}

GadgetPtr ALU_CNJMP_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_CNJMP_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_CNJMP_Gadget::init(){}

void ALU_CNJMP_Gadget::generateConstraints(){
	pb_->addGeneralConstraint(results_.flag_ + inputs_.flag_, "inputs.flag_ = results.flag_", Opcode::CNJMP);
	pb_->addGeneralConstraint(results_.result_ + inputs_.arg2_val_, "inputs.arg2_val = result.result", Opcode::CNJMP);
}

void ALU_CNJMP_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	pb_->val(results_.flag_) = pb_->val(inputs_.flag_);
	pb_->val(results_.result_) = pb_->val(inputs_.arg2_val_);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_STOREW_Gadget                  ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
ALU_STOREW_Gadget::ALU_STOREW_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
									:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_STOREW_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_STOREW_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_STOREW_Gadget::init(){}

void ALU_STOREW_Gadget::generateConstraints(){
	pb_->addGeneralConstraint(results_.isMemOp_ + Algebra::one(), "isMemOp = 1", Opcode::STOREW);
	pb_->addGeneralConstraint(results_.isLoadOp_, "isLoadOp = 0", Opcode::STOREW);
	pb_->addGeneralConstraint(inputs_.flag_ + results_.flag_, "inputs_.flag = results.flag_", Opcode::STOREW);
	pb_->addGeneralConstraint(results_.value_ + inputs_.dest_val_, "inputs_.value_ = inputs_.dest_val", Opcode::STOREW);
}

void ALU_STOREW_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	pb_->val(results_.isMemOp_) = Algebra::one();
	pb_->val(results_.isLoadOp_) = Algebra::zero();
	FElem memoryAddress = pb_->val(inputs_.arg2_val_);
	FElem value = pb_->val(inputs_.dest_val_);
	// Stores arg2 To dest_val
	pb_->storeValue(memoryAddress, value);
	pb_->val(results_.value_) = value;
	pb_->val(results_.address_) = memoryAddress;
	pb_->val(results_.flag_) = pb_->val(inputs_.flag_);
}

/*********************************/
/***       END OF Gadget       ***/
/*********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_LOADW_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
ALU_LOADW_Gadget::ALU_LOADW_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_LOADW_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_LOADW_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_LOADW_Gadget::init(){}

void ALU_LOADW_Gadget::generateConstraints(){
	pb_->addGeneralConstraint(results_.isMemOp_ + Algebra::one(), "isMemOp = 1", Opcode::LOADW);
	pb_->addGeneralConstraint(results_.isLoadOp_ + Algebra::one(), "isLoadOp = 1", Opcode::LOADW);
	pb_->addGeneralConstraint(inputs_.flag_ + results_.flag_, "inputs_.flag = results.flag_", Opcode::LOADW);
}

void ALU_LOADW_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	pb_->val(results_.isLoadOp_) = Algebra::one();
	pb_->val(results_.isMemOp_) = Algebra::one();
	FElem address = pb_->val(inputs_.arg2_val_); // stores [A] to r_1 - check traceConsistency
	FElem value = pb_->loadValue(address);
	pb_->val(results_.address_) = address;
	pb_->val(results_.value_) = value;
	pb_->val(results_.flag_) = pb_->val(inputs_.flag_);
}


/*********************************/
/***       END OF Gadget       ***/
/*********************************/


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_ANSWER_Gadget                  ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_ANSWER_Gadget::ALU_ANSWER_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
									: Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_ANSWER_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_ANSWER_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}
void ALU_ANSWER_Gadget::init(){}
void ALU_ANSWER_Gadget::generateConstraints(){
	if (max_timestep > 1) {
		pb_->addBoundaryConstraint(inputs_.arg2_val_, max_timestep, program_output);
	}
}
void ALU_ANSWER_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	static bool flag = true;
	if (flag) {
		flag = false;
		if (Algebra::one() == program_output)
			program_output = pb_->val(inputs_.arg2_val_);
		/*
         * size_t a = mapFieldElementToInteger(0, EXTDIM, pb_->val(inputs_.arg2_val_));
         * std::cout << "\n*** TIMESTEPS=" << max_timestep << " ANSWER=" << a << " (binary " << std::bitset<REGISTER_LENGTH>(a) << ")\n" << std::endl;
         */
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_MOV_Gadget						******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_MOV_Gadget::ALU_MOV_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
											: Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_MOV_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_MOV_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_MOV_Gadget::init(){}

void ALU_MOV_Gadget::generateConstraints(){
	pb_->addGeneralConstraint(inputs_.flag_ + results_.flag_, "inputs_.flag = results.flag_", Opcode::MOV);
	pb_->addGeneralConstraint(inputs_.arg2_val_ + results_.result_, "results.result = inputs.arg2_val", Opcode::MOV);
}
void ALU_MOV_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	pb_->val(results_.flag_) = pb_->val(inputs_.flag_);
	pb_->val(results_.result_) = pb_->val(inputs_.arg2_val_);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************          ALU_RESERVED_OPCODE_24_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALU_RESERVED_OPCODE_24_Gadget::ALU_RESERVED_OPCODE_24_Gadget(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results)
	:Gadget(pb), ALU_Component_Gadget(pb, inputs, results){}

GadgetPtr ALU_RESERVED_OPCODE_24_Gadget::create(ProtoboardPtr pb, const ALUInput& inputs, const ALUOutput& results){
	GadgetPtr pGadget(new ALU_RESERVED_OPCODE_24_Gadget(pb, inputs, results));
	pGadget->init();
	return pGadget;
}

void ALU_RESERVED_OPCODE_24_Gadget::init(){
	unpackArg1_g_ = CompressionPacking_Gadget::create(pb_, unpackedArg1_, inputs_.arg2_val_, PackingMode::UNPACK, Opcode::RESERVED_OPCODE_24);
}

void ALU_RESERVED_OPCODE_24_Gadget::generateConstraints(){
	if (standAlone_) {
		unpackArg1_g_->generateConstraints();
	}
	srand(prngseed); 
	rand();
	CircuitPolynomial t, rPoly = Algebra::zero();
	for (unsigned int i = 0; i < ROMSIZE; ++i){
		t = Algebra::one();
		for (unsigned int k=0,e=1; e<ROMSIZE; ++k,e*=2)
			if (1 & (i >> k))
				t = t * unpackedArg1_[k];
			else
				t = t * (unpackedArg1_[k] + Algebra::one());
		rPoly = rPoly + t * (results_.result_ + mapIntegerToFieldElement(0, REGISTER_LENGTH, rand()-RAND_MAX/2));
	}
	pb_->addGeneralConstraint(rPoly, "ROM", Opcode::RESERVED_OPCODE_24);
}

void ALU_RESERVED_OPCODE_24_Gadget::generateWitness(){
	initGeneralOpcodes(pb_);
	initMemResult(pb_, results_);
	unpackArg1_g_->generateWitness();
	srand(prngseed); rand();
	size_t n = mapFieldElementToInteger(0, EXTDIM, val(inputs_.arg2_val_));
	if (n < ROMSIZE)
		for (unsigned int i = 0; i < n; ++i)
			(void)rand(); //lame, replace with md5 or sha
	unsigned int res = rand() - RAND_MAX/2;
	val(results_.result_) = mapIntegerToFieldElement(0, REGISTER_LENGTH, res);
}



