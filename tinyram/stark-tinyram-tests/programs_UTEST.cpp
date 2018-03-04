#include <gtest/gtest.h>
#include <gadgetlib/protoboard.hpp>
#include <TinyRAM/TinyRAMDefinitions.hpp>
#include <TinyRAMtoBair/RamToContraintSystem/transitionFunction.hpp>

using namespace gadgetlib;


namespace{
	void initializeVars(ProtoboardPtr pb, FollowingTraceVariables followingTraceVariables){
		pb->val(followingTraceVariables.first_.flag_) = Algebra::zero();
		pb->val(followingTraceVariables.first_.timeStamp_) = Algebra::one();
		int pcLength = followingTraceVariables.first_.pc_.size();
		for (int i = 0; i < pcLength; i++){
			pb->val(followingTraceVariables.first_.pc_[i]) = Algebra::zero();
		}
		for (int i = 0; i < trNumRegisters; i++){
			pb->val(followingTraceVariables.first_.registers_[i]) = Algebra::zero();
		}
	}

	void copyTraceOutputValuesToTraceInput(ProtoboardPtr pb, FollowingTraceVariables followingTraceVariables){
		pb->val(followingTraceVariables.first_.timeStamp_) = pb->val(followingTraceVariables.second_.timeStamp_);
		pb->val(followingTraceVariables.first_.flag_) = pb->val(followingTraceVariables.second_.flag_);
		int pcLength = followingTraceVariables.first_.pc_.size();
		for (int i = 0; i < pcLength; i++){
			pb->val(followingTraceVariables.first_.pc_[i]) = pb->val(followingTraceVariables.second_.pc_[i]);
		}
		for (int i = 0; i < trNumRegisters; i++){
			pb->val(followingTraceVariables.first_.registers_[i]) = pb->val(followingTraceVariables.second_.registers_[i]);
		}
	}


	TEST(TinyRAMPrograms, checkCNJMP){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);

		TinyRAMProgram program("program", trNumRegisters, trRegisterLen);
		MachineInstruction instruction1(Opcode::XOR, true, 1, 1, 1);
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::CNJMP, true, 0, 0, 0);
		program.addInstruction(instruction2);
		MachineInstruction instruction3(Opcode::ADD, true, 2, 2, 1);
		program.addInstruction(instruction3);
		MachineInstruction instruction4(Opcode::ADD, true, 2, 2, 1);
		program.addInstruction(instruction4);

		//Variables
		const int pcLength = program.pcLength();
		FollowingTraceVariables followingTraceVariable(program.pcLength(), trNumRegisters);
		MemoryFollowingTraceVariables memoryFollowingTraceVariables(followingTraceVariable.first_.timeStamp_, followingTraceVariable.second_.timeStamp_);
		GadgetPtr transitionFunction = TransitionFunction::create(pb, followingTraceVariable, memoryFollowingTraceVariables, program);

		transitionFunction->generateConstraints();
		
		initializeVars(pb, followingTraceVariable);
		// Generate witness for line 0 
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(0);

		EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[1]), Algebra::one());
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 1){ continue; }
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 1
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(1);

		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		for (int i = 0; i < pcLength; i++){
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 1){ continue; }
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}

		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line  0 - Because of CJMP
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(2);

		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::one());
		for (int i = 0; i < trNumRegisters; i++){
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		for (int i = 0; i < pcLength; i++){
			if (i == 0){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 1 
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(3);
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::one());
		for (int i = 0; i < trNumRegisters; i++){
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		for (int i = 0; i < pcLength; i++){
			if (i == 1){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 2
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(4);
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 2){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		for (int i = 0; i < pcLength; i++){
			if (i == 1 || i == 0){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}
	}

	TEST(TinyRAMPrograms, checkCJMP){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);

		TinyRAMProgram program("program", trNumRegisters, trRegisterLen);
		MachineInstruction instruction1(Opcode::XOR, true, 1, 1, 1);
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::XOR, true, 1, 1, 1);
		program.addInstruction(instruction2);
		MachineInstruction instruction3(Opcode::CJMP, true, 0, 0, 5);
		program.addInstruction(instruction3);
		MachineInstruction instruction4(Opcode::ADD, true, 2, 2, 0);
		program.addInstruction(instruction4);
		MachineInstruction instruction5(Opcode::ADD, true, 2, 2, 1);
		program.addInstruction(instruction5);
		MachineInstruction instruction6(Opcode::ADD, true, 2, 2, 2);
		program.addInstruction(instruction6);

		const int pcLength = program.pcLength();
		FollowingTraceVariables followingTraceVariable(pcLength, trNumRegisters);
		MemoryFollowingTraceVariables memoryFollowingTraceVariables(followingTraceVariable.first_.timeStamp_, followingTraceVariable.second_.timeStamp_);
		GadgetPtr transitionFunction = TransitionFunction::create(pb, followingTraceVariable, memoryFollowingTraceVariables, program);

		transitionFunction->generateConstraints();

		initializeVars(pb, followingTraceVariable);
		// Generate witness for line 0 
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(0);

		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());

		for (int i = 0; i < trNumRegisters; i++){
			if (i == 1){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}

		for (int i = 0; i < pcLength; i++){
			if (i == 0){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}

		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);

		// Generate witness for line 1 
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(1);

		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::one()); 
		for (int i = 0; i < trNumRegisters; i++){
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]),Algebra::zero());
		}
		for (int i = 0; i < pcLength;  i++){
			if (i == 1){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}
		
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		pb->val(followingTraceVariable.second_.flag_) = Algebra::zero();
		EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
		pb->val(followingTraceVariable.second_.flag_) = Algebra::one();

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 1 
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(2);
		for (int i = 0; i < trNumRegisters; i++){
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		
		for (int i = 0; i < pcLength; i++){
			if (i == 0 || i == 2){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}
		
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		pb->val(followingTraceVariable.second_.pc_[1]) = Algebra::one();
		EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
		pb->val(followingTraceVariable.second_.pc_[1]) = Algebra::zero();
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 5 - Because of CJMP
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(3);
		FElem g = mapIntegerToFieldElement(0, trRegisterLen, 2);
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());
		for (int i = 0; i < pcLength; i++){
			if (i == 1 || i == 2){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 2){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), g);
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		EXPECT_ANY_THROW((::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(4));
	}

	TEST(TinyRAMPrograms, checkSUBADD){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);

		TinyRAMProgram program("program", trNumRegisters, trRegisterLen);
		MachineInstruction instruction1(Opcode::ADD, true, 4, 4, 4);
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::XOR, true, 1, 1, 1);
		program.addInstruction(instruction2);
		MachineInstruction instruction3(Opcode::SUB, true, 4, 4, 0);
		program.addInstruction(instruction3);
		MachineInstruction instruction4(Opcode::SUB, true, 4, 4, 4);
		program.addInstruction(instruction4);
	
		const int pcLength = program.pcLength();
		FollowingTraceVariables followingTraceVariable(pcLength, trNumRegisters);
		MemoryFollowingTraceVariables memoryFollowingTraceVariables(followingTraceVariable.first_.timeStamp_, followingTraceVariable.second_.timeStamp_);
		GadgetPtr transitionFunction = TransitionFunction::create(pb, followingTraceVariable, memoryFollowingTraceVariables, program);

		transitionFunction->generateConstraints();
		
		initializeVars(pb, followingTraceVariable);
		// Generate witness for line 0 
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(0);
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		for (int i = 0; i < trNumRegisters; i++){
			if (i == 4){
				Algebra::FieldElement val = mapIntegerToFieldElement(0, trRegisterLen, 4);
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]),val);
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		for (int i = 0; i < pcLength; i++){
			if (i == 0) {
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
			pb->val(followingTraceVariable.second_.pc_[i]) = Algebra::one();
			EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
			pb->val(followingTraceVariable.second_.pc_[i]) = mapIntegerToFieldElement(0, trRegisterLen, i);
			EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
			pb->val(followingTraceVariable.second_.pc_[i]) = Algebra::zero();
			EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		}
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());
		pb->val(followingTraceVariable.second_.flag_) = mapIntegerToFieldElement(0, trRegisterLen, 4);
		EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
		pb->val(followingTraceVariable.second_.flag_) = Algebra::zero();
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 1
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(1);

		for (int i = 0; i < pcLength; i++){
			if (i == 1){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}

		for (int i = 0; i < trNumRegisters; i++){
			if (i == 1){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::one());
				continue;
			}
			if (i == 4){
				Algebra::FieldElement val = mapIntegerToFieldElement(0, trRegisterLen, 4);
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), val);
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());
		FElem g = Algebra::FElem(getGF2E_X());
		EXPECT_EQ(pb->val(followingTraceVariable.first_.timeStamp_), g);
		EXPECT_EQ(pb->val(followingTraceVariable.second_.timeStamp_), g*g);
		pb->val(followingTraceVariable.first_.timeStamp_) = g * g;
		EXPECT_FALSE(pb->isSatisfied(Opcode::NONE));
		pb->val(followingTraceVariable.first_.timeStamp_) = g;
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));

		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 2
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(2);

		for (int i = 0; i < pcLength; i++){
			if (i == 1 || i == 0){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}

		for (int i = 0; i < trNumRegisters; i++){
			if (i == 1){
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::one());
				continue;
			}
			if (i == 4){
				Algebra::FieldElement val = mapIntegerToFieldElement(0, trRegisterLen, 4);
				EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), val);
				continue;
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[i]), Algebra::zero());
		}
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());
	
		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 3
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(3);
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::zero());
		EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[4]), Algebra::zero());
	}
		
	TEST(TinyRAMPrograms, checkADDBorrow){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);

		TinyRAMProgram program("program", trNumRegisters, trRegisterLen);
		MachineInstruction instruction1(Opcode::ADD, true, 0, 0, 65535); //r0 = r0 + (2^16 -1)
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::ADD, true, 0, 0, 1); // r0 = r0 + 1;
		program.addInstruction(instruction2);
		
		const int pcLength = program.pcLength();
		FollowingTraceVariables followingTraceVariable(pcLength, trNumRegisters);
		MemoryFollowingTraceVariables memoryFollowingTraceVariables(followingTraceVariable.first_.timeStamp_, followingTraceVariable.second_.timeStamp_);
		GadgetPtr transitionFunction = TransitionFunction::create(pb, followingTraceVariable, memoryFollowingTraceVariables, program);

		transitionFunction->generateConstraints();

		initializeVars(pb, followingTraceVariable);
		// Generate witness for line 0 
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(0);
		
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		FElem val = mapIntegerToFieldElement(0, trRegisterLen, 65535);
 		EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[0]), val);
		for (int i = 0; i < pcLength; i++){
			if (i == 0){ 
				EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::one());
				continue; 
			}
			EXPECT_EQ(pb->val(followingTraceVariable.second_.pc_[i]), Algebra::zero());
		}
		
		copyTraceOutputValuesToTraceInput(pb, followingTraceVariable);
		// Generate witness for line 1
		(::std::dynamic_pointer_cast<TransitionFunction>)(transitionFunction)->generateWitness(1);
		EXPECT_TRUE(pb->isSatisfied(Opcode::NONE));
		EXPECT_EQ(pb->val(followingTraceVariable.second_.flag_), Algebra::one());
		EXPECT_EQ(pb->val(followingTraceVariable.second_.registers_[0]), Algebra::zero());
	}


}
