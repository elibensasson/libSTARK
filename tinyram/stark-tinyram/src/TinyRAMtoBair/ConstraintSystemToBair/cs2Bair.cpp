#include "cs2Bair.hpp"
#include "../RamToContraintSystem/MemoryConsraints.hpp"

using libstark::BairInstance;
using libstark::BairWitness;
using libstark::ConstraintSys;


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2Bair				                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

cs2Bair::cs2Bair(ProtoboardPtr pb,const TinyRAMProgram& program, const int transcriptLen, bool constructWitness) :
    pb_(pb),
	program_(program),
    transcript_len(transcriptLen),
	doesProgramUsesMemory_(false),
	followingTraceVariable_(program.pcLength(), std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->numRegisters()),
	memoryFollowingTraceVariables_(followingTraceVariable_.first_.timeStamp_, followingTraceVariable_.second_.timeStamp_)
    {
		this->init();
		this->generateConstraints();
		this->boundaryConstraints();
        if(constructWitness){
            generateWitness();
            generateMemoryWitness();
        }
}

void cs2Bair::init() {
	// Init the First Values of timestamp
	pb_->addDegreeTranslation(Algebra::FieldElement(getGF2E_X()), 1);
	pb_->addDegreeTranslation(Algebra::one(), 0);
	// GadgetPtr
	transitionFunction_ = TransitionFunction::create(pb_, followingTraceVariable_,memoryFollowingTraceVariables_ , program_);
	checkMemoryUse();
	if (doesProgramUsesMemory_){
		memoryConstraints_ = MemoryConstraints::create(pb_, memoryFollowingTraceVariables_);
	}
}

void cs2Bair::boundaryConstraints() const{
	::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
	pb_->addBoundaryConstraint(followingTraceVariable_.first_.flag_, 0, Algebra::zero());
	pb_->addBoundaryConstraint(followingTraceVariable_.first_.timeStamp_, 0, Algebra::one());
	for (unsigned int i = 0; i < program_.pcLength(); i++){
		pb_->addBoundaryConstraint(followingTraceVariable_.first_.pc_[i], 0, Algebra::zero()); 
	}
	//for (int i = 0; i < params->numRegisters(); i++){
	//	pb_->addBoundaryConstraint(followingTraceVariable_.first_.registers_[i], 0, Algebra::zero());
	//}
}

void cs2Bair::initInitialVars(){
	pb_->val(followingTraceVariable_.first_.flag_) = Algebra::zero();
	pb_->val(followingTraceVariable_.first_.timeStamp_) = Algebra::one();
	int pcLength = program_.pcLength();
	int numRegisters = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->numRegisters();
	for (int i = 0; i < pcLength; i++){
		pb_->val(followingTraceVariable_.first_.pc_[i]) = Algebra::zero();
	}
	for (int i = 0; i < numRegisters; i++){
		pb_->val(followingTraceVariable_.first_.registers_[i]) = Algebra::zero();
	}
}

void cs2Bair::checkMemoryUse(){
	for (unsigned int i = 0; i < program_.code().size(); ++i){
		Opcode opcode = program_.code()[i].opcode_;
		if (opcode == Opcode::STOREB || opcode == Opcode::STOREW ||
			opcode == Opcode::LOADB || opcode == Opcode::LOADW){
			doesProgramUsesMemory_ = true;
			break;
		}
	}
}

std::vector<Variable> cs2Bair::variablesToVector(TraceVariables traceVariables){
	std::vector<Variable> v;
	v.emplace_back(traceVariables.flag_);
	v.emplace_back(traceVariables.timeStamp_);
	::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
	for (unsigned int i = 0; i < traceVariables.pc_.size(); i++){
		v.emplace_back(traceVariables.pc_[i]);
	}
	for (unsigned int i = 0; i < params->numRegisters(); i++){
		v.emplace_back(traceVariables.registers_[i]);
	}
	return v;
}

std::vector<Variable> cs2Bair::memoryVariablesToVector(MemoryTraceVariables traceVariables){
	std::vector<Variable> v;
	v.emplace_back(traceVariables.isMemOp_);
	v.emplace_back(traceVariables.isLoad_);
	v.emplace_back(traceVariables.timeStamp_);
	v.emplace_back(traceVariables.address_);
	v.emplace_back(traceVariables.value_);
	return v;
}

void cs2Bair::copyTraceOutputValuesToTraceInput(){
	pb_->val(followingTraceVariable_.first_.timeStamp_) = pb_->val(followingTraceVariable_.second_.timeStamp_);
	pb_->val(followingTraceVariable_.first_.flag_) = pb_->val(followingTraceVariable_.second_.flag_);
	::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
	int pcLength = program_.pcLength();
	for (int i = 0; i < pcLength; i++){
		pb_->val(followingTraceVariable_.first_.pc_[i]) = pb_->val(followingTraceVariable_.second_.pc_[i]);
	}
	for (unsigned int i = 0; i < params->numRegisters(); i++){
		pb_->val(followingTraceVariable_.first_.registers_[i]) = pb_->val(followingTraceVariable_.second_.registers_[i]);
	}
}


void cs2Bair::createTranslationVector(){
	Algebra::Variable::set traceFirstVariables;
	Algebra::Variable::set traceSecondVariables;
	
	std::vector<Variable> trace1 = variablesToVector(followingTraceVariable_.first_);
	std::vector<Variable> trace2 = variablesToVector(followingTraceVariable_.second_);
	std::vector<Variable> memoryTrace1 = memoryVariablesToVector(memoryFollowingTraceVariables_.first_);
	std::vector<Variable> memoryTrace2 = memoryVariablesToVector(memoryFollowingTraceVariables_.second_);
	
	traceFirstVariables.insert(trace1.begin(), trace1.end());
	traceFirstVariables.insert(memoryTrace1.begin(), memoryTrace1.end());
	traceSecondVariables.insert(trace2.begin(), trace2.end());
	traceSecondVariables.insert(memoryTrace2.begin(), memoryTrace2.end());

	Algebra::Variable::set auxVars = pb_->constraintSystem(Opcode::NONE).getUsedVariables();
	Algebra::Variable::set memoryauxVars = pb_->constraintSystem(Opcode::MEMORY).getUsedVariables();
	auxVars.insert(memoryauxVars.begin(), memoryauxVars.end());
	for (const auto& var : trace1) {
		auxVars.erase(var);

	}
	for (const auto& var : trace2) {
		auxVars.erase(var);
	}
	for (const auto& var : memoryTrace1) {
		auxVars.erase(var);
	}
	for (const auto& var : memoryTrace2) {
		auxVars.erase(var);
	}
 	translation_.insert(translation_.end(), traceFirstVariables.cbegin(), traceFirstVariables.cend());
	translation_.insert(translation_.end(), auxVars.cbegin(), auxVars.cend());
	translation_.insert(translation_.end(), traceSecondVariables.cbegin(), traceSecondVariables.cend());
	Algebra::Variable unUsed("Unused aux Variable");
	for (size_t i = 0; i < auxVars.size(); i++){
		translation_.emplace_back(unUsed);
	}
	// Jenya - pretty ugly - we leave it like this for now - we'll change it as soon as I have time.
	pb_->setNewIndicesForTranslation(translation_);
	translation_ = pb_->getTranslationVector();
}

void cs2Bair::generateConstraints(){
	transitionFunction_->generateConstraints();
	if (doesProgramUsesMemory_){
		memoryConstraints_->generateConstraints();
	}
	createTranslationVector();
}

//VariableAssignment <-> vector<FieldElement> mappings
std::vector<Algebra::FieldElement> cs2Bair::assignmentToVec(const VariableAssignment& ass)const{
    std::vector<Algebra::FieldElement> res(translation_.size()/2);
    for(size_t i=0; i< res.size(); i++){
        res[i] = ass.at(translation_[i]);
    }
    return res;
}

VariableAssignment cs2Bair::vectorToAssignment(const std::vector<Algebra::FieldElement>& vec)const{
    VariableAssignment res;
    for(size_t i=0; i< translation_.size()/2; i++){
        res[translation_[i]] = vec[i];
    }
    return res;
}

//#define falseWitness//checking how good PCP is at finding small errors
//#define printTrace //print the execution trace (i.e. witness) while generating it
void cs2Bair::generateWitness(){
	//const unsigned int transcript_len = POW2(TRANSCIPT_LEN_LOG) - 1;
	// First Assignment should be zero
    initInitialVars();
#ifdef printTrace
	set<Algebra::Variable, Variable::VariableStrictOrder> usedVars;
#endif
    for (int i = 0; i < transcript_len; ++i){
		::std::dynamic_pointer_cast<TransitionFunction>(transitionFunction_)->generateWitness(i);
        Algebra::VariableAssignment assignment = pb_->assignment();
        traceAssignmentTable_.push_back(assignmentToVec(assignment));
#ifdef falseWitness
		if (i == 3)
			traceAssignmentTable_[i][4] = Algebra::xFE();
#endif
		copyTraceOutputValuesToTraceInput();
#ifdef printTrace
		//cout << "var num:" << assignment.size() << endl;
		//cout << i << ":" << endl;
		for (auto j : assignment){
			if (j.second != Algebra::zero()){
				usedVars.insert(j.first);
			//	cout << "var num";
				
			}
		//	cout << j.first.name() << ":" << j.second << " ";
		}
	/*	cout << "var num:" << pb_->getUsedVariables().size() << endl;

		for (auto j : pb_->getUsedVariables()){
			cout << j.name() << ":" << pb_->assignment()[j] << endl;
		}*/
		//cout << endl;
#endif
	}
#ifdef printTrace

	cout << "non-zero-constant vars in trace:" << endl;
	for (auto j : usedVars)
		cout << j.name() << " " <<endl;
	cout << "total:" << usedVars.size();
#endif

}

ConstraintSystem cs2Bair::getMemoryConstraints() const{
	if (doesProgramUsesMemory_){
		return pb_->constraintSystem(Opcode::MEMORY); 
	}
	return ConstraintSystem();
}

void cs2Bair::generateMemoryWitness(){

    Variable::set usedMemoryVariables = pb_->constraintSystem(Opcode::MEMORY).getUsedVariables();
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.first_.timeStamp_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.first_.isMemOp_); 
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.first_.isLoad_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.first_.value_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.first_.address_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.second_.timeStamp_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.second_.isMemOp_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.second_.isLoad_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.second_.value_);
	usedMemoryVariables.erase(memoryFollowingTraceVariables_.second_.address_);

	std::vector<MemoryInfo> memoryTrace = pb_->getMemoryTrace();
	std::sort(memoryTrace.begin(), memoryTrace.end(), sortMemoryInfo);
	GADGETLIB_ASSERT(memoryTrace.size() == traceAssignmentTable_.size(), "memoryInfo size should be the same as the coloring");
	for (unsigned int i = 0; i < memoryTrace.size(); i++){
		size_t serialNumber1 = memoryTrace[i].getSerialNumber();
		size_t serialNumber2 = memoryTrace[(i + 1) % memoryTrace.size()].getSerialNumber();
		memoryPermutation_[serialNumber1] = serialNumber2;
		if (doesProgramUsesMemory_){
			VariableAssignment currAssignment = vectorToAssignment(traceAssignmentTable_[serialNumber1]);
			VariableAssignment nextAssignment = vectorToAssignment(traceAssignmentTable_[serialNumber2]);
			pb_->clearAssignment();
			pb_->val(memoryFollowingTraceVariables_.first_.timeStamp_) = currAssignment[memoryFollowingTraceVariables_.first_.timeStamp_];
			pb_->val(memoryFollowingTraceVariables_.first_.isMemOp_) = currAssignment[memoryFollowingTraceVariables_.first_.isMemOp_];
			pb_->val(memoryFollowingTraceVariables_.first_.isLoad_) = currAssignment[memoryFollowingTraceVariables_.first_.isLoad_];
			pb_->val(memoryFollowingTraceVariables_.first_.value_) = currAssignment[memoryFollowingTraceVariables_.first_.value_];
			pb_->val(memoryFollowingTraceVariables_.first_.address_) = memoryTrace[i].getAddress();

			pb_->val(memoryFollowingTraceVariables_.second_.timeStamp_) = nextAssignment[memoryFollowingTraceVariables_.first_.timeStamp_];
			pb_->val(memoryFollowingTraceVariables_.second_.isMemOp_) = nextAssignment[memoryFollowingTraceVariables_.first_.isMemOp_];
			pb_->val(memoryFollowingTraceVariables_.second_.isLoad_) = nextAssignment[memoryFollowingTraceVariables_.first_.isLoad_];
			pb_->val(memoryFollowingTraceVariables_.second_.value_) = nextAssignment[memoryFollowingTraceVariables_.first_.value_];
			pb_->val(memoryFollowingTraceVariables_.second_.address_) = memoryTrace[(i+1) % memoryTrace.size()].getAddress();
			if (Algebra::one() == pb_->val(memoryFollowingTraceVariables_.first_.isMemOp_)){

				memoryConstraints_->generateWitness();

				if (i+1 != memoryTrace.size()){
					GADGETLIB_ASSERT(pb_->isSatisfied(Opcode::MEMORY), "MemoryConstraints are not satisfied");
					for (const Variable& v : usedMemoryVariables){
						currAssignment[v] = pb_->val(v);
					}
				}
				else{
					for (const Variable& v : usedMemoryVariables){
						currAssignment[v] = Algebra::zero();
					}
				}
				traceAssignmentTable_[serialNumber1] = assignmentToVec(currAssignment);
			}
		}
	}
}


size_t cs2Bair::varsAmount() const{
	std::vector<Algebra::Variable> translation = pb_->getTranslationVector();
	GADGETLIB_ASSERT(translation.size() % 2 == 0, "translation vector size is expected to be even.");
	/*
	return translation_.size() / 2;
	*/
	// Michael said that he wants the size of all the variables.
	return translation.size();
}


BairInstance::boundaryConstraints_t cs2Bair::getBoundaryConstraints() const{
	BairInstance::boundaryConstraints_t boundaryConstraints;

	BoundaryVariables boundaryVariables = pb_->boundaryVariables();
	BoundaryTimestamps boundaryTimestamps = pb_->boundaryTimestamps();
	BoundaryAssignments boundaryAssignment = pb_->boundaryAssignments();

	_COMMON_ASSERT(boundaryVariables.size() == boundaryTimestamps.size(),
		"Number of variables should be equal to the number of timestamps in boundary constaints");
	_COMMON_ASSERT(boundaryAssignment.size() == boundaryVariables.size(),
		"Number of variabled should be equal to the number os assignments in boundary constraints");

	for (unsigned int i = 0; i < boundaryVariables.size(); ++i){
		for (unsigned int j = 0; j < translation_.size(); ++j){
			if (translation_[j] == boundaryVariables[i]){
				BairInstance::point_t location(boundaryTimestamps[i], j);
				boundaryConstraints[location] = boundaryAssignment[i];
			}
		}
	}
	return boundaryConstraints;
}

Algebra::Variable::set cs2Bair::getStateVariables() const {
	Algebra::Variable::set retSet;
	retSet.insert(followingTraceVariable_.first_.flag_);
	retSet.insert(followingTraceVariable_.second_.flag_);
	retSet.insert(followingTraceVariable_.first_.timeStamp_);
	retSet.insert(followingTraceVariable_.second_.timeStamp_);
	GADGETLIB_ASSERT(followingTraceVariable_.first_.pc_.size() == followingTraceVariable_.second_.pc_.size(), 
							"CS2Bair: unpacked pc should have the exact same size in both of the states");
	for (unsigned int i = 0; i < followingTraceVariable_.first_.pc_.size(); i++) {
		retSet.insert(followingTraceVariable_.first_.pc_[i]);
		retSet.insert(followingTraceVariable_.second_.pc_[i]);
	}
	GADGETLIB_ASSERT(followingTraceVariable_.first_.registers_.size() == followingTraceVariable_.second_.registers_.size(),
									"CS2Bair: number of registers should be the same in both of the states");
	for (unsigned int i = 0; i < followingTraceVariable_.first_.registers_.size(); i++) {
		retSet.insert(followingTraceVariable_.first_.registers_[i]);
		retSet.insert(followingTraceVariable_.second_.registers_[i]);
	}
	return retSet;
}


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairConstraints	                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

cs2BairConstraints::cs2BairConstraints(const cs2Bair& cs2bair) : numVars_(cs2bair.varsAmount()){
	ConstraintSystem cs = cs2bair.getConstraints();
    for(const auto& p: cs.getConstraintPolynomials()){
        constraints_.push_back(p->clone());
    }
};

cs2BairConstraints::cs2BairConstraints(const polySet_t& constraints, const size_t numVars):numVars_(numVars){
    for(const auto& p: constraints){
        constraints_.push_back(p->clone());
    }
}

size_t cs2BairConstraints::numVars() const{
	return numVars_;;
}
const ConstraintSys::polySet_t& cs2BairConstraints::constraints() const{
	return constraints_;
}

cs2BairConstraints* cs2BairConstraints::clone() const{
    return new cs2BairConstraints(constraints_, numVars_);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairColoring		                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

cs2BairColoring::cs2BairColoring(const cs2Bair& cs2bair) : traceAssignments_(cs2bair.getTraceAssignmet()){
	translation_ = cs2bair.getTranslationVector();
};

BairWitness::color_t cs2BairColoring::getElementByIndex(index_t index) const{
	GADGETLIB_ASSERT(index < traceAssignments_.size(), "Attempted to get an illegal element; " +std::to_string(index) + " >= " + std::to_string(traceAssignments_.size()));
    return traceAssignments_[index];
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairMemory			                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

cs2BairMemory::cs2BairMemory(const cs2Bair& cs2bair) : memoryPermutation_(cs2bair.getMemoryPermutation()){}

size_t cs2BairMemory::getElementByIndex(index_t index) const{
	return memoryPermutation_.at(index);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairCS				                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

cs2BairMemoryCS::cs2BairMemoryCS(const cs2Bair& cs2bair) : numVars_(cs2bair.varsAmount()){
	ConstraintSystem cs = cs2bair.getMemoryConstraints();
	constraints_ = cs.getConstraintPolynomials();
};

cs2BairMemoryCS::cs2BairMemoryCS(const polySet_t& constraints, const size_t numVars):numVars_(numVars){
    for(const auto& p: constraints){
        constraints_.push_back(p->clone());
    }
}

size_t cs2BairMemoryCS::numVars() const{
	return numVars_;
}

const ConstraintSys::polySet_t& cs2BairMemoryCS::constraints() const {
	return constraints_;
}

cs2BairMemoryCS* cs2BairMemoryCS::clone() const{
    return new cs2BairMemoryCS(constraints_,numVars_);
}



