#include <gtest/gtest.h>
#include <gadgetlib/protoboard.hpp>
#include <fstream>
#include <TinyRAM/TinyRAMDefinitions.hpp>
#include <TinyRAM/TinyRAMInstance.hpp>

#include <TinyRAMtoBair/ConstraintSystemToBair/cs2Bair.hpp>
#include <languages/Bair/BairInstance.hpp>
#include <languages/Bair/BairWitness.hpp>
#include <languages/Bair/BairWitnessChecker.hpp>
#include <languages/Acsp/AcspWitnessChecker.hpp>
#include <reductions/BairToAcsp/BairToAcsp.hpp>
#include <TinyRAMtoBair/RamToContraintSystem/ALU.hpp> //for prngseed, remove later
#include "protocols/Ali/verifier.hpp"
#include "protocols/Ali/prover.hpp"

#define EXTDIM 64

using namespace gadgetlib;


namespace{

    const unsigned short securityParameter = 60;
	//static const unsigned short TRANSCRIPT_LEN_LOG = 7;


#if 0
	TinyRAMProgram getProgram(unsigned int programNumber, int aux_param){
		TinyRAMProgram program("program", trNumRegisters, trRegisterLen);
		if (programNumber == 0){
			MachineInstruction instruction1(Opcode::XOR, false, 2, 1, 1);
			program.addInstruction(instruction1);
			MachineInstruction instruction2(Opcode::STOREW, true, 2, 3, 3);
			program.addInstruction(instruction2);
			MachineInstruction instruction3(Opcode::AND, false, 3, 2, 2);
			program.addInstruction(instruction3);
			MachineInstruction instruction4(Opcode::AND, false, 3, 2, 2);
			program.addInstruction(instruction4);
			MachineInstruction instruction5(Opcode::AND, false, 3, 2, 2);
			program.addInstruction(instruction5);
			MachineInstruction instruction6(Opcode::AND, false, 3, 2, 2);
			program.addInstruction(instruction6);
			MachineInstruction instruction7(Opcode::AND, false, 3, 2, 2);
			program.addInstruction(instruction7);
		}
		else if (programNumber == 1){
			MachineInstruction instruction1(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction1);
			MachineInstruction instruction2(Opcode::JMP, true, 0, 0, 0);
			program.addInstruction(instruction2);
		}
		else if (programNumber == 2){
			MachineInstruction instruction1(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction1);
			MachineInstruction instruction2(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction2);
			MachineInstruction instruction3(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction3);
			MachineInstruction instruction4(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction4);
			MachineInstruction instruction5(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction5);
			MachineInstruction instruction6(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction6);
			MachineInstruction instruction7(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction7);
		}
		else if (programNumber == 3){
			MachineInstruction instruction1(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction1);
			MachineInstruction instruction2(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction2);
			MachineInstruction instruction3(Opcode::STOREW, true, 1, 0, 3); // store r[1] in address 3(as FElem)
			program.addInstruction(instruction3);
			MachineInstruction instruction4(Opcode::LOADW, true, 2, 0, 3); // load from address 3(as FElem) to r2
			program.addInstruction(instruction4);
			MachineInstruction instruction5(Opcode::ADD, true, 1, 1, 1);
			program.addInstruction(instruction5);
			MachineInstruction instruction6(Opcode::STOREW, true, 1, 0, 5); // store r[1] in address 3(as FElem)
			program.addInstruction(instruction6);
			MachineInstruction instruction7(Opcode::ANSWER, true, 0, 0, 0);
			program.addInstruction(instruction7);
		}
		return program;
	}
#endif

void testProg(const TinyRAMProgram & prog, std::function<void(gadgetlib::ProtoboardPtr)> initWitnessMem, const int transcript_len_log = 3) {
    resetALU_GadgetGlobalState();
    vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
	int flags = 1|4;

	//The following code is in a separate block so the memory it allocates
	//won't pass on to the PCP proof constructions phase.
	//Any case we get here some (lots of) memory that is passed on and I'm not sure why (Michael)
    // Init PB
    initTinyRAMParamsFromEnvVariables();
    std::shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(prog.archParams().numRegisters, trRegisterLen,
                trOpcodeLen, 16, 1));
    //
    // Reduce TinyRAM to Bair instance
    //
    gadgetlib::ProtoboardPtr pb_instance = Protoboard::create(archParams_);
    // Init cs2Bair
    cs2Bair cs2bair_instance(pb_instance, prog, int(gadgetlib::POW2(transcript_len_log) - 1), false);
    //

	unique_ptr<cs2BairConstraints> cs2bairConstraints_(new cs2BairConstraints(cs2bair_instance));
    unique_ptr<cs2BairMemoryCS> cs2bairMemoryCS_(new cs2BairMemoryCS(cs2bair_instance));
    //std::cout << "Number of Vars: " << pb_instance->getTranslationVector().size() << std::endl;
    size_t numVars = (cs2bairConstraints_->numVars() / 2);
    // Reduction
    const libstark::BairInstance instance(
            numVars,
            transcript_len_log, //3 /*TRANSCRIPT_LEN_LOG d: 2^d -1 = T*/,
            move(cs2bairConstraints_),
            move(cs2bairMemoryCS_),
            cs2bair_instance.getBoundaryConstraints(),
            std::vector<Algebra::FieldElement>(numVars,Algebra::zero()));
    const bool verifierOnly = 8 & flags;
    if(verifierOnly){
        libstark::Protocols::simulateProtocol(instance,securityParameter);
        return;
    }
    {
        //
        // Reduce TinyRAM to Bair witness
        //
        gadgetlib::ProtoboardPtr pb_witness = Protoboard::create(archParams_); //replace with pb_instance ?
        if (initWitnessMem != nullptr) initWitnessMem(pb_witness);
        // Init cs2Bair
        cs2Bair cs2bair_witness(pb_witness, prog, int(gadgetlib::POW2(transcript_len_log) - 1), true);
        unique_ptr<cs2BairColoring> cs2bairColoring_(new cs2BairColoring(cs2bair_witness));
        unique_ptr<cs2BairMemory> cs2bairMemory_(new cs2BairMemory(cs2bair_witness));
        
        // create Bair witness
        const libstark::BairWitness witness(move(cs2bairColoring_), move(cs2bairMemory_));
        
        const bool testBair = 1 & flags;
        const bool testAcsp = 2 & flags;
        const bool testPCP = 4 & flags;
        EXPECT_TRUE(libstark::Protocols::executeProtocol(instance,witness,securityParameter,testBair,testAcsp,testPCP));
    }
}
	//example of run:
	//PCP --gtest --gtest_filter=cs2Bair.Collatz --rand-args "9 4 3"
	//That's for 2^t= 2^9 (length of execution trace), run pcp=4, collatz_sequence_start=3
	//Faster run: PCP.exe --gtest --gtest_filter=cs2Bair.Collatz --rand-args "3 4 1"
	//To only run TinyRam To Constraint system reduction use 1 as second parameter rather than 4

TEST(cs2Bair, Collatz){
	initTinyRAMParamsFromEnvVariables();
    vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
	int aux_param = 3;
	if (runtimeArgs.size() >= 3){
		aux_param = stoi(runtimeArgs[2]);
	}

	TinyRAMProgram program("3n+1", REGISTERS_NUMBER, trRegisterLen);
	MachineInstruction instruction0(Opcode::MOV, true, 8, 0, aux_param);
    program.addInstruction(instruction0);
	MachineInstruction instruction1(Opcode::CMPE, true, 0, 8, 1);
	program.addInstruction(instruction1);
	MachineInstruction instruction2(Opcode::CJMP, true, 0, 0, 12);
	program.addInstruction(instruction2);
	MachineInstruction instruction3(Opcode::ADD, true, 9, 9, 1);
	program.addInstruction(instruction3);
	MachineInstruction instruction4(Opcode::AND, true, 7, 8, 1);
	program.addInstruction(instruction4);
	MachineInstruction instruction5(Opcode::CMPE, true, 0, 7, 0);
	program.addInstruction(instruction5);
	MachineInstruction instruction6(Opcode::CJMP, true, 0, 0, 10);
	program.addInstruction(instruction6);
	MachineInstruction instruction7(Opcode::SHL, true, 7, 8, 1);
	program.addInstruction(instruction7);
	MachineInstruction instruction8(Opcode::ADD, false, 7, 7, 8);
	program.addInstruction(instruction8);
	MachineInstruction instruction9(Opcode::ADD, true, 8, 7, 1);
	program.addInstruction(instruction9);
	MachineInstruction instruction10(Opcode::SHR, true, 8, 8, 1);
	program.addInstruction(instruction10);
	MachineInstruction instruction11(Opcode::JMP, true, 0, 0, 1);
	program.addInstruction(instruction11);
	MachineInstruction instruction12(Opcode::ANSWER, false, 0, 0, 9);
	program.addInstruction(instruction12);
    testProg(program, nullptr);
}
//example of run: pcp --gtest --gtest_filter=cs2Bair.coNPsubsetsum --rand-args "7 4 123 2 99"
	
	TEST(cs2Bair, coNPsubsetsum){
		vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
		int target_sum = 0, how_many = 3;
		if (runtimeArgs.size() >= 5){
			prngseed = stoi(runtimeArgs[2]);
			how_many = stoi(runtimeArgs[3]);
			target_sum = stoi(runtimeArgs[4]);
		}

		srand(prngseed); rand();
		cout << "ROM: ";
		for (unsigned int i = 0; i < ROMSIZE; i++) {
			cout << i << "=" << (int16_t)(rand() - RAND_MAX / 2) << " ";
		}
		cout << endl;

		TinyRAMProgram program("coNPsubsetsum", 5, trRegisterLen);
		MachineInstruction instruction0(Opcode::MOV, true, 0, 0, 1);
		program.addInstruction(instruction0);
		/*label=L1*/MachineInstruction instruction1(Opcode::CMPE, true, 0, 0, (1 << how_many) & 0xffff);
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::CJMP, true, 0, 0, 20 /*L5*/);
		program.addInstruction(instruction2);
		MachineInstruction instruction3(Opcode::MOV, true, 1, 0, 0);
		program.addInstruction(instruction3);
		MachineInstruction instruction4(Opcode::MOV, false, 2, 0, 0);
		program.addInstruction(instruction4);
		MachineInstruction instruction5(Opcode::MOV, true, 3, 0, 0);
		program.addInstruction(instruction5);
		/*label=L2*/MachineInstruction instruction6(Opcode::AND, true, 4, 2, 1);
		program.addInstruction(instruction6);
		MachineInstruction instruction7(Opcode::CMPE, true, 0, 4, 0);
		program.addInstruction(instruction7);
		MachineInstruction instruction8(Opcode::CJMP, true, 0, 0, 11 /*L3*/);
		program.addInstruction(instruction8);
		MachineInstruction instruction9(Opcode::RESERVED_OPCODE_24, false, 4, 0, 3);
		program.addInstruction(instruction9);
		MachineInstruction instruction10(Opcode::ADD, false, 1, 1, 4);
		program.addInstruction(instruction10);
		/*label=L3*/MachineInstruction instruction11(Opcode::SHR, true, 2, 2, 1);
		program.addInstruction(instruction11);
		MachineInstruction instruction12(Opcode::CMPE, true, 0, 2, 0);
		program.addInstruction(instruction12);
		MachineInstruction instruction13(Opcode::CJMP, true, 0, 0, 16 /*L4*/);
		program.addInstruction(instruction13);
		MachineInstruction instruction14(Opcode::ADD, true, 3, 3, 1);
		program.addInstruction(instruction14);
		MachineInstruction instruction15(Opcode::JMP, true, 0, 0, 6 /*L2*/);
		program.addInstruction(instruction15);
		/*label=L4*/MachineInstruction instruction16(Opcode::CMPE, true, 0, 1, target_sum);
		program.addInstruction(instruction16);
		MachineInstruction instruction17(Opcode::CJMP, true, 0, 0, 21 /*L6*/);
		program.addInstruction(instruction17);
		MachineInstruction instruction18(Opcode::ADD, true, 0, 0, 1);
		program.addInstruction(instruction18);
		MachineInstruction instruction19(Opcode::JMP, true, 0, 0, 1 /*L1*/);
		program.addInstruction(instruction19);
		/*label=L5*/MachineInstruction instruction20(Opcode::MOV, true, 0, 0, 0);
		program.addInstruction(instruction20);
		/*label=L6*/MachineInstruction instruction21(Opcode::ANSWER, false, 0, 0, 0);
		program.addInstruction(instruction21);

		testProg(program, nullptr);
	}

#define MEMssuminpaddr 65400
#define MEMssumoffset 32768
#define MEMssumbaseaddr1 0
#define MEMssumbaseaddr2 16384
	TEST(cs2Bair, MEMcoNPsubsetsum){
		vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
		int target_sum = 0, half = 1, cut = 0;
		unsigned int seed = 123;
		if (runtimeArgs.size() >= 6){
			seed = stoi(runtimeArgs[2]);
			half = stoi(runtimeArgs[3]);
			cut = stoi(runtimeArgs[4]);
			target_sum = stoi(runtimeArgs[5]);
		}

		srand(seed); rand();
		cout << "nums: ";
		for (int i = 0; i < 2 * half; i++) {
			cout << i << "=" << (int16_t)((rand() >> cut) - (RAND_MAX >> (cut + 1))) << " ";
		}
		cout << endl;

		TinyRAMProgram p("MEMcoNPsubsetsum", 10, trRegisterLen);
		//std::vector< MachineInstruction* > m(4*half + 76); //no need, emplace_back?

		srand(seed); rand();
		for (int i = 0; i < 2 * half; i++) {
			//m.push_back(new MachineInstruction(Opcode::MOV, true, 9, 0, (int16_t)(rand() - RAND_MAX / 2))); p.addInstruction(*(m.back()));
			//m.push_back(new MachineInstruction(Opcode::STOREW, true, 9, 0, MEMssuminpaddr + i)); p.addInstruction(*(m.back()));
			p.addInstruction(MachineInstruction(Opcode::MOV, true, 9, 0, (uint16_t)((rand() >> cut) - (RAND_MAX >> (cut + 1)))));
			p.addInstruction(MachineInstruction(Opcode::STOREW, true, 9, 0, MEMssuminpaddr + i));
		}
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 0, 0, MEMssuminpaddr));
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 1, 0, MEMssumbaseaddr1)); //sort first half
		/*L1:PrepareHalf*/p.addInstruction(MachineInstruction(Opcode::MOV, true, 9, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::STOREW, false, 9, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 2, 1, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::STOREW, false, 9, 0, 2));
		p.addInstruction(MachineInstruction(Opcode::MOV, false, 2, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 4, 1, 1));
		p.addInstruction(MachineInstruction(Opcode::MOV, false, 5, 0, 4));
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 8, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 9, 0, half));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 43 + 4 * half /*L5*/));
		/*L2:MergeIteration*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 0, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 9, 0));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 59 + 4 * half /*L7*/));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::SHL, true, 8, 8, 1));
		p.addInstruction(MachineInstruction(Opcode::MOV, false, 5, 0, 4));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 43 + 4 * half /*L5*/));
		/*L3:MergeCompare*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 7, 4, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::STOREW, false, 6, 0, 7));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 4, 4, 1));
		p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 5, 1));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 35 + 4 * half /*L4*/));
		p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 5, 2));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 13 + 4 * half /*L2*/));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 2));
		p.addInstruction(MachineInstruction(Opcode::ADD, false, 6, 6, 3));
		p.addInstruction(MachineInstruction(Opcode::STOREW, false, 6, 0, 4));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 6, 2, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 6));
		p.addInstruction(MachineInstruction(Opcode::XOR, false, 6, 6, 8));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 2, 2, 1));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 20 + 4 * half /*L3*/));
		/*L4:MergeOne*/p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 5, 2));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 43 + 4 * half /*L5*/));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::STOREW, false, 6, 0, 4));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 6, 1, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 6));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 1, 1));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 20 + 4 * half /*L3*/));
		/*L5:MergeBoth*/p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 7, 0, 2));
		p.addInstruction(MachineInstruction(Opcode::ADD, false, 7, 7, 3));
		p.addInstruction(MachineInstruction(Opcode::CMPG, false, 0, 6, 7));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 53 + 4 * half /*L6*/));
		p.addInstruction(MachineInstruction(Opcode::STOREW, false, 6, 0, 4));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 6, 1, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 6));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 1, 1));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 20 + 4 * half /*L3*/));
		/*L6:MergeOther*/p.addInstruction(MachineInstruction(Opcode::STOREW, false, 7, 0, 4));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 6, 2, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 6));
		p.addInstruction(MachineInstruction(Opcode::XOR, false, 6, 6, 8));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 2, 2, 1));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 20 + 4 * half /*L3*/));
		/*L7:EndOfPrepareHalf*/p.addInstruction(MachineInstruction(Opcode::CMPA, true, 0, 1, MEMssumbaseaddr2));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 63 + 4 * half /*L8*/));
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 1, 0, MEMssumbaseaddr2)); //sort second half
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 2 + 4 * half /*L1*/));
		/*L8:InitSearch*/p.addInstruction(MachineInstruction(Opcode::MOV, true, 0, 0, MEMssumbaseaddr1 + (1 << (half + 1)) - 2));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 2, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 1));
		/*L9:SearchLoop*/p.addInstruction(MachineInstruction(Opcode::ADD, false, 4, 2, 3));
		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 4, target_sum));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 82 + 4 * half /*L12*/));
		p.addInstruction(MachineInstruction(Opcode::CMPG, true, 0, 4, target_sum));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 76 + 4 * half /*L10*/));
		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 1, MEMssumbaseaddr2 + (1 << (half + 1)) - 2));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 81 + 4 * half /*L11*/));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 1, 1));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 66 + 4 * half /*L9*/));
		/*L10:SearchOther*/p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 0, MEMssumbaseaddr1));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 81 + 4 * half /*L11*/));
		p.addInstruction(MachineInstruction(Opcode::SUB, true, 0, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 2, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 66 + 4 * half /*L9*/));
		/*L11:ReturnFalse*/p.addInstruction(MachineInstruction(Opcode::ANSWER, true, 0, 0, 0));
		/*L12:ReturnTrue*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 2, 0, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 2, 0, 2));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 3, 1, MEMssumoffset));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 3));
		p.addInstruction(MachineInstruction(Opcode::SHL, true, 3, 3, half));
		p.addInstruction(MachineInstruction(Opcode::XOR, false, 2, 2, 3));
		p.addInstruction(MachineInstruction(Opcode::ANSWER, false, 0, 0, 2));
		testProg(p, nullptr);
	}

#if 0
	TEST(cs2Bair, sharpPsubsetsum){
		vector<string> runtimeArgs = ::Configuration::getInstance().getRandomArgs();
		int target_sum = 0, how_many = 8;
		if (runtimeArgs.size() >= 7){
			prngseed = stoi(runtimeArgs[4]);
			how_many = stoi(runtimeArgs[5]);
			target_sum = stoi(runtimeArgs[6]);
		}

		srand(prngseed); rand();
		cout << "ROM: ";
		for (int i = 0; i < ROMSIZE; i++) {
			cout << i << "=" << (int16_t)(rand() - RAND_MAX / 2) << " ";
		}
		cout << endl;

		gadgetlib::TinyRAMProgram program("sharpPsubsetsum", REGISTERS_NUMBER, trRegisterLen);
		MachineInstruction instruction0(Opcode::MOV, true, 8, 0, 0);
		program.addInstruction(instruction0);
		MachineInstruction instruction1(Opcode::MOV, true, 7, 0, target_sum);
		program.addInstruction(instruction1);
		MachineInstruction instruction2(Opcode::MOV, false, 6, 0, 7);
		program.addInstruction(instruction2);
		MachineInstruction instruction3(Opcode::MOV, true, 0, 0, 0);
		program.addInstruction(instruction3);
		/*label=L1*/MachineInstruction instruction4(Opcode::ADD, true, 0, 0, 1);
		program.addInstruction(instruction4);
		MachineInstruction instruction5(Opcode::CMPE, true, 0, 0, (1 << how_many) & 0xffff);
		program.addInstruction(instruction5);
		MachineInstruction instruction6(Opcode::CJMP, true, 0, 0, 28 /*L5*/);
		program.addInstruction(instruction6);
		MachineInstruction instruction7(Opcode::MOV, true, 1, 0, 0);
		program.addInstruction(instruction7);
		MachineInstruction instruction8(Opcode::MOV, false, 2, 0, 0);
		program.addInstruction(instruction8);
		MachineInstruction instruction9(Opcode::MOV, true, 3, 0, 0);
		program.addInstruction(instruction9);
		/*label=L2*/MachineInstruction instruction10(Opcode::AND, true, 4, 2, 1);
		program.addInstruction(instruction10);
		MachineInstruction instruction11(Opcode::CMPE, true, 0, 4, 0);
		program.addInstruction(instruction11);
		MachineInstruction instruction12(Opcode::CJMP, true, 0, 0, 15 /*L3*/);
		program.addInstruction(instruction12);
		MachineInstruction instruction13(Opcode::RESERVED_OPCODE_24, false, 4, 0, 3);
		program.addInstruction(instruction13);
		MachineInstruction instruction14(Opcode::ADD, false, 1, 1, 4);
		program.addInstruction(instruction14);
		/*label=L3*/MachineInstruction instruction15(Opcode::SHR, true, 2, 2, 1);
		program.addInstruction(instruction15);
		MachineInstruction instruction16(Opcode::CMPE, true, 0, 2, 0);
		program.addInstruction(instruction16);
		MachineInstruction instruction17(Opcode::CJMP, true, 0, 0, 20 /*L4*/);
		program.addInstruction(instruction17);
		MachineInstruction instruction18(Opcode::ADD, true, 3, 3, 1);
		program.addInstruction(instruction18);
		MachineInstruction instruction19(Opcode::JMP, true, 0, 0, 10 /*L2*/);
		program.addInstruction(instruction19);
		/*label=L4*/MachineInstruction instruction20(Opcode::CMPA, false, 0, 1, 7);
		program.addInstruction(instruction20);
		MachineInstruction instruction21(Opcode::CJMP, true, 0, 0, 4 /*L1*/);
		program.addInstruction(instruction21);
		MachineInstruction instruction22(Opcode::SUB, false, 4, 7, 1);
		program.addInstruction(instruction22);
		MachineInstruction instruction23(Opcode::CMPA, false, 0, 6, 4);
		program.addInstruction(instruction23);
		MachineInstruction instruction24(Opcode::CNJMP, true, 0, 0, 4 /*L1*/);
		program.addInstruction(instruction24);
		MachineInstruction instruction25(Opcode::MOV, false, 6, 0, 4);
		program.addInstruction(instruction25);
		MachineInstruction instruction26(Opcode::MOV, false, 8, 0, 0);
		program.addInstruction(instruction26);
		MachineInstruction instruction27(Opcode::JMP, true, 0, 0, 4 /*L1*/);
		program.addInstruction(instruction27);
		/*label=L5*/MachineInstruction instruction28(Opcode::ANSWER, false, 0, 0, 8);
		program.addInstruction(instruction28);

		testProg(program, nullptr);
	}
#endif

	void ssumMemFill(gadgetlib::ProtoboardPtr pb){
		vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
		const unsigned int seed = stoi(runtimeArgs[2]);
		const int half = stoi(runtimeArgs[3]);
		const int cut = stoi(runtimeArgs[4]);

		srand(seed); rand();

		for (int baseAddr = 0;;) {
			int size = 1;
			std::pair<int, int> * a = new std::pair<int, int>[size];
			a[0] = std::pair<int, int>(0, 0);
			for (int i = 0; i < half; ++i) {
				std::pair<int, int> * b = new std::pair<int, int>[size];
				int currinput = (rand() >> cut) - (RAND_MAX >> (cut + 1));
				for (int j = 0; j < size; ++j) {
					b[j].first = a[j].first + currinput;
					if (b[j].first >= (1 << 15) || b[j].first < -(1 << 15)) {
						std::cout << "OVERFLOW!" << std::endl;
						exit(1);
					}
					b[j].second = a[j].second | size;
				}
				std::pair<int, int> * c = new std::pair<int, int>[2 * size];
				for (int p = 0, q = 0, j = 0;; ++j) {
					if (p < size) {
						if (q < size) {
							if (a[p].first < b[q].first)
								c[j] = a[p++];
							else
								c[j] = b[q++];
							continue;
						}
						c[j] = a[p++];
						continue;
					}
					if (q < size) {
						c[j] = b[q++];
						continue;
					}
					break;
				}
				delete[] a;	delete[] b;
				a = c; size *= 2;
			}
			for (int i = 0; i < size; ++i) {
				pb->storeValue(mapIntegerToFieldElement(0, EXTDIM, baseAddr + i), mapIntegerToFieldElement(0, trRegisterLen, a[i].first)); //sorted sums
				pb->storeValue(mapIntegerToFieldElement(0, EXTDIM, baseAddr + size + i), mapIntegerToFieldElement(0, EXTDIM, a[i].second)); //permu
				pb->storeValue(mapIntegerToFieldElement(0, EXTDIM, baseAddr + 2 * size + a[i].second), mapIntegerToFieldElement(0, EXTDIM, i)); //inv permu
			}
			delete[] a;
			if (0 != baseAddr) break;
			baseAddr = 3 * size;
		}
		/*for (int i = 0; i < (1 << half) * 3 * 2; ++i){ //debug
		FElem v = pb->loadValue(mapIntegerToFieldElement(0, 64, i));
		cout << i << ": " << int16_t(mapFieldElementToInteger(0, 64, v)) << endl;
		}*/
	}

	TEST(cs2Bair, nondetMEMcoNPsubsetsum){
		vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
		if (runtimeArgs.size() < 6) return;
		const unsigned int seed = stoi(runtimeArgs[2]);
		const int half = stoi(runtimeArgs[3]);
		const int cut = stoi(runtimeArgs[4]);
		const int target_sum = stoi(runtimeArgs[5]);
		const int expo = 1 << half;

		srand(seed); rand();
		cout << "nums: ";
		for (int i = 0; i < 2 * half; i++) {
			cout << i << "=" << (int16_t)((rand() >> cut) - (RAND_MAX >> (cut + 1))) << " ";
		}
		cout << endl;

		TinyRAMProgram p("nondetMEMcoNPsubsetsum", REGISTERS_NUMBER, trRegisterLen);

		srand(seed); rand();
		for (int i = 0; i < 2 * half; i++) {
			p.addInstruction(MachineInstruction(Opcode::MOV, true, 1, 0, (int16_t)((rand() >> cut) - (RAND_MAX >> (cut + 1)))));
			p.addInstruction(MachineInstruction(Opcode::STOREW, true, 1, 0, MEMssuminpaddr + i));
		}

		p.addInstruction(MachineInstruction(Opcode::MOV, true, 0, 0, 0));

		/*L1:Verify*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 0, expo));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 1, 0, 1));

		p.addInstruction(MachineInstruction(Opcode::MOV, true, 2, 0, 0)); //verify sum
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 3, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 4, 0, 1));
		/*L2:VerifySumLoop*/p.addInstruction(MachineInstruction(Opcode::AND, false, 5, 1, 4));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 11 + 4 * half /*L3*/));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 5, 3, MEMssuminpaddr));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 5, 0, 5));
		p.addInstruction(MachineInstruction(Opcode::ADD, false, 2, 2, 5));
		/*L3:VerifySumEscape*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 3, 3, 1));
		p.addInstruction(MachineInstruction(Opcode::SHL, true, 4, 4, 1));
		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 3, half));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 6 + 4 * half /*L2*/));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 6, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 2, 6));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 75 + 4 * half /*L10*/));

		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 1, 2 * expo)); //verify permu
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 1, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 1, 0));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 75 + 4 * half /*L10*/));


		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 0, 4 * expo));  //2nd half
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 1, 0, 1));

		p.addInstruction(MachineInstruction(Opcode::MOV, true, 2, 0, 0));  //verify sum
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 3, 0, half));
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 4, 0, 1));
		/*L4:VerifySumLoop*/p.addInstruction(MachineInstruction(Opcode::AND, false, 5, 1, 4));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 32 + 4 * half /*L5*/));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 5, 3, MEMssuminpaddr));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 5, 0, 5));
		p.addInstruction(MachineInstruction(Opcode::ADD, false, 2, 2, 5));
		/*L5:VerifySumEscape*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 3, 3, 1));
		p.addInstruction(MachineInstruction(Opcode::SHL, true, 4, 4, 1));
		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 3, 2 * half));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 27 + 4 * half /*L4*/));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 5, 0, 3 * expo));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 5, 0, 5));
		p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 2, 5));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 75 + 4 * half /*L10*/));

		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 1, 5 * expo)); //verify permu
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 1, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::CMPE, false, 0, 1, 0));
		p.addInstruction(MachineInstruction(Opcode::CNJMP, true, 0, 0, 75 + 4 * half /*L10*/));


		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 0, 0)); //verify order
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 50 + 4 * half /*L6*/));
		p.addInstruction(MachineInstruction(Opcode::CMPG, false, 0, 7, 6));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 75 + 4 * half /*L10*/));
		p.addInstruction(MachineInstruction(Opcode::CMPG, false, 0, 8, 5));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 75 + 4 * half /*L10*/));

		/*L6:EscapeVerifyOrder*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 0, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 0, expo));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 56 + 4 * half /*L7*/));
		p.addInstruction(MachineInstruction(Opcode::MOV, false, 7, 0, 6));
		p.addInstruction(MachineInstruction(Opcode::MOV, false, 8, 0, 5));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 1 + 4 * half /*L1*/));


		/*L7:InitSearch*/p.addInstruction(MachineInstruction(Opcode::MOV, true, 0, 0, expo - 1));
		p.addInstruction(MachineInstruction(Opcode::MOV, true, 1, 0, 3 * expo));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 2, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 1));
		/*L8:SearchLoop*/p.addInstruction(MachineInstruction(Opcode::ADD, false, 4, 2, 3));
		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 4, target_sum));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 76 + 4 * half /*L11*/));
		p.addInstruction(MachineInstruction(Opcode::CMPG, true, 0, 4, target_sum));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 70 + 4 * half /*L9*/));
		p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 1, 4 * expo - 1));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 75 + 4 * half /*L10*/));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 1, 1));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 60 + 4 * half /*L8*/));
		/*L9:SearchOther*/p.addInstruction(MachineInstruction(Opcode::CMPE, true, 0, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::CJMP, true, 0, 0, 75 + 4 * half /*L10*/));
		p.addInstruction(MachineInstruction(Opcode::SUB, true, 0, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 2, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::JMP, true, 0, 0, 60 + 4 * half /*L8*/));
		/*L10:ReturnFalse*/p.addInstruction(MachineInstruction(Opcode::ANSWER, true, 0, 0, 0));
		/*L11:ReturnTrue*/p.addInstruction(MachineInstruction(Opcode::ADD, true, 0, 0, expo));
		p.addInstruction(MachineInstruction(Opcode::ADD, true, 1, 1, expo));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 2, 0, 0));
		p.addInstruction(MachineInstruction(Opcode::LOADW, false, 3, 0, 1));
		p.addInstruction(MachineInstruction(Opcode::SHL, true, 3, 3, half));
		p.addInstruction(MachineInstruction(Opcode::XOR, false, 2, 2, 3));
		p.addInstruction(MachineInstruction(Opcode::ANSWER, false, 0, 0, 2));
		testProg(p, ssumMemFill);
	}

	TEST(cs2Bair, ccompilerMEMcoNPsubsetsum){
		vector<string> runtimeArgs;// = ::Configuration::getInstance().getRandomArgs();
		int target_sum = 0, half = 7, cut = 0;
		unsigned int seed = 123;
		if (runtimeArgs.size() >= 6){
			seed = stoi(runtimeArgs[2]);
			//half = stoi(runtimeArgs[3]);
			cut = stoi(runtimeArgs[4]);
			target_sum = stoi(runtimeArgs[5]);
		}
		else return;

		srand(seed); rand();
		cout << "nums: ";
		for (int i = 0; i < 2 * half; i++) {
			cout << i << "=" << (int16_t)((rand() >> cut) - (RAND_MAX >> (cut + 1))) << " ";
		}
		cout << endl;

		TinyRAMProgram program("ccompilerMEMcoNPsubsetsum", REGISTERS_NUMBER, trRegisterLen);
		srand(seed); rand();
		for (int i = 0; i < 2 * half; i++) {
			program.addInstruction(MachineInstruction(Opcode::MOV, true, 9, 0, (int16_t)((rand() >> cut) - (RAND_MAX >> (cut + 1)))));
			program.addInstruction(MachineInstruction(Opcode::STOREW, true, 9, 0, 0 + 2 * i));
		}
		MachineInstruction instruction9(Opcode::MOV, true, 9, 0, 0);
		program.addInstruction(instruction9);
		MachineInstruction instruction10(Opcode::MOV, true, 12, 0, 28);
		program.addInstruction(instruction10);
		MachineInstruction instruction11(Opcode::MOV, false, 8, 0, 12);
		program.addInstruction(instruction11);
		MachineInstruction instruction12(Opcode::ADD, true, 13, 8, 4);
		program.addInstruction(instruction12);
		MachineInstruction instruction13(Opcode::MOV, false, 4, 0, 13);
		program.addInstruction(instruction13);
		MachineInstruction instruction14(Opcode::MOV, true, 2, 0, 0);
		program.addInstruction(instruction14);
		MachineInstruction instruction15(Opcode::ADD, true, 0, 8, 2);
		program.addInstruction(instruction15);
		MachineInstruction instruction16(Opcode::STOREW, false, 2, 0, 0);
		program.addInstruction(instruction16);
		MachineInstruction instruction17(Opcode::STOREW, false, 2, 0, 8);
		program.addInstruction(instruction17);
		MachineInstruction instruction18(Opcode::MOV, true, 14, 0, 1);
		program.addInstruction(instruction18);
		MachineInstruction instruction19(Opcode::ADD, true, 5, 9, 14);
		program.addInstruction(instruction19);
		MachineInstruction instruction20(Opcode::CMPE, false, 8, 8, 13);
		program.addInstruction(instruction20);
		MachineInstruction instruction21(Opcode::CNJMP, true, 0, 0, 38 + 4 * half - 9);
		program.addInstruction(instruction21);
		MachineInstruction instruction22(Opcode::CMPAE, false, 12, 12, 13);
		program.addInstruction(instruction22);
		MachineInstruction instruction23(Opcode::CJMP, true, 0, 0, 80 + 4 * half - 9);
		program.addInstruction(instruction23);
		MachineInstruction instruction24(Opcode::LOADW, false, 3, 0, 12);
		program.addInstruction(instruction24);
		MachineInstruction instruction25(Opcode::LOADW, false, 2, 0, 9);
		program.addInstruction(instruction25);
		MachineInstruction instruction26(Opcode::ADD, false, 2, 3, 2);
		program.addInstruction(instruction26);
		MachineInstruction instruction27(Opcode::ADD, true, 12, 12, 2);
		program.addInstruction(instruction27);
		MachineInstruction instruction28(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction28);
		MachineInstruction instruction29(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction29);
		MachineInstruction instruction30(Opcode::LOADW, false, 2, 0, 12);
		program.addInstruction(instruction30);
		MachineInstruction instruction31(Opcode::XOR, false, 2, 14, 2);
		program.addInstruction(instruction31);
		MachineInstruction instruction32(Opcode::ADD, true, 12, 12, 2);
		program.addInstruction(instruction32);
		MachineInstruction instruction33(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction33);
		MachineInstruction instruction34(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction34);
		MachineInstruction instruction35(Opcode::CMPAE, false, 12, 12, 13);
		program.addInstruction(instruction35);
		MachineInstruction instruction36(Opcode::CNJMP, true, 0, 0, 24 + 4 * half - 9);
		program.addInstruction(instruction36);
		MachineInstruction instruction37(Opcode::JMP, true, 0, 0, 80 + 4 * half - 9);
		program.addInstruction(instruction37);
		MachineInstruction instruction38(Opcode::CMPE, false, 12, 12, 13);
		program.addInstruction(instruction38);
		MachineInstruction instruction39(Opcode::CNJMP, true, 0, 0, 53 + 4 * half - 9);
		program.addInstruction(instruction39);
		MachineInstruction instruction40(Opcode::CMPAE, false, 8, 8, 13);
		program.addInstruction(instruction40);
		MachineInstruction instruction41(Opcode::CJMP, true, 0, 0, 80 + 4 * half - 9);
		program.addInstruction(instruction41);
		MachineInstruction instruction42(Opcode::LOADW, false, 2, 0, 8);
		program.addInstruction(instruction42);
		MachineInstruction instruction43(Opcode::ADD, true, 8, 8, 2);
		program.addInstruction(instruction43);
		MachineInstruction instruction44(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction44);
		MachineInstruction instruction45(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction45);
		MachineInstruction instruction46(Opcode::LOADW, false, 2, 0, 8);
		program.addInstruction(instruction46);
		MachineInstruction instruction47(Opcode::ADD, true, 8, 8, 2);
		program.addInstruction(instruction47);
		MachineInstruction instruction48(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction48);
		MachineInstruction instruction49(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction49);
		MachineInstruction instruction50(Opcode::CMPAE, false, 8, 8, 13);
		program.addInstruction(instruction50);
		MachineInstruction instruction51(Opcode::CNJMP, true, 0, 0, 42 + 4 * half - 9);
		program.addInstruction(instruction51);
		MachineInstruction instruction52(Opcode::JMP, true, 0, 0, 80 + 4 * half - 9);
		program.addInstruction(instruction52);
		MachineInstruction instruction53(Opcode::LOADW, false, 2, 0, 12);
		program.addInstruction(instruction53);
		MachineInstruction instruction54(Opcode::LOADW, false, 3, 0, 9);
		program.addInstruction(instruction54);
		MachineInstruction instruction55(Opcode::ADD, false, 3, 2, 3);
		program.addInstruction(instruction55);
		MachineInstruction instruction56(Opcode::LOADW, false, 2, 0, 8);
		program.addInstruction(instruction56);
		MachineInstruction instruction57(Opcode::CMPG, false, 2, 2, 3);
		program.addInstruction(instruction57);
		MachineInstruction instruction58(Opcode::CNJMP, true, 0, 0, 71 + 4 * half - 9);
		program.addInstruction(instruction58);
		MachineInstruction instruction59(Opcode::LOADW, false, 3, 0, 12);
		program.addInstruction(instruction59);
		MachineInstruction instruction60(Opcode::LOADW, false, 2, 0, 9);
		program.addInstruction(instruction60);
		MachineInstruction instruction61(Opcode::ADD, false, 2, 3, 2);
		program.addInstruction(instruction61);
		MachineInstruction instruction62(Opcode::ADD, true, 12, 12, 2);
		program.addInstruction(instruction62);
		MachineInstruction instruction63(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction63);
		MachineInstruction instruction64(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction64);
		MachineInstruction instruction65(Opcode::LOADW, false, 2, 0, 12);
		program.addInstruction(instruction65);
		MachineInstruction instruction66(Opcode::XOR, false, 2, 14, 2);
		program.addInstruction(instruction66);
		MachineInstruction instruction67(Opcode::ADD, true, 12, 12, 2);
		program.addInstruction(instruction67);
		MachineInstruction instruction68(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction68);
		MachineInstruction instruction69(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction69);
		MachineInstruction instruction70(Opcode::JMP, true, 0, 0, 20 + 4 * half - 9);
		program.addInstruction(instruction70);
		MachineInstruction instruction71(Opcode::LOADW, false, 2, 0, 8);
		program.addInstruction(instruction71);
		MachineInstruction instruction72(Opcode::ADD, true, 8, 8, 2);
		program.addInstruction(instruction72);
		MachineInstruction instruction73(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction73);
		MachineInstruction instruction74(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction74);
		MachineInstruction instruction75(Opcode::LOADW, false, 2, 0, 8);
		program.addInstruction(instruction75);
		MachineInstruction instruction76(Opcode::ADD, true, 8, 8, 2);
		program.addInstruction(instruction76);
		MachineInstruction instruction77(Opcode::STOREW, false, 2, 0, 4);
		program.addInstruction(instruction77);
		MachineInstruction instruction78(Opcode::ADD, true, 4, 4, 2);
		program.addInstruction(instruction78);
		MachineInstruction instruction79(Opcode::JMP, true, 0, 0, 20 + 4 * half - 9);
		program.addInstruction(instruction79);
		MachineInstruction instruction80(Opcode::ADD, true, 9, 9, 2);
		program.addInstruction(instruction80);
		MachineInstruction instruction81(Opcode::CMPE, false, 9, 9, 5);
		program.addInstruction(instruction81);
		MachineInstruction instruction82(Opcode::CJMP, true, 0, 0, 86 + 4 * half - 9);
		program.addInstruction(instruction82);
		MachineInstruction instruction83(Opcode::SHL, true, 14, 14, 1);
		program.addInstruction(instruction83);
		MachineInstruction instruction84(Opcode::MOV, false, 13, 0, 4);
		program.addInstruction(instruction84);
		MachineInstruction instruction85(Opcode::JMP, true, 0, 0, 20 + 4 * half - 9);
		program.addInstruction(instruction85);
		MachineInstruction instruction86(Opcode::CMPA, true, 8, 8, 1052);
		program.addInstruction(instruction86);
		MachineInstruction instruction87(Opcode::CJMP, true, 0, 0, 91 + 4 * half - 9);
		program.addInstruction(instruction87);
		MachineInstruction instruction88(Opcode::MOV, false, 12, 0, 4);
		program.addInstruction(instruction88);
		MachineInstruction instruction89(Opcode::MOV, false, 8, 0, 4);
		program.addInstruction(instruction89);
		MachineInstruction instruction90(Opcode::JMP, true, 0, 0, 12 + 4 * half - 9);
		program.addInstruction(instruction90);
		MachineInstruction instruction91(Opcode::MOV, true, 4, 0, 1044);
		program.addInstruction(instruction91);
		MachineInstruction instruction92(Opcode::LOADW, false, 3, 0, 4);
		program.addInstruction(instruction92);
		MachineInstruction instruction93(Opcode::LOADW, false, 2, 0, 12);
		program.addInstruction(instruction93);
		MachineInstruction instruction94(Opcode::ADD, false, 2, 3, 2);
		program.addInstruction(instruction94);
		MachineInstruction instruction95(Opcode::CMPE, true, 2, 2, target_sum);
		program.addInstruction(instruction95);
		MachineInstruction instruction96(Opcode::CNJMP, true, 0, 0, 104 + 4 * half - 9);
		program.addInstruction(instruction96);
		MachineInstruction instruction97(Opcode::ADD, true, 0, 12, 2);
		program.addInstruction(instruction97);
		MachineInstruction instruction98(Opcode::LOADW, false, 12, 0, 0);
		program.addInstruction(instruction98);
		MachineInstruction instruction99(Opcode::SHL, true, 12, 12, half);
		program.addInstruction(instruction99);
		MachineInstruction instruction100(Opcode::ADD, true, 0, 4, 2);
		program.addInstruction(instruction100);
		MachineInstruction instruction101(Opcode::LOADW, false, 4, 0, 0);
		program.addInstruction(instruction101);
		MachineInstruction instruction102(Opcode::XOR, false, 2, 12, 4);
		program.addInstruction(instruction102);
		MachineInstruction instruction103(Opcode::JMP, true, 0, 0, 119 + 4 * half - 9);
		program.addInstruction(instruction103);
		MachineInstruction instruction104(Opcode::LOADW, false, 3, 0, 4);
		program.addInstruction(instruction104);
		MachineInstruction instruction105(Opcode::LOADW, false, 2, 0, 12);
		program.addInstruction(instruction105);
		MachineInstruction instruction106(Opcode::ADD, false, 2, 3, 2);
		program.addInstruction(instruction106);
		MachineInstruction instruction107(Opcode::CMPG, true, 2, 2, target_sum - 1);
		program.addInstruction(instruction107);
		MachineInstruction instruction108(Opcode::CJMP, true, 0, 0, 114 + 4 * half - 9);
		program.addInstruction(instruction108);
		MachineInstruction instruction109(Opcode::MOV, true, 2, 0, 2064);
		program.addInstruction(instruction109);
		MachineInstruction instruction110(Opcode::CMPE, false, 12, 12, 2);
		program.addInstruction(instruction110);
		MachineInstruction instruction111(Opcode::CJMP, true, 0, 0, 118 + 4 * half - 9);
		program.addInstruction(instruction111);
		MachineInstruction instruction112(Opcode::ADD, true, 12, 12, 4);
		program.addInstruction(instruction112);
		MachineInstruction instruction113(Opcode::JMP, true, 0, 0, 92 + 4 * half - 9);
		program.addInstruction(instruction113);
		MachineInstruction instruction114(Opcode::CMPE, true, 4, 4, 28);
		program.addInstruction(instruction114);
		MachineInstruction instruction115(Opcode::CJMP, true, 0, 0, 118 + 4 * half - 9);
		program.addInstruction(instruction115);
		MachineInstruction instruction116(Opcode::ADD, true, 4, 4, 65532);
		program.addInstruction(instruction116);
		MachineInstruction instruction117(Opcode::JMP, true, 0, 0, 92 + 4 * half - 9);
		program.addInstruction(instruction117);
		MachineInstruction instruction118(Opcode::MOV, true, 2, 0, 0);
		program.addInstruction(instruction118);
		MachineInstruction instruction119(Opcode::ANSWER, false, 0, 0, 2);
		program.addInstruction(instruction119);
		testProg(program, nullptr);
	}
} // namespace
