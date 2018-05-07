#include <iostream>
#include <string>
#include <protocols/protocol.hpp>

#include "TinyRAMtoBair/ConstraintSystemToBair/cs2Bair.hpp"
#include "TinyRAMtoBair/RamToContraintSystem/ALU.hpp"

using std::cout;
using std::endl;
using std::to_string;
using std::string;
using std::stoul;
using std::shared_ptr;
using std::vector;
using std::move;

const string timePrefix = "-t";
const string securityPrefix = "-s";
const string noProverPrefix = "-p";

void printHelp(const string exeName){
    cout<<"Usage:"<<endl;
    cout<<"$>"<<exeName<<" <TinyRAM assembly file path> "<<timePrefix<<"<trace length log_2> ["<<securityPrefix<<"<security parameter]> ["<<noProverPrefix<<"<0,1>]"<<endl;
    cout<<endl<<"Example:"<<endl;
    cout<<"$>"<<exeName<<" examples-tinyram/collatz.asm "<<timePrefix<<"10 "<<securityPrefix<<"120"<<endl;
    cout<<endl<<"The above execution results in execution of STARK simulation over the collatz program, using at most 1023 (which is 2^10-1) machine steps, and soundness error at most 2^-120."<<endl;
    cout<<endl<<"In the simulation the Prover and Verify interact, the Prover generates a proof and the Verifier verifies it. During the executions the specifications of generated BAIR and APR, measurements, and Verifiers decision, are printed to the standard output."<<endl;
    cout<<"adding '-p0' to the arguments causes execution of verifier only, without the prover, simulating its execution time and measuring proof length"<<std::endl;
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

void execute(const string assemblyFile, const unsigned int t, const unsigned int securityParameter, const bool simulateOnly){
    cout<<"Executing simulation with assembly from " + assemblyFile + " over 2^" + to_string(t) +"-1 steps, and soundness error at most 2^-" +to_string(securityParameter)<<endl;
    
    //Initialize instance
    initTinyRAMParamsFromEnvVariables();
	TinyRAMProgram program(assemblyFile, REGISTERS_NUMBER, trRegisterLen);
    program.addInstructionsFromFile(assemblyFile);

    
    //simulation only - no prover
    if(simulateOnly){
        const auto bairInstance = constructInstance(program,t);
        libstark::Protocols::simulateProtocol(bairInstance, securityParameter);
        return;
    }
    
    //full execution 
    const auto bairWitness = constructWitness(program,t);
    const auto bairInstance = constructInstance(program,t);
    libstark::Protocols::executeProtocol(bairInstance,bairWitness,securityParameter,false,false,true);
}

int main(int argc, char *argv[]) {
    if(argc < 2){
        printHelp(argv[0]);
        return 0;
    }

    string assemblyFile(argv[1]);
    unsigned int executionLenLog = 0;
    unsigned int securityParameter = 60;
    bool noProver = false;
    for(int i=2; i< argc; i++){
        const string currArg(argv[i]);
        if(currArg.length()<3){
            continue;
        }

        const string prefix = currArg.substr(0,2);
        const unsigned int num(stoul(currArg.substr(2)));

        if(prefix == timePrefix){
            executionLenLog = num;
        }
        if(prefix == securityPrefix){
            securityParameter = num;
        }
        if(prefix == noProverPrefix){
            noProver = (num == 0);
        }
    }

    if((executionLenLog == 0) || (securityParameter == 0)){
        printHelp(argv[0]);
        return 0;
    }

    execute(assemblyFile,executionLenLog,securityParameter,noProver);

    return 0;
}
