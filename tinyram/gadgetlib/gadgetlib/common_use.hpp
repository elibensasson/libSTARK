#ifndef GADGETLIB3_COMMON_USE_HPP_
#define GADGETLIB3_COMMON_USE_HPP_

#include <algorithm>
#include <vector>
#include <math.h>
#include <algebraLib/FieldElement.hpp>
#include <algebraLib/CircuitPolynomial.hpp>
#include <algebraLib/variable.hpp>
#include <gadgetlib/infrastructure.hpp>


namespace gadgetlib{
	Algebra::FieldElement getGF2E_X();
	const Algebra::FieldElement& getGenerator2toTheNthPower(const unsigned int n);
	Algebra::FieldElement getGeneratorPower(const unsigned int n);

enum class Opcode : int {
	MEMORY = -2,
	NONE = -1,
	AND = 0,
	OR = 1,
	XOR = 2,
	NOT = 3,
	ADD = 4,
	SUB = 5,
	MULL = 6,
	UMULH = 7,
	SMULH = 8,
	UDIV = 9,
	UMOD = 10,
	SHL = 11,
	SHR = 12,
	SHAR = 13,

	CMPE = 14,
	CMPA = 15,
	CMPAE = 16,
	CMPG = 17,
	CMPGE = 18,

	MOV = 19,
	CMOV = 20,
	JMP = 21,

	CJMP = 22,
	CNJMP = 23,

	RESERVED_OPCODE_24 = 24,
	RESERVED_OPCODE_25 = 25,

	STOREB = 26,
	LOADB = 27,
	STOREW = 28,
	LOADW = 29,
	READ = 30,
	ANSWER = 31,
	NUM_OPCODES = 32
}; // enum Opcode


/*************************************************************************************************/
/*************************************************************************************************/
/****************************                                         ****************************/
/****************************         Selector Functions              ****************************/
/****************************                                         ****************************/
/*************************************************************************************************/
/*************************************************************************************************/

//useful for constructing SelectorSum objects
std::vector<Algebra::Variable> getPCVars(const Algebra::UnpackedWord& pc);


// We need the unpackedPC be at least the log(programLength). 
// The polynomial is Product (
Algebra::CircuitPolynomial getSelector(int programLength, int instructionLine, Algebra::UnpackedWord unpackedPC);



/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					Memory Info				 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class MemoryInfo{
private:
	int serialNumber_;
	bool isMemOp_;
	bool isLoadOp_;
	Algebra::FElem timestamp_;
	int timestampDegree_;
	Algebra::FElem address_;
	Algebra::FElem value_;
public:
	MemoryInfo();
	MemoryInfo(int serialNumber, bool isMemOp, bool isLoadOp,
		const Algebra::FElem& timestamp, int timestampDegree,
		const Algebra::FElem& address, const Algebra::FElem& value);
	int getSerialNumber() const{ return serialNumber_; }
	bool getIsMemOp() const{ return isMemOp_; }
	bool getIsLoadOp() const { return isLoadOp_; }
	Algebra::FElem getTimestamp() const{ return timestamp_; }
	int getTimestampDegree() const{ return timestampDegree_; }
	Algebra::FElem getAddress() const { return address_; }
	Algebra::FElem getValue() const{ return value_; }
	void updateIsMemOp(bool isMemOp){ isMemOp_ = isMemOp; }
	void updateIsLoadOp(bool isLoadOp){ isLoadOp_ = isLoadOp; }
	void updateAddress(const Algebra::FElem& address){ address_ = address; }
	void updateSerialNumber(int serialNumber){ serialNumber_ = serialNumber; }
	void updateValue(const Algebra::FElem& value){ value_ = value; }
	void updateTimestamp(Algebra::FElem timestamp, int timestampDegree);
};

inline bool operator==(const MemoryInfo& a, const MemoryInfo& b){
	return (a.getSerialNumber() == b.getSerialNumber() && a.getIsMemOp() == b.getIsMemOp() && a.getIsLoadOp() == b.getIsLoadOp() &&
		a.getTimestamp() == b.getTimestamp() && a.getAddress() == b.getAddress() && a.getValue() == b.getValue());
}

bool sortMemoryInfo(MemoryInfo a, MemoryInfo b);

} // namespace


#endif //  GADGETLIB3_COMMON_USE_HPP_
