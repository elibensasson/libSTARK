/** @file
 *****************************************************************************
 Definition of Protoboard, a "memory manager" for building arithmetic constraints
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef GADGETLIB3_PROTOBOARD_HPP_
#define GADGETLIB3_PROTOBOARD_HPP_

#include <string>
#include <tuple>
#include <memory>
#include <map>
#include <algebraLib/variable.hpp>
#include <gadgetlib/common_use.hpp>
#include <gadgetlib/constraint.hpp>
#include <gadgetlib/infrastructure.hpp>
#include <algebraLib/CircuitPolynomial.hpp>

#define ASSERT_CONSTRAINTS_SATISFIED(pb) \
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED))

#define ASSERT_CONSTRAINTS_NOT_SATISFIED(pb) \
    ASSERT_FALSE(pb->isSatisfied(PrintOptions::NO_DBG_PRINT))

namespace gadgetlib {

class ProtoboardParams; // Forward declaration
class Protoboard; //Forward declaration

typedef Algebra::CircuitPolynomial Polynomial;
typedef ::std::shared_ptr<Protoboard> ProtoboardPtr;
typedef ::std::shared_ptr<const ProtoboardParams> ProtoboardParamsCPtr;
typedef ::std::vector<Algebra::Variable> BoundaryVariables;
typedef ::std::vector<Algebra::FieldElement> BoundaryAssignments;
typedef ::std::vector<size_t> BoundaryTimestamps;

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                       class Protoboard                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class Protoboard{
private:
	ConstraintSystem memoryConstraintSystem_; // Memory Constratins.
	ConstraintSystem constraintSystem_; // General Constraints
	std::vector<ConstraintSystem> opcodeConstraintSystem_;  // Constraints for each opcode
	Algebra::VariableAssignment assignment_;
	size_t numInputs_;
	size_t numConstratins_;
    ProtoboardParamsCPtr pParams_; // TODO try to refactor this out 

	// We translate between my indices to Michaels
	std::map<Algebra::VarIndex_t, Algebra::VarIndex_t>  old2newID_;
	// When we transfer the variables to BREX we set them in a vector.
	std::vector<Algebra::Variable> translation_;
	// translation between g^i to i.
	std::map<Algebra::FElem, unsigned int, Algebra::classCompElements> degreeTranslation_;
	// Simulate the code
	std::vector<MemoryInfo> memoryTrace_;
	// Memory Implementaion - Mapping between an address and a value.
	// Notice that for now we assume that each 
	std::map<Algebra::FElem, Algebra::FElem, Algebra::classCompElements> memory_;
	// Boundary consistency
	BoundaryVariables boundaryVariables_;
	BoundaryAssignments boundaryAssignments_;
	BoundaryTimestamps boundaryTimestamps_; // In some use cases, the same constraints system will be used
											// many times while only one instance of verification should
											// be checked for boundary conditions. In these cases we
											// use boundaryTimestamps_ to specify which instance uses
											// the boundary condition




	Protoboard(ProtoboardParamsCPtr pParams) :
        opcodeConstraintSystem_(33,ConstraintSystem()),
        numInputs_(0),
        numConstratins_(0),
        pParams_(pParams) {};

	void addBoundaryVariable(const Algebra::Variable& var) { boundaryVariables_.emplace_back(var); }
	void addBoundaryTimestamp(const size_t timeStamp) { boundaryTimestamps_.emplace_back(timeStamp); }
	void addBoundaryAssignment(const Algebra::FElem& assignment) { boundaryAssignments_.emplace_back(assignment); }

public:
    static ProtoboardPtr create(ProtoboardParamsCPtr pParams = NULL) {
        return ProtoboardPtr(new Protoboard(pParams));
    }
	unsigned int numConstraints() const;
	Algebra::Variable::set getUsedVariables() const;
	ProtoboardParamsCPtr params() const { return pParams_; }
	Algebra::VariableAssignment assignment() const { return assignment_; }
	Algebra::FElem& val(const Algebra::Variable& var);
	const Algebra::FElem val(const Algebra::Variable& var) const;
	const Algebra::FElem val(const Algebra::LinearCombination& lc) const;
	void addGeneralConstraint(const Polynomial& a,
		                      const ::std::string& name,
							  Opcode opcode);
	void addMemoryConstraint(const Polynomial& a,
							const std::string& name);
	bool isSatisfied(Opcode opcode,const PrintOptions& printOnFail = PrintOptions::NO_DBG_PRINT);
	void enforceBooleanity(const Algebra::Variable& var,Opcode opcode);
	bool flagIsSet(const Algebra::FlagVariable& flag) const { return val(flag) == Algebra::one(); }
	ConstraintSystem constraintSystem(Opcode opcode) const;
	void multiplyConstraintSystem(Opcode opcode, const Algebra::CircuitPolynomial&  mul);
	void clearConstraintSystem(Opcode opcode);
	void setNewIndicesForTranslation(::std::vector<Algebra::Variable> translationVector);
	std::vector<Algebra::Variable> getTranslationVector() const { return translation_; }
	void addDegreeTranslation(const Algebra::FElem& elem, unsigned int degree);
	unsigned int getDegreeOfFElem(const Algebra::FElem& elem);
	void addMemoryInfo(const MemoryInfo& memoryInfo){ memoryTrace_.emplace_back(memoryInfo); }
	void storeValue(const Algebra::FElem& memory, const Algebra::FElem& value);
	Algebra::FElem loadValue(const Algebra::FElem& address);
	std::vector<MemoryInfo> getMemoryTrace() { return memoryTrace_; }
	void clearAssignment(){ assignment_.clear(); }
	BoundaryVariables boundaryVariables() const { return boundaryVariables_; }
	BoundaryAssignments boundaryAssignments() const { return boundaryAssignments_; }
	BoundaryTimestamps boundaryTimestamps() const { return boundaryTimestamps_; }
	void addBoundaryConstraint(const Algebra::Variable& var, const size_t timeStamp, const Algebra::FElem& assignment);
};
/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                     class ProtoboardParams                 ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
/*
    An abstract class to hold any additional information needed by a specific Protoboard. For
    example a Protoboard specific to TinyRAM will have a class ArchParams which will inherit from
    this class.
*/
class ProtoboardParams {
public:
    virtual ~ProtoboardParams() = 0;
};

///*************************************************************************************************/
///*************************************************************************************************/
///****************************                                         ****************************/
///****************************         class TinyRAMProtoboardParams   ****************************/
///****************************                                         ****************************/
///*************************************************************************************************/
///*************************************************************************************************/
//
//class TinyRAMProtoboardParams : public ProtoboardParams {
//private:
//	TinyRAMArchParams archParams_;
//	size_t opcodeWidth_;
//	size_t timeBound_;
//	size_t pcIncrement_;
//public:
//	TinyRAMProtoboardParams(unsigned int numRegisters, unsigned int registerLength,
//		size_t opcodeWidth, size_t timeBound, size_t pcIncrement)
//		: archParams_(TinyRAMArchParams{ numRegisters, registerLength }),
//		opcodeWidth_(opcodeWidth),
//		timeBound_(timeBound), pcIncrement_(pcIncrement) {}
//	TinyRAMProtoboardParams()
//		: archParams_(TinyRAMArchParams{ 0, 0 }), opcodeWidth_(0), timeBound_(0), pcIncrement_(0) {}
//	TinyRAMArchParams archParams() const { return archParams_; }
//	size_t opcodeWidth() const { return opcodeWidth_; }
//	size_t numRegisters() const { return archParams_.numRegisters; }
//	size_t registerLength() const { return archParams_.registerLength; }
//	size_t registerIndexLength() const { return Log2ceil(numRegisters()); }
//	size_t arg2length() const { return std::max({ registerIndexLength(), registerLength() }); }
//	size_t numOpcodes() const { return 1u << (opcodeWidth()); }
//	size_t timeBound() const { return timeBound_; }
//	size_t pcIncrement() const { return pcIncrement_; }
//}; // class TinyRAMProtoboardParams
//
} // namespace gadgetlib


#endif // LIBSNARK_GADGETLIB3_INCLUDE_GADGETLIB3_PROTOBOARD_HPP_
