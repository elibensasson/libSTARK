#include <gtest/gtest.h>
#include <algorithm>
#include "TinyRAMtoBair/RamToContraintSystem/generalPurpose.hpp"
#include "TinyRAM/TinyRAMDefinitions.hpp"
#include "TinyRAM/TinyRAMInstance.hpp"
#include <gadgetlib/common_use.hpp>



#define NUM_DEGREES 10

namespace{

	TEST(generalPurpose, memoryInfoCompare){
		initTinyRAMParamsFromEnvVariables();
		std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(trNumRegisters, trRegisterLen,
			trOpcodeLen, 16, 1));
		ProtoboardPtr pb = Protoboard::create(archParams_);
		
		// Add mapping between FieldElement and degree
		const Algebra::FElem g = Algebra::FElem(getGF2E_X());
		Algebra::FElem x_i = Algebra::one();
		for (unsigned int i = 0; i < NUM_DEGREES; i++){
			pb->addDegreeTranslation(x_i, i);
			x_i *= g;
		}
		
		//serialNumber = 1, isMemOp = true, isLoadOp = false, timestamp = 1, address = 1, value = 0
		MemoryInfo a(1, true, false, Algebra::one(), 0, Algebra::one(), Algebra::zero());
		//serialNumber = 2, isMemOp = false, isLoadOp = false, timestamp = 1, address = 1, value = 0
		MemoryInfo b(2, false, false, Algebra::one(), 0, Algebra::one(), Algebra::zero());
		// a < b - because a.isMemOp = true and b.isMemOp = false
		EXPECT_TRUE(sortMemoryInfo(a, b));
		EXPECT_FALSE(sortMemoryInfo(b, a));
		
		//a:  serialNumber = 1, isMemOp = true, isLoadOp = false, timestamp = 1, address = 1, value = 0
		//b:  serialNumber = 2, isMemOp = true, isLoadOp = false, timestamp = 1, address = g, value = 0
		b.updateIsMemOp(true);
		b.updateAddress(g);

		// a < b - because a's address is earlier
		EXPECT_TRUE(sortMemoryInfo(a, b));
		EXPECT_FALSE(sortMemoryInfo(b, a));
	
		//a:  serialNumber = 1, isMemOp = true, isLoadOp = false, timestamp = 1, address = g, value = 0
		//b:  serialNumber = 2, isMemOp = true, isLoadOp = false, timestamp = g, address = g, value = 0
		a.updateAddress(g);
		b.updateTimestamp(g,1);
		EXPECT_TRUE(sortMemoryInfo(a, b));
	}

	TEST(generalPurpose, memoryInfoSortVecor){
		const Algebra::FElem g = Algebra::FElem(getGF2E_X());
		FElem x_i = g;
		MemoryInfo a(1, false, false, Algebra::one(), 0, Algebra::one(), Algebra::zero());
		MemoryInfo b(2, true, false, g, 1, Algebra::one(), Algebra::zero());
		x_i *= g;
		MemoryInfo c(3, true, false, x_i, 2, Algebra::one(), Algebra::zero());
		x_i *= g;
		MemoryInfo d(4, true, false, x_i, 3, Algebra::one(), Algebra::zero());
		x_i *= g;
		MemoryInfo e(5, true, false, x_i, 4, g, Algebra::zero());
		x_i *= g;
		MemoryInfo f(6, true, false, x_i, 5, g, Algebra::zero());
		vector<MemoryInfo> memoryInfoToSort = { a, c, b, e, f, d };
		std::sort(memoryInfoToSort.begin(), memoryInfoToSort.end(), sortMemoryInfo);
		EXPECT_EQ(memoryInfoToSort[5], a);
		EXPECT_EQ(memoryInfoToSort[0], b);
		EXPECT_EQ(memoryInfoToSort[1], c);
		EXPECT_EQ(memoryInfoToSort[2], d);
		EXPECT_EQ(memoryInfoToSort[3], e);
		EXPECT_EQ(memoryInfoToSort[4], f);
	}

	TEST(generalPurpose, checkPCLengthValuesFromTinyRAMProgram){
		TinyRAMProgram program("program", trNumRegisters, trRegisterLen);
		MachineInstruction instruction1(Opcode::ADD, true, 1, 1, 1); //r1 = r1 + 1;
		program.addInstruction(instruction1);
		EXPECT_EQ(program.size(), 1);
		EXPECT_EQ(program.pcLength(), 1);
		program.addInstruction(instruction1);
		EXPECT_EQ(program.size(), 2);
		EXPECT_EQ(program.pcLength(), 1);
		program.addInstruction(instruction1);
		EXPECT_EQ(program.size(), 3);
		EXPECT_EQ(program.pcLength(), 2);
		program.addInstruction(instruction1);
		EXPECT_EQ(program.size(), 4);
		EXPECT_EQ(program.pcLength(), 2);
	
	}


}
