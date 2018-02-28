#ifndef RAM_DEFINITIONS_HPP
#define RAM_DEFINITIONS_HPP
/************************************************************************TinyRAMDefinitions.hpp****************************************/
/** @file
 * Basic definitions of parameters and constants for TinyRAM, together with data types needed in the reductions from TinyRAM.
 * It also contains functions for creating input instances for the RAMParams constructor.
 **/

#include <cstdint>
#include <bitset>
#include <fstream>
#include <sstream>
#include <common/Infrastructure/Infrastructure.hpp>

#define REGISTERS_NUMBER 15
#define REGISTER_LENGTH 16
#define NUMBER_OPCODES 32
#define BIN_EOF_STRING "EOF"

using namespace std;



/** Number of registers that the TinyRAM has (excluding PC) */
extern int trNumRegisters;

/** The bit-length of TinyRAM registers */
extern int trRegisterLen;


/** Numbers of registers including the program counter (used for code clarity)
 * Set to trNumRegisters + 1. */
extern int trNumRegistersInclPC;

/** Length of an index into the register file, including PC.
 * Set to ceil( ::Infrastructure::Log2(trNumRegistersInclPC)).
 */
extern int trRegisterOrPCIndexLen;

/** Length of an index into the register file, excluding PC.
 * Set to ceil( ::Infrastructure::Log2(trNumRegistersPC) ).
 */
extern int trRegisterNoPCIndexLen;

/**
* Holds the number of possible opcodes
*/
const int trNumOpcodes = NUMBER_OPCODES;

/** 
 * Holds the length of an opcode representation. Should be Log2ceil(trNumOpcodes).
 */
const int trOpcodeLen = ::Infrastructure::Log2ceil(trNumOpcodes);


/**
 * Set the various RAM parameters.
 * Must be called before using any RAM-related variables or classes.
 * Must not be called more than once.
 */
void initTinyRAMParams(int numRegisters, int registerLen);
void initTinyRAMParamsFromEnvVariables();

/**
 * Clears the TinyRAM params
 * sets the state as before it was initialized
 */
void clearTinyRAMParams();




#endif

