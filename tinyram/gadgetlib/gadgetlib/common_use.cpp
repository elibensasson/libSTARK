#include <gadgetlib/common_use.hpp>
#include <gadgetlib/infrastructure.hpp>
#include <algebraLib/variable_operators.hpp>
#include <stdint.h>

namespace gadgetlib{

using Algebra::FieldElement;
using Algebra::FElem;
using Algebra::xFE;

	//useful for constructing SelectorSum objects
	std::vector<Algebra::Variable> getPCVars(const Algebra::UnpackedWord& pc){
		std::vector<Algebra::Variable> retVal;
		for (unsigned int i = 0; i < pc.size(); i++)
			retVal.push_back(pc[i]);
		return retVal;
	}


	//MachineInstruction::MachineInstruction(const Opcode& opcode, const bool arg2isImmediate,
	//	const size_t destIdx, const size_t arg1Idx, const size_t arg2IdxOrImmediate) :
	//	opcode_(opcode), arg2isImmediate_(arg2isImmediate), destIdx_(destIdx), arg1Idx_(arg1Idx),
	//	arg2IdxOrImmediate_(arg2IdxOrImmediate){}

FieldElement getGF2E_X() {
	return xFE();
}

const FieldElement& getGenerator2toTheNthPower(const unsigned int n){
	static std::vector<FieldElement> g_2_i = { getGF2E_X() };
	for (unsigned int i = g_2_i.size(); i <= n; ++i)
			g_2_i.push_back(Algebra::power(g_2_i[i - 1], 2));
	return g_2_i[n];
}

FieldElement getGeneratorPower(const unsigned int n){
	const FieldElement generator = getGF2E_X();

	FieldElement res = Algebra::one();
    const int expLog = floor(Log2(n));
    for(int i=expLog; i>=0; i--){
        res *= res;
        if((n>>i) & 1UL){
            res *= generator;
        }
    }
    
    return res;
}



#define tempSelector
Algebra::CircuitPolynomial getSelector(int programLength, int instructionLine, Algebra::UnpackedWord unpackedPC){
	GADGETLIB_ASSERT(unpackedPC.size() >= Log2ceiled(programLength), "Number of unpacked bits used should be at least log length of the program");
	FElem value = (instructionLine & 1U) ? Algebra::one() : Algebra::zero();
#ifndef tempSelector

	Algebra::CircuitPolynomial selectorPoly(Algebra::one() + value + unpackedPC[0]);

	instructionLine >>= 1U;
	int i = 1;
	while (instructionLine || i < unpackedPC.size()){
		FElem value = (instructionLine & 1U) ? Algebra::one() : Algebra::zero();
		selectorPoly = selectorPoly * (Algebra::one() + value + unpackedPC[i]);
		i++;
		instructionLine >>= 1U;
	}
	return selectorPoly;
#else
	std::vector<Algebra::LinearCombination> lcVec;
	Algebra::LinearCombination lc(Algebra::one() + value + unpackedPC[0]);
	lcVec.push_back(lc);
	instructionLine >>= 1U;
	unsigned int i = 1;
	while (instructionLine || i < unpackedPC.size()){
		FElem value = (instructionLine & 1U) ? Algebra::one() : Algebra::zero();
		lcVec.push_back(Algebra::one() + value + unpackedPC[i]);
		i++;
		instructionLine >>= 1U;
	}
	return Algebra::CircuitPolynomial(lcVec);

#endif
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					Memory Info				 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
MemoryInfo::MemoryInfo(){
	serialNumber_ = 0;
	isMemOp_ = 0;
	isLoadOp_ = 0;
	timestamp_ = Algebra::zero();
	timestampDegree_ = -1;
	address_ = Algebra::zero();
	value_ = Algebra::zero();
}

MemoryInfo::MemoryInfo(int serialNumber, bool isMemOp, bool isLoadOp,
	const FElem& timestamp, int timestampDegree,
	const FElem& address, const FElem& value){
	serialNumber_ = serialNumber;
	isMemOp_ = isMemOp;
	isLoadOp_ = isLoadOp;
	timestamp_ = timestamp;
	timestampDegree_ = timestampDegree;
	address_ = address;
	value_ = value;
	GADGETLIB_ASSERT(timestamp == getGeneratorPower(timestampDegree_), "g^timestampDegree != timestamp");
}

void MemoryInfo::updateTimestamp(FElem timestamp, int timestampDegree){
	GADGETLIB_ASSERT(timestamp == getGeneratorPower(timestampDegree), "g^timestampDegree != timestamp");
	timestamp_ = timestamp;
	timestampDegree_ = timestampDegree;
}

bool sortMemoryInfo(MemoryInfo a, MemoryInfo b){
	if (!a.getIsMemOp() && !b.getIsMemOp()){
		return a.getSerialNumber() < b.getSerialNumber();
	}
	else if (a.getIsMemOp() && !b.getIsMemOp()){
		return true;
	}
	else if (!a.getIsMemOp() && b.getIsMemOp()){
		return false;
	}
	else{ // MemOp = True -> Sort By Address
		int aAddr = mapFieldElementToInteger(0, 16, a.getAddress());
		int bAddr = mapFieldElementToInteger(0, 16, b.getAddress());
		if (aAddr < bAddr){
			return true;
		}
		else if (bAddr < aAddr){
			return false;
		}
		else{ // MemOp = True and Same Address -> Sort By TimeStamp
			return a.getTimestampDegree() < b.getTimestampDegree();
		}
	}
}



} // namespace
