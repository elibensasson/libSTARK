#include "TinyRAMDefinitions.hpp"



int trNumRegisters = -1;
int trRegisterLen = -1;
int trNumRegistersInclPC = -1;
int trRegisterOrPCIndexLen = -1;
int trRegisterNoPCIndexLen = -1;
int trRegisterIndexLen = -1;
int trPCLength = -1;
int trInstructionLength = -1;
int trConfigurationLength = -1;
int trFirstReadInstr = -1;
int trInputReadSteps = -1;
int trInputRegisterNum = -1;

static bool trParametersInited = false;

void clearTinyRAMParams(){
    trParametersInited = false;
    trNumRegisters = -1;
    trRegisterLen = -1;
    trNumRegistersInclPC = -1;
    trRegisterOrPCIndexLen = -1;
    trRegisterNoPCIndexLen = -1;
    trRegisterIndexLen = -1;
    trPCLength = -1;
    trInstructionLength = -1;
    trConfigurationLength = -1;
    trFirstReadInstr = -1;
    trInputReadSteps = -1;
    trInputRegisterNum = -1;
}

void initTinyRAMParams(int numRegisters, int registerLen) {
    trParametersInited = true;

    trNumRegisters = numRegisters;
    trRegisterLen = registerLen;
    trNumRegistersInclPC = trNumRegisters + 1;
    trRegisterOrPCIndexLen = (int)ceil( ::Infrastructure::Log2((double)(trNumRegistersInclPC)) );
    trRegisterNoPCIndexLen = (int)ceil( ::Infrastructure::Log2((double)(trNumRegisters)) );
    trRegisterIndexLen = trRegisterOrPCIndexLen;
    trPCLength = trRegisterLen;
    trInstructionLength = trOpcodeLen + 1 + 2*trRegisterIndexLen + max(trRegisterIndexLen,trRegisterLen);
    trConfigurationLength = trPCLength + trNumRegisters * trRegisterLen + 1;

    //Input Reading:
    trFirstReadInstr = 1;
    trInputReadSteps = 3;
    trInputRegisterNum = 0;
}

void initTinyRAMParamsFromEnvVariables() {
    initTinyRAMParams(REGISTERS_NUMBER,
                      REGISTER_LENGTH);
}
