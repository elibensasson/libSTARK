#include <gtest/gtest.h>
#include <memory>
#include <TinyRAMtoBair/RamToContraintSystem/traceConsistency.hpp>
#include <TinyRAMtoBair/RamToContraintSystem/generalPurpose.hpp>
#include <TinyRAM/TinyRAMDefinitions.hpp>
#include <TinyRAM/TinyRAMInstance.hpp>



namespace{
	TEST(ALU, traceConsistency){
		// Initialize pb and gadget parameters
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);

		
		// Initialize Program
		TinyRAMProgram program("inputConsistencyTEST", trNumRegisters, trRegisterLen);
		MachineInstruction instruction1(Opcode::ADD, false, 2, 1, 1);
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::ADD, false, 3, 1, 1);
		program.addInstruction(instruction2);
		MachineInstruction instruction3(Opcode::ADD, false, 4, 1, 1);
		program.addInstruction(instruction3);
		MachineInstruction instruction4(Opcode::ADD, false, 5, 1, 1);
		program.addInstruction(instruction4);

		// Initialize traceVariable
		const int pcLength = program.pcLength();
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		// Init neccassary variables
		FollowingTraceVariables followingTraceVariables(pcLength, trNumRegisters);
		ALUOutput aluOutput(followingTraceVariables.second_.flag_);


		pb->val(followingTraceVariables.first_.flag_) = Algebra::zero();
		for (int i = 0; i < pcLength; i++){
			pb->val(followingTraceVariables.first_.pc_[i]) = Algebra::zero();
		}
		pb->val(followingTraceVariables.first_.timeStamp_) = Algebra::one();
		Algebra::FElem val = Algebra::one();
		vector<Algebra::FElem> registerValues;
		for (int i = 0; i < trNumRegisters; ++i){
			pb->val(followingTraceVariables.first_.registers_[i]) = val;
			registerValues.emplace_back(val);
			val *= generator;
		}
		// Initialize useful variables
		Algebra::FElem x_i = generator;

		//Initialize Gadget
		GadgetPtr traceConsistency = TraceConsistency::create(pb, aluOutput, followingTraceVariables);
		(::std::dynamic_pointer_cast<TraceConsistency>(traceConsistency))->setProgram(program);
		traceConsistency->generateConstraints();
		// Generate Witness
		pb->val(aluOutput.flag_) = Algebra::zero();
		pb->val(aluOutput.result_) = Algebra::one();
		// Code line 0
		(::std::dynamic_pointer_cast<TraceConsistency>(traceConsistency))->generateWitness(0);
		
		EXPECT_EQ(pb->val(followingTraceVariables.second_.timeStamp_), x_i);
		EXPECT_EQ(pb->val(followingTraceVariables.second_.registers_[2]), Algebra::one());
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 2) {
				continue;
			}
			Algebra::FElem valueFirstRegI = pb->val(followingTraceVariables.first_.registers_[i]);
			Algebra::FElem valueNextRegI = pb->val(followingTraceVariables.second_.registers_[i]);
			EXPECT_EQ(valueFirstRegI, valueNextRegI);
		}
		//check pc
		EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[0]), Algebra::one());
		for (int i = 1; i < pcLength; i++){
			EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[i]), Algebra::zero());
		}

		// code line 1
		pb->val(followingTraceVariables.first_.timeStamp_) = pb->val(followingTraceVariables.second_.timeStamp_);
		// update pc
		for (int i = 0; i < pcLength; i++){
			pb->val(followingTraceVariables.first_.pc_[i]) = pb->val(followingTraceVariables.second_.pc_[i]);
		}

		(::std::dynamic_pointer_cast<TraceConsistency>(traceConsistency))->generateWitness(1);
		x_i *= generator;
		EXPECT_EQ(pb->val(followingTraceVariables.second_.timeStamp_), x_i);
		EXPECT_EQ(pb->val(followingTraceVariables.second_.registers_[3]), Algebra::one());
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 3) {
				continue;
			}
			Algebra::FElem valueFirstRegI = pb->val(followingTraceVariables.first_.registers_[i]);
			Algebra::FElem valueNextRegI = pb->val(followingTraceVariables.second_.registers_[i]);
			EXPECT_EQ(valueFirstRegI, valueNextRegI);
		}
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		
		//check pc
		EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[0]), Algebra::zero());
		EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[1]), Algebra::one());
		for (int i = 2; i < pcLength; i++){
			EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[i]), Algebra::zero());
		}

		//code line 2
		pb->val(followingTraceVariables.first_.timeStamp_) = pb->val(followingTraceVariables.second_.timeStamp_);
		// update pc
		for (int i = 0; i < pcLength; i++){
			pb->val(followingTraceVariables.first_.pc_[i]) = pb->val(followingTraceVariables.second_.pc_[i]);
		}

		(::std::dynamic_pointer_cast<TraceConsistency>(traceConsistency))->generateWitness(2);
		x_i *= generator;
		EXPECT_EQ(pb->val(followingTraceVariables.second_.timeStamp_), x_i);
		EXPECT_EQ(pb->val(followingTraceVariables.second_.registers_[4]), Algebra::one());
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 4) {
				continue;
			}
			Algebra::FElem valueFirstRegI = pb->val(followingTraceVariables.first_.registers_[i]);
			Algebra::FElem valueNextRegI = pb->val(followingTraceVariables.second_.registers_[i]);
			EXPECT_EQ(valueFirstRegI, valueNextRegI);
		}
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));

	}


}
