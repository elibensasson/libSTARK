#ifndef _COMMON_CONSTRAINTSLIB2_CONSTRAINTSYSTEMTOBair_cs2Bair_HPP_
#define _COMMON_CONSTRAINTSLIB2_CONSTRAINTSYSTEMTOBair_cs2Bair_HPP_

#include "gadgetlib/common_use.hpp"
#include "TinyRAMtoBair/RamToContraintSystem/transitionFunction.hpp"
#include "TinyRAMtoBair/RamToContraintSystem/transitionFunction.hpp"
#include <languages/Bair/ConstraintsSys.hpp>
#include <languages/Bair/BairWitness.hpp>
#include <languages/Bair/BairInstance.hpp>





/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2Bair				                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class cs2Bair{
private:
	// Input Variables
	ProtoboardPtr pb_;
	TinyRAMProgram program_;

	// Internal Variables
	int transcript_len;
	// Is there any Memory Use in the program
	bool doesProgramUsesMemory_;
	// Memory and Regular constraints
	GadgetPtr transitionFunction_;
	GadgetPtr memoryConstraints_;
	// Trace Variables for regular and memory constraints.
	FollowingTraceVariables followingTraceVariable_;
	MemoryFollowingTraceVariables memoryFollowingTraceVariables_;
	// Translation vector between CS variables to Bair
	std::vector<Algebra::Variable> translation_;
    
	std::vector<std::vector<Algebra::FieldElement>> traceAssignmentTable_;
    std::map<size_t,size_t> memoryPermutation_;

	// Help Functions
	void initInitialVars();
	void checkMemoryUse();
	void copyTraceOutputValuesToTraceInput();
	void createTranslationVector();
	std::vector<Variable> variablesToVector(TraceVariables traceVariables);
	std::vector<Variable> memoryVariablesToVector(MemoryTraceVariables traceVariables);
	void generateMemoryConstraints() const;
	void boundaryConstraints() const;
	Algebra::Variable::set getStateVariables() const;

	//Functions
	void init();
	void generateConstraints();
	void generateWitness();
	void generateMemoryWitness();

    //VariableAssignment <-> vector<FieldElement> mappings
    std::vector<Algebra::FieldElement> assignmentToVec(const VariableAssignment& ass)const;
    VariableAssignment vectorToAssignment(const std::vector<Algebra::FieldElement>& vec)const;

public:
	cs2Bair(ProtoboardPtr pb, const TinyRAMProgram& program, int transcriptLen, bool constructWitness);

	ConstraintSystem getConstraints() const { return pb_->constraintSystem(Opcode::NONE); }
	ConstraintSystem getMemoryConstraints() const;

	const std::vector<std::vector<Algebra::FieldElement>>& getTraceAssignmet()const { return traceAssignmentTable_; }
	std::vector<Variable> getTranslationVector() const { return pb_->getTranslationVector(); }
	const std::map<size_t, size_t>& getMemoryPermutation()const { return memoryPermutation_; }
	size_t varsAmount() const; // Number of Variables in one traceLine
    libstark::BairInstance::boundaryConstraints_t getBoundaryConstraints() const; 
}; // cs2Bair


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairConstraints	                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class cs2BairConstraints :public libstark::ConstraintSys {
private:
	polySet_t constraints_;
    const size_t numVars_;
public:
	cs2BairConstraints(const cs2Bair& cs2bair);
	cs2BairConstraints(const polySet_t& constraints, const size_t numVars);
	size_t numVars() const; // amount of variables in 1 traceline (including aux variables)
	const polySet_t& constraints() const;
    cs2BairConstraints* clone() const;
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairColoring		                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class cs2BairColoring : public libstark::BairWitness::assignment_t{
private:
	const std::vector<std::vector<FieldElement>> traceAssignments_;
	std::vector<Variable> translation_;
public:
	cs2BairColoring(const cs2Bair& cs2bair);
    libstark::BairWitness::color_t getElementByIndex(index_t index) const;
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairMemory			                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

// For now - stub. We don't use memory permutation
class cs2BairMemory : public libstark::Sequence<size_t>{
private:
	const std::map<size_t, size_t> memoryPermutation_;
public:
	cs2BairMemory(const cs2Bair& cs2bair);
	size_t getElementByIndex(index_t index) const;

};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      cs2BairCS				                ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

//ForNow we use empty constraint system
class cs2BairMemoryCS : public libstark::ConstraintSys {
private:
	polySet_t constraints_;
	const size_t numVars_;
	
public:
	cs2BairMemoryCS(const cs2Bair& cs2bair);
	cs2BairMemoryCS(const polySet_t& constraints, const size_t numVars);
	size_t numVars() const;
	const polySet_t& constraints() const;
    cs2BairMemoryCS* clone()const;
};



#endif // _COMMON_CONSTRAINTSLIB2_CONSTRAINTSYSTEMTOBair_cs2Bair_HPP_
