/************************************** TinyRAMInstance.hpp **************************************/
/**
 * @file
 *
 * @brief The file TinyRAMInstance.hpp contains the interface for a TinyRAM instance.
 * 
 * Main classes defined in the file:
 *     TinyRAMPartialInstance
 *     TinyRAMFullInstance
 */
  /***********************************************************************************************/

#ifndef __RAM_INSTANCE_HPP
#define __RAM_INSTANCE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include "TinyRAM/TinyRAMDefinitions.hpp"
#include "common/Utils/ErrorHandling.hpp"
#include <algorithm>
#include <math.h>
#include <algebraLib/FieldElement.hpp>
#include <algebraLib/CircuitPolynomial.hpp>
#include <algebraLib/variable.hpp>
#include <gadgetlib/infrastructure.hpp>
#include <gadgetlib/common_use.hpp>
#include <gadgetlib/protoboard.hpp>

using gadgetlib::Opcode;
using gadgetlib::Log2ceil;



/*************************************************************************************************/
/*************************************************************************************************/
/****************************                                         ****************************/
/****************************         class RAMArchParams         ****************************/
/****************************                                         ****************************/
/*************************************************************************************************/
/*************************************************************************************************/

struct RAMArchParams {
	size_t numRegisters;
	size_t registerLength;

	bool operator==(const RAMArchParams& rhs) const;
};

/*************************************************************************************************/
/*************************************************************************************************/
/****************************                                         ****************************/
/****************************         class TinyRAMProgram            ****************************/
/****************************                                         ****************************/
/*************************************************************************************************/
/*************************************************************************************************/
/// A data object which holds a TinyRAM code and auxiliary information.
struct MachineInstruction {
	Opcode opcode_ = Opcode::ANSWER;
	bool arg2isImmediate_ = true;
	size_t destIdx_ = 0;
	size_t arg1Idx_ = 0;
	size_t arg2IdxOrImmediate_ = 1;

	MachineInstruction(
		const Opcode& opcode,
		const bool arg2isImmediate,
		const size_t destIdx,
		const size_t arg1Idx,
		const size_t arg2IdxOrImmediate);
	
    MachineInstruction(const std::string line);

    void print()const;
};

class TinyRAMProgram {
public:
	typedef std::vector<MachineInstruction> RAMMachineCode;
private:
	std::string name_;
	RAMArchParams archParams_;
	RAMMachineCode code_;
public:
	TinyRAMProgram(const std::string& name,
		const RAMArchParams& archParams,
		const RAMMachineCode& code) :
		name_(name), archParams_(archParams), code_(code) {}

	TinyRAMProgram(const std::string& name,
		size_t numRegisters,
		size_t wordSize) :
		name_(name), archParams_(RAMArchParams{ numRegisters, wordSize }) {
	}

	std::string name() const { return name_; }
	const RAMMachineCode& code() const { return code_; }
	const size_t size() const { return code_.size(); }
	const RAMArchParams& archParams() const { return archParams_; }
	const MachineInstruction& getInstructionAtPc(const size_t pc) const { return code_[pc]; }
	void addInstruction(const MachineInstruction& instruction) { code_.emplace_back(instruction); }
	void addInstructionsFromFile(const std::string filename);
    unsigned int pcLength() const {
		int codeSize = code_.size();
		if (codeSize == 0){ _COMMON_FATAL("TinyRAMProgram : The code is not initialized"); };
		if (codeSize == 1) { return 1; }
		return  gadgetlib::Log2ceiled(codeSize);
	}

    void print()const;

};


/*************************************************************************************************/
/*************************************************************************************************/
/****************************                                         ****************************/
/****************************         class TinyRAMProtoboardParams   ****************************/
/****************************                                         ****************************/
/*************************************************************************************************/
/*************************************************************************************************/

class TinyRAMProtoboardParams : public gadgetlib::ProtoboardParams {
private:
	RAMArchParams archParams_;
	size_t opcodeWidth_;
	size_t timeBound_;
	size_t pcIncrement_;
public:
	TinyRAMProtoboardParams(unsigned int numRegisters, unsigned int registerLength,
		size_t opcodeWidth, size_t timeBound, size_t pcIncrement)
		: archParams_(RAMArchParams{ numRegisters, registerLength }),
		opcodeWidth_(opcodeWidth),
		timeBound_(timeBound), pcIncrement_(pcIncrement) {}
	TinyRAMProtoboardParams()
		: archParams_(RAMArchParams{ 0, 0 }), opcodeWidth_(0), timeBound_(0), pcIncrement_(0) {}
	RAMArchParams archParams() const { return archParams_; }
	size_t opcodeWidth() const { return opcodeWidth_; }
	size_t numRegisters() const { return archParams_.numRegisters; }
	size_t registerLength() const { return archParams_.registerLength; }
	size_t registerIndexLength() const { return Log2ceil(numRegisters()); }
	size_t arg2length() const { return std::max({ registerIndexLength(), registerLength() }); }
	size_t numOpcodes() const { return 1u << (opcodeWidth()); }
	size_t timeBound() const { return timeBound_; }
	size_t pcIncrement() const { return pcIncrement_; }
}; // class TinyRAMProtoboardParams


#endif 
