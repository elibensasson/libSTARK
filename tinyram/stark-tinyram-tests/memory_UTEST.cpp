#include <gtest/gtest.h>
#include <memory>
#include <gadgetlib/protoboard.hpp>
#include <algebraLib/variable_operators.hpp>
#include <TinyRAM/TinyRAMDefinitions.hpp>
#include <TinyRAM/TinyRAMInstance.hpp>
#include <TinyRAMtoBair/RamToContraintSystem/MemoryConsraints.hpp>

using namespace gadgetlib;


#define NUM_DEGREES 10

namespace{

	void initMemVars(ProtoboardPtr pb, MemoryTraceVariables traceLines){
		pb->val(traceLines.isMemOp_) = Algebra::zero();
		pb->val(traceLines.isLoad_) = Algebra::zero();
		pb->val(traceLines.address_) = Algebra::zero();
		pb->val(traceLines.timeStamp_) = Algebra::one();
		pb->val(traceLines.value_) = Algebra::zero();
	
	}

	TEST(Memory, constraintsCheck){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		MemoryFollowingTraceVariables traceLines;
		GadgetPtr memoryConstraints = MemoryConstraints::create(pb, traceLines);
		
		// Add mapping between FieldElement and degree
		const Algebra::FElem g = Algebra::FElem(getGF2E_X());
		Algebra::FElem x_i = Algebra::one();
		for (unsigned int i = 0; i < NUM_DEGREES; i++){
			pb->addDegreeTranslation(x_i, i);
			x_i *= g;
		}

		// Init All params to zero
		initMemVars(pb, traceLines.first_);
		initMemVars(pb, traceLines.second_);

		// Generate Constraints
		memoryConstraints->generateConstraints();

		//If current opcode is NOT memory then the next opcode can't
		//be as well
		pb->val(traceLines.second_.isMemOp_) = Algebra::one();
		// generate witness
		memoryConstraints->generateWitness();
		EXPECT_FALSE(pb->isSatisfied(Opcode::MEMORY));

		// If opcode is Mem then addr' >= addr 
		pb->val(traceLines.first_.isMemOp_) = Algebra::one();
		// traceLines.first_.address_ = 1 , traceLines.second_.address_ = 0 
		pb->val(traceLines.first_.address_) = Algebra::one();
		memoryConstraints->generateWitness();
		EXPECT_FALSE(pb->isSatisfied(Opcode::MEMORY));
	
		// If opcode is Mem and addr'==addr then timeStamp' > timestamp
		pb->val(traceLines.second_.address_) = Algebra::one();
		// traceLines.first_.address_ = 1 , traceLines.second_.address_ = 1 
		// traceLines.first_.timestamp = g, traceLines.second_.timestamp_ = 1
		const FElem generator = Algebra::FElem(getGF2E_X());
		pb->val(traceLines.first_.timeStamp_) = generator;
		memoryConstraints->generateWitness();
		EXPECT_FALSE(pb->isSatisfied(Opcode::MEMORY));

		pb->val(traceLines.second_.timeStamp_) = generator * generator;
		memoryConstraints->generateWitness();
		EXPECT_TRUE(pb->isSatisfied(Opcode::MEMORY));
		
		// If opcode is Mem addr'==add and isLoad' = True then value' == value
		pb->val(traceLines.second_.isLoad_) = Algebra::one();
		memoryConstraints->generateWitness();
		EXPECT_TRUE(pb->isSatisfied(Opcode::MEMORY));
		pb->val(traceLines.second_.value_) = generator;
		EXPECT_FALSE(pb->isSatisfied(Opcode::MEMORY));

	}

}
