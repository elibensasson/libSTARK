#include <gtest/gtest.h>
#include <memory>
#include <gadgetlib/protoboard.hpp>
#include <gadgetlib/gadget.hpp>
#include <TinyRAMtoBair/RamToContraintSystem/generalPurpose.hpp>
#include <TinyRAMtoBair/RamToContraintSystem/transitionFunction.hpp>
#include <TinyRAM/TinyRAMDefinitions.hpp>
#include <TinyRAM/TinyRAMInstance.hpp>
using namespace gadgetlib;


namespace {

	TEST(ALU, transitionFunction){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);

		
		// Init program
		TinyRAMProgram program("program", trNumRegisters, trRegisterLen);
		MachineInstruction instruction1(Opcode::XOR, false, 2, 1, 1);
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::AND, false, 3, 2, 2);
		program.addInstruction(instruction2);
		MachineInstruction instruction3(Opcode::OR, false, 4, 3, 3);
		program.addInstruction(instruction3);

		// Init variables
		const int pcLength = program.pcLength();
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		// Init used Variables
		FollowingTraceVariables followingTraceVariables(pcLength, trNumRegisters);
		MemoryFollowingTraceVariables memoryFollowingTraceVariables;



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
		
		// Init Gadget 
		GadgetPtr transitionFunction = TransitionFunction::create(pb, followingTraceVariables, memoryFollowingTraceVariables, program);
		transitionFunction->generateConstraints();
		
		//generate Witness
		//line 0
		for (unsigned int j = 0; j < program.size(); j++){
			std::dynamic_pointer_cast<TransitionFunction>(transitionFunction)->generateWitness(j);

			EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
			//Check PC
			if (j == 0){
				EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[0]), Algebra::one());
				for (int i = 1; i < pcLength; i++){
					EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[i]), Algebra::zero());
				}
			}
			else if (j == 1){
				EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[0]), Algebra::zero());
				EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[1]), Algebra::one());
				for (int i = 2; i < pcLength; i++){
					EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[i]), Algebra::zero());
				}
			}
			else{ //If j == 2
				GADGETLIB_ASSERT(j == 2, "There only 3 lines in the code");
				EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[0]), Algebra::one());
				EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[1]), Algebra::one());
				for (int i = 2; i < pcLength; i++){
					EXPECT_EQ(pb->val(followingTraceVariables.second_.pc_[i]), Algebra::zero());
				}
			}
			
			// Change pc and check soundness
			for (int i = 0; i < pcLength; i++){
				pb->val(followingTraceVariables.second_.pc_[i]) = Algebra::one() + pb->val(followingTraceVariables.second_.pc_[i]);
				EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
				pb->val(followingTraceVariables.second_.pc_[i]) = Algebra::one() + pb->val(followingTraceVariables.second_.pc_[i]);
				EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
			}

			// Check flag
			EXPECT_EQ(pb->val(followingTraceVariables.second_.flag_), Algebra::one());
			pb->val(followingTraceVariables.second_.flag_) += Algebra::one();
			EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
			pb->val(followingTraceVariables.second_.flag_) += Algebra::one();
			EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));

			// Check timeStamp
			EXPECT_EQ(pb->val(followingTraceVariables.second_.timeStamp_), 
						pb->val(followingTraceVariables.first_.timeStamp_) * generator);
			pb->val(followingTraceVariables.second_.timeStamp_) = Algebra::one();
			EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
			pb->val(followingTraceVariables.second_.timeStamp_) = pb->val(followingTraceVariables.first_.timeStamp_) * generator;

			// Check Registers
			for (int i = 0; i < trNumRegisters; i++){
				if (i == (int)program.code()[j].destIdx_){
					continue;
				}
				EXPECT_EQ(pb->val(followingTraceVariables.first_.registers_[i]),
					pb->val(followingTraceVariables.second_.registers_[i]));
			}
			FElem changedRegister = pb->val(followingTraceVariables.second_.registers_[program.code()[j].destIdx_]);
			EXPECT_EQ(changedRegister, Algebra::zero());

			//Copy value
			pb->val(followingTraceVariables.first_.timeStamp_) = pb->val(followingTraceVariables.second_.timeStamp_);
			pb->val(followingTraceVariables.first_.flag_) = pb->val(followingTraceVariables.second_.flag_);
			for (int i = 0; i < trNumRegisters; i++){
				pb->val(followingTraceVariables.first_.registers_[i]) = pb->val(followingTraceVariables.second_.registers_[i]);
			}
			for (int i = 0; i < pcLength; i++){
				pb->val(followingTraceVariables.first_.pc_[i]) = pb->val(followingTraceVariables.second_.pc_[i]);
			}

		}
		


	}

} //namespace
