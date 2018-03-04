#include <gtest/gtest.h>
#include <memory>
#include <gadgetlib/protoboard.hpp>
#include <TinyRAMtoBair/RamToContraintSystem/ALU.hpp>
#include <TinyRAM/TinyRAMDefinitions.hpp>
#include <TinyRAM/TinyRAMInstance.hpp>

#define ROUNDS 10
#define BitsPerFElem Algebra::ExtensionDegree 

using namespace gadgetlib;

namespace {
	TEST(ALU, XOR_TEST){
		// Initialize pb and gadget parameters
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr xorGadget = ALU_XOR_Gadget::create(pb, aluInput, aluOutput);
		xorGadget->generateConstraints();
		pb->val(aluInput.arg1_val_) = Algebra::one();
		pb->val(aluInput.arg2_val_) = Algebra::one();
		xorGadget->generateWitness();
		EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::one());
		EXPECT_TRUE(pb->isSatisfied(Opcode::XOR));
		pb->val(aluOutput.flag_) = Algebra::zero();
		EXPECT_FALSE(pb->isSatisfied(Opcode::XOR));
		FElem x = Algebra::one();
		FElem y = Algebra::one();
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = x;
				pb->val(aluInput.arg2_val_) = y;
				xorGadget->generateWitness();
				EXPECT_EQ(pb->val(aluOutput.result_), x + y);
				y *= generator;
			}
			x *= generator;
		}
	}

	TEST(ALU, AND_TEST){
		// Initialize pb and gadget parameters
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr andGadget = ALU_AND_Gadget::create(pb, aluInput, aluOutput);
		andGadget->generateConstraints();
		// Initialize algebra variables
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		// Start Checking
		pb->val(aluInput.arg1_val_) = Algebra::one();
		pb->val(aluInput.arg2_val_) = generator;
		andGadget->generateWitness();
		EXPECT_EQ(pb->val(aluOutput.result_), Algebra::zero());
		EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::one());
		EXPECT_TRUE(pb->isSatisfied(Opcode::AND));
		pb->val(aluOutput.result_) = Algebra::one();
		EXPECT_FALSE(pb->isSatisfied(Opcode::AND));
		pb->val(aluOutput.result_) = Algebra::zero();
		pb->val(aluOutput.flag_) = Algebra::zero();
		EXPECT_FALSE(pb->isSatisfied(Opcode::AND));

		// Checking that values of aluOutput.result aluOutput.flag 
		FElem x = Algebra::one();
		for (int j = 0; j < ROUNDS; j++){
			FElem y = Algebra::one();
			for (int i = 0; i < ROUNDS; i++){
				pb->val(aluInput.arg1_val_) = x;
				pb->val(aluInput.arg2_val_) = y;
				andGadget->generateWitness();
				int xAsInt = mapFieldElementToInteger(0, trRegisterLen, x);
				int yAsInt = mapFieldElementToInteger(0, trRegisterLen, y);
				FElem resultShouldBe = mapIntegerToFieldElement(0, trRegisterLen, xAsInt & yAsInt);
				if (xAsInt & yAsInt){
					EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::zero());
				}
				else{
					EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::one());
				}
				EXPECT_EQ(pb->val(aluOutput.result_), resultShouldBe);
				EXPECT_NE(pb->val(aluOutput.result_), resultShouldBe + Algebra::one());
				y *= generator;
			}
			x *= generator;
		}
	}

	TEST(ALU, OR_TEST){
		// Initialize the parameters
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr orGadget = ALU_OR_Gadget::create(pb, aluInput, aluOutput);
		// Initialize algebra variables
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());

		orGadget->generateConstraints();
		pb->val(aluInput.arg1_val_) = Algebra::zero();
		pb->val(aluInput.arg2_val_) = Algebra::zero();
		orGadget->generateWitness();
		EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::one());
		EXPECT_EQ(pb->val(aluOutput.result_), Algebra::zero());
		EXPECT_TRUE(pb->isSatisfied(Opcode::OR));
		pb->val(aluOutput.flag_) = Algebra::zero();
		EXPECT_FALSE(pb->isSatisfied(Opcode::OR));

		// Checking that values of aluOutput.result aluOutput.flag 
		FElem x = Algebra::one();
		for (int j = 0; j < ROUNDS; j++){
			FElem y = Algebra::one();
			for (int i = 0; i < ROUNDS; i++){
				pb->val(aluInput.arg1_val_) = x;
				pb->val(aluInput.arg2_val_) = y;
				orGadget->generateWitness();
				int xAsInt = mapFieldElementToInteger(0, trRegisterLen, x);
				int yAsInt = mapFieldElementToInteger(0, trRegisterLen, y);
				FElem resultShouldBe = mapIntegerToFieldElement(0, trRegisterLen, xAsInt | yAsInt);
				if (xAsInt | yAsInt){
					EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::zero());
				}
				else{
					EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::one());
				}
				EXPECT_EQ(pb->val(aluOutput.result_), resultShouldBe);
				EXPECT_NE(pb->val(aluOutput.result_), resultShouldBe + Algebra::one());
				y *= generator;
			}
			x *= generator;
		}
	}

	TEST(ALU, NOT_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		GadgetPtr notGadget = ALU_NOT_Gadget::create(pb, aluInput, aluOutput);
		notGadget->generateConstraints();
		const Algebra::FElem x = Algebra::FElem(getGF2E_X());
		Algebra::FElem x_i = Algebra::one(); // will hold x^i
		Algebra::FElem allOnes = Algebra::zero();
		for (int i = 0; i < trRegisterLen; i++){
			allOnes += x_i;
			x_i *= x;
		}
		pb->val(aluInput.arg1_val_) = allOnes;
		notGadget->generateWitness();
		EXPECT_EQ(pb->val(aluOutput.result_), Algebra::zero());
		EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::one());
		Algebra::FElem y = Algebra::one();
		for (int i = 0; i < ROUNDS; ++i){
			pb->val(aluInput.arg1_val_) = y;
			notGadget->generateWitness();
			EXPECT_TRUE(pb->isSatisfied(Opcode::NOT));
			EXPECT_EQ(pb->val(aluOutput.result_), allOnes - y);
			y *= x;
		}
	}

	TEST(ALU, ADD_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr addGadget = ALU_ADD_Gadget::create(pb, aluInput, aluOutput);
		addGadget->generateConstraints();
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				addGadget->generateWitness();
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, i + j);
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::zero());
				EXPECT_TRUE(pb->isSatisfied(Opcode::ADD));
			}

		}
		const Algebra::FElem x = Algebra::FElem(getGF2E_X());
		Algebra::FElem x_i = Algebra::one(); // will hold x^i
		Algebra::FElem allOnes = Algebra::zero();
		for (int i = 0; i < trRegisterLen; i++){
			allOnes += x_i;
			x_i *= x;
		}
		pb->val(aluInput.arg1_val_) = allOnes;
		pb->val(aluInput.arg2_val_) = Algebra::one();
		addGadget->generateWitness();
		EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::one());
		EXPECT_TRUE(pb->isSatisfied(Opcode::ADD));
	}

	TEST(ALU, SUB_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr subGadget = ALU_SUB_Gadget::create(pb, aluInput, aluOutput);
		subGadget->generateConstraints();
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				subGadget->generateWitness();
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, /* 0x10000 + */ i - j);
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				EXPECT_EQ(pb->val(aluOutput.flag_), i>=j ? Algebra::zero() : Algebra::one());
				//std::cout << pb->val(aluOutput.flag_) << std::endl;
				EXPECT_TRUE(pb->isSatisfied(Opcode::SUB));
			}
		}
	}

	TEST(ALU, MULL_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr mullGadget = ALU_MULL_Gadget::create(pb, aluInput, aluOutput);
		mullGadget->generateConstraints();
		for (int i = 3000; i < 3000+ROUNDS; i++){ // result > 2^16
			for (int j = 3000; j < 3000+ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				mullGadget->generateWitness();
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, i*j);
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				EXPECT_EQ(pb->val(aluOutput.flag_), (i*j)>>trRegisterLen ? Algebra::one() : Algebra::zero());
				//std::cout << "R=" << i*j << std::endl;
				EXPECT_TRUE(pb->isSatisfied(Opcode::MULL));
			}
		}
	}

	TEST(ALU, UMULH_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr umulhGadget = ALU_UMULH_Gadget::create(pb, aluInput, aluOutput);
		umulhGadget->generateConstraints();
		for (int i = 3000; i < 3000+ROUNDS; i++){ // result > 2^16
			for (int j = 3000; j < 3000+ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				umulhGadget->generateWitness();
				const int v = (i*j) >> trRegisterLen;
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, v);
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				EXPECT_EQ(pb->val(aluOutput.flag_), v ? Algebra::one() : Algebra::zero());
				//std::cout << "v=" << v << " R=" << i*j << std::endl;
				EXPECT_TRUE(pb->isSatisfied(Opcode::UMULH));
			}
		}
	}

	TEST(ALU, SMULH_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr smulhGadget = ALU_SMULH_Gadget::create(pb, aluInput, aluOutput);
		smulhGadget->generateConstraints();
		for (int i = 300; i < 320; i++){ // result < -2^16
			for (int j = 200; j < 250; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, -j);
				smulhGadget->generateWitness();
				const int prod = (-i*j);
				const int v = prod >> trRegisterLen;
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, v);
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				EXPECT_EQ(pb->val(aluOutput.flag_), (prod>65535 || prod<-65536) ? Algebra::one() : Algebra::zero());
				//std::cout << "v=" << v << " prod=" << prod << " f=" << (prod>65535||prod<-65536) << std::endl;
				EXPECT_TRUE(pb->isSatisfied(Opcode::SMULH));
			}
		}
	}

	TEST(ALU, UDIV_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr udivGadget = ALU_UDIV_Gadget::create(pb, aluInput, aluOutput);
		udivGadget->generateConstraints();
		for (int i = 0; i < 100+ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				udivGadget->generateWitness();
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, j ? i/j : 0);
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				EXPECT_EQ(pb->val(aluOutput.flag_), j ? Algebra::zero() : Algebra::one());
				//std::cout << "R=" << (j ? i/j : 0) << std::endl;
				EXPECT_TRUE(pb->isSatisfied(Opcode::MULL));
			}
		}
	}

	TEST(ALU, UMOD_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr umodGadget = ALU_UMOD_Gadget::create(pb, aluInput, aluOutput);
		umodGadget->generateConstraints();
		for (int i = 0; i < 100+ROUNDS; i++){
			for (int j = 0; j < 15+ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				umodGadget->generateWitness();
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, j ? i%j : 0);
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				EXPECT_EQ(pb->val(aluOutput.flag_), j ? Algebra::zero() : Algebra::one());
				//std::cout << "div=" << (j ? i/j : 0) << " mod=" << (j ? i%j : 0) << std::endl;
				EXPECT_TRUE(pb->isSatisfied(Opcode::MULL));
			}
		}
	}
	
	TEST(ALU, CMPE_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr cmpeGadget = ALU_CMPE_Gadget::create(pb, aluInput, aluOutput);
		cmpeGadget->generateConstraints();
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				cmpeGadget->generateWitness();
				FElem flag = i == j ? Algebra::one() : Algebra::zero();
				EXPECT_EQ(pb->val(aluOutput.flag_), flag);
				EXPECT_TRUE(pb->isSatisfied(Opcode::CMPE));
			}

		}
	}

	TEST(ALU, CMPA_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr cmpeGadget = ALU_CMPA_Gadget::create(pb, aluInput, aluOutput);
		cmpeGadget->generateConstraints();
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				cmpeGadget->generateWitness();
				FElem flag = i > j ? Algebra::one() : Algebra::zero();
				EXPECT_EQ(pb->val(aluOutput.flag_), flag);
				EXPECT_TRUE(pb->isSatisfied(Opcode::CMPA));
			}
		}
	}

	TEST(ALU, CMPAE_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr cmpeGadget = ALU_CMPAE_Gadget::create(pb, aluInput, aluOutput);
		cmpeGadget->generateConstraints();
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				cmpeGadget->generateWitness();
				FElem flag = i >= j ? Algebra::one() : Algebra::zero();
				EXPECT_EQ(pb->val(aluOutput.flag_), flag);
				EXPECT_TRUE(pb->isSatisfied(Opcode::CMPAE));
			}

		}
	}

	TEST(ALU, CMPG_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr cmpeGadget = ALU_CMPG_Gadget::create(pb, aluInput, aluOutput);
		cmpeGadget->generateConstraints();
		for (short i = -ROUNDS; i < ROUNDS; i++){
			for (short j = -ROUNDS; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				cmpeGadget->generateWitness();
				FElem flag = i > j ? Algebra::one() : Algebra::zero();
				//std::cout << i << " , " << j << " : " << (i > j) << std::endl;
				EXPECT_EQ(pb->val(aluOutput.flag_), flag);
				EXPECT_TRUE(pb->isSatisfied(Opcode::CMPG));
			}

		}
	}

	TEST(ALU, CMPGE_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr cmpeGadget = ALU_CMPGE_Gadget::create(pb, aluInput, aluOutput);
		cmpeGadget->generateConstraints();
		for (short i = -ROUNDS; i < ROUNDS; i++){
			for (short j = -ROUNDS; j < ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				cmpeGadget->generateWitness();
				FElem flag = i >= j ? Algebra::one() : Algebra::zero();
				//std::cout << i << " , " << j << " : " << (i >= j) << std::endl;
				EXPECT_EQ(pb->val(aluOutput.flag_), flag);
				EXPECT_TRUE(pb->isSatisfied(Opcode::CMPGE));
			}

		}
	}

	TEST(ALU, SHL_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr SHLGadget = ALU_SHL_Gadget::create(pb, aluInput, aluOutput);
		SHLGadget->generateConstraints();
		for (size_t i = 65000; i < 65000+ROUNDS; i++){
			for (size_t j = 0; j < 10*ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				SHLGadget->generateWitness();
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, ((j >= BitsPerFElem) ? 0 : (i << j)));
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				//EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::zero());
				EXPECT_TRUE(pb->isSatisfied(Opcode::SHL));
			}
		}
	}

	TEST(ALU, SHR_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		standAlone_ = true;
		GadgetPtr SHRGadget = ALU_SHR_Gadget::create(pb, aluInput, aluOutput);
		SHRGadget->generateConstraints();
		for (size_t i = 0; i < ROUNDS; i++){
			for (size_t j = 0; j < 10*ROUNDS; j++){
				pb->val(aluInput.arg1_val_) = mapIntegerToFieldElement(0, trRegisterLen, i);
				pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, trRegisterLen, j);
				SHRGadget->generateWitness();
				FElem res = mapIntegerToFieldElement(0, trRegisterLen, ((j >= BitsPerFElem) ? 0 : (i >> j)));
				EXPECT_EQ(pb->val(aluOutput.result_), res);
				//EXPECT_EQ(pb->val(aluOutput.flag_), Algebra::zero());
				EXPECT_TRUE(pb->isSatisfied(Opcode::SHR));
			}
		}
	}

	TEST(ALU, STOREW_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		GadgetPtr storeGadget = ALU_STOREW_Gadget::create(pb, aluInput, aluOutput);
		storeGadget->generateConstraints();
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				FElem address = mapIntegerToFieldElement(0, trRegisterLen, i);
				FElem value = mapIntegerToFieldElement(0, trRegisterLen, j);
				pb->val(aluInput.arg2_val_) = address;
				pb->val(aluInput.dest_val_) = value;
				storeGadget->generateWitness();
				EXPECT_EQ(pb->val(aluOutput.isMemOp_), Algebra::one());
				EXPECT_EQ(pb->val(aluOutput.isLoadOp_), Algebra::zero());
				EXPECT_EQ(pb->loadValue(address), value);
				EXPECT_NE(pb->loadValue(address), value + Algebra::one());
				EXPECT_TRUE(pb->isSatisfied(Opcode::STOREW));
			}
		}
	}


	TEST(ALU, LOADW_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		GadgetPtr loadGadget = ALU_LOADW_Gadget::create(pb, aluInput, aluOutput);
		loadGadget->generateConstraints();
		for (int i = 0; i < ROUNDS; i++){
			for (int j = 0; j < ROUNDS; j++){
				FElem address = mapIntegerToFieldElement(0, trRegisterLen, i);
				FElem value = mapIntegerToFieldElement(0, trRegisterLen, j);
				pb->storeValue(address, value);
				pb->val(aluInput.arg2_val_) = address;
				loadGadget->generateWitness();
				EXPECT_EQ(pb->val(aluOutput.isMemOp_), Algebra::one());
				EXPECT_EQ(pb->val(aluOutput.isLoadOp_), Algebra::one());
				EXPECT_EQ(pb->val(aluOutput.value_), value);
				EXPECT_NE(pb->val(aluOutput.value_), value + Algebra::one());
				EXPECT_TRUE(pb->isSatisfied(Opcode::LOADW));
			}
		}
	}

	TEST(ALU, ROM_TEST){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		ALUInput aluInput;
		ALUOutput aluOutput;
		GadgetPtr romGadget = ALU_RESERVED_OPCODE_24_Gadget::create(pb, aluInput, aluOutput);
		vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
		if (runtimeArgs.size() > 0)
			prngseed = stoi(runtimeArgs[0]);
		ROMSIZE = 3;
		romGadget->generateConstraints();
		srand(prngseed); rand();
		cout << "ROM: ";
		std::vector<int16_t> arr;
		for (unsigned int i = 0; i < ROMSIZE; i++) {
			arr.push_back(rand() - RAND_MAX / 2);
			cout << i << "=" << arr.back()  << " ";
		}
		cout << endl;
		std::vector<int16_t>::iterator it = arr.begin();
		for (unsigned int i = 0; i < ROMSIZE; i++){;
			pb->val(aluInput.arg2_val_) = mapIntegerToFieldElement(0, BitsPerFElem, i);
			romGadget->generateWitness();
			EXPECT_EQ(pb->val(aluOutput.result_), mapIntegerToFieldElement(0, REGISTER_LENGTH, *(it++)));
			EXPECT_TRUE(pb->isSatisfied(Opcode::RESERVED_OPCODE_24));
		}
	}

} //end namespace
