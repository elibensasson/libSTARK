#include <iostream>
#include <string>
#include <protocols/protocol.hpp>

#include "TinyRAMtoBair/ConstraintSystemToBair/cs2Bair.hpp"
#include "TinyRAMtoBair/RamToContraintSystem/ALU.hpp"

using std::cout;
using std::endl;
using std::string;
using std::stoul;
using std::shared_ptr;
using std::vector;
using std::move;

void printHelp(const string exeName){
    cout<<"Usage:"<<endl;
    cout<<"$>"<<exeName<<" <TinyRAM assembly file path> <trace length log_2>"<<endl;
    cout<<endl<<"Example:"<<endl;
    cout<<"$>"<<exeName<<" examples-tinyram/collatz.asm 10"<<std::endl;
    cout<<endl<<"The above execution results in execution of STARK simulation over the collatz program, using at most 1023 (which is 2^10-1) machine steps."<<endl;
    cout<<"In the simulation the Prover and verify interact, the Prover generates a proof and the verify verifies it."<<endl;
    cout<<"During the executions the specifications of generated BAIR and APR, measurements, and Verifiers decision, are printed to the standard output."<<endl;

}

libstark::BairInstance constructInstance(const TinyRAMProgram& prog, const unsigned int t){
    resetALU_GadgetGlobalState();
    shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(prog.archParams().numRegisters, trRegisterLen, trOpcodeLen, 16, 1));

    gadgetlib::ProtoboardPtr pb_instance = Protoboard::create(archParams_);
    cs2Bair cs2bair_instance(pb_instance, prog, int(gadgetlib::POW2(t) - 1), false);

    unique_ptr<cs2BairConstraints> cs2bairConstraints_(new cs2BairConstraints(cs2bair_instance));
    unique_ptr<cs2BairMemoryCS> cs2bairMemoryCS_(new cs2BairMemoryCS(cs2bair_instance));
    size_t numVars = (cs2bairConstraints_->numVars() / 2);
    return libstark::BairInstance(
            numVars,
            t,
            move(cs2bairConstraints_),
            move(cs2bairMemoryCS_),
            cs2bair_instance.getBoundaryConstraints(),
            vector<Algebra::FieldElement>(numVars,Algebra::zero()));
}

libstark::BairWitness constructWitness(const TinyRAMProgram& prog, const unsigned int t){
    resetALU_GadgetGlobalState();
    shared_ptr<const TinyRAMProtoboardParams> archParams_(make_shared<const TinyRAMProtoboardParams>(prog.archParams().numRegisters, trRegisterLen, trOpcodeLen, 16, 1));

    gadgetlib::ProtoboardPtr pb_witness = Protoboard::create(archParams_);
    cs2Bair cs2bair_witness(pb_witness, prog, int(gadgetlib::POW2(t) - 1), true);
    unique_ptr<cs2BairColoring> cs2bairColoring_(new cs2BairColoring(cs2bair_witness));
    unique_ptr<cs2BairMemory> cs2bairMemory_(new cs2BairMemory(cs2bair_witness));

    return libstark::BairWitness(move(cs2bairColoring_), move(cs2bairMemory_));
}

void execute(const string assemblyFile, const unsigned int t){
    initTinyRAMParamsFromEnvVariables();
	TinyRAMProgram program(assemblyFile, REGISTERS_NUMBER, trRegisterLen);
    program.addInstructionsFromFile(assemblyFile);

    const auto bairInstance = constructInstance(program,t);
    const auto bairWitness = constructWitness(program,t);
    libstark::Protocols::executeProtocol(bairInstance,bairWitness,false,false,true);
}

int main(int argc, char *argv[]) {
    if(argc != 3){
        printHelp(argv[0]);
        return 0;
    }

    string assemblyFile(argv[1]);
    unsigned int executionLenLog(stoul(argv[2]));
    execute(assemblyFile,executionLenLog);

    return 0;
}
