#ifndef CONSTARINTSLIB2_GENERAL_PURPOSE_HPP_
#define CONSTARINTSLIB2_GENERAL_PURPOSE_HPP_

#include <algebraLib/variable.hpp>
#include <gadgetlib/protoboard.hpp>
#include <gadgetlib/infrastructure.hpp>

using namespace Algebra;
using namespace gadgetlib;

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         TraceVariables	                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

struct TraceVariables{
	Algebra::Variable timeStamp_;
	Algebra::FlagVariable flag_;
	Algebra::UnpackedWord pc_;
	Algebra::VariableArray registers_;
	
	TraceVariables(const Algebra::Variable timeStamp,
					const Algebra::FlagVariable flag,
					const Algebra::UnpackedWord pc,
					const Algebra::VariableArray registers);

	TraceVariables(unsigned int pcLength, unsigned int numberOfRegisters);

	int size() const;
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                  FollowingTraceVariables					******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

struct FollowingTraceVariables{
	TraceVariables first_;
	TraceVariables second_;

	FollowingTraceVariables(unsigned int pcLength, unsigned int numberOfRegisters);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					ALUInput				 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/


struct ALUInput{
	Algebra::Variable arg1_val_;
	Algebra::Variable arg2_val_;
	Algebra::Variable dest_val_;
	Algebra::FlagVariable flag_;
	Algebra::UnpackedWord pc_;

	ALUInput(const Algebra::Variable& arg1_val,
			const Algebra::Variable& arg2_val,
			const Algebra::Variable& dest_val,
			const Algebra::FlagVariable& flag,
			const Algebra::UnpackedWord& pc);

	ALUInput(const Algebra::FlagVariable& flag,
			const Algebra::UnpackedWord& pc);

	ALUInput();
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					ALUOutput				 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

struct ALUOutput{
	Algebra::Variable flag_;
	Algebra::Variable result_;
	Algebra::FlagVariable isMemOp_;
	Algebra::Variable address_;
	Algebra::FlagVariable isLoadOp_;
	Algebra::Variable value_;

	ALUOutput(const Algebra::Variable& flag,
			const Algebra::Variable& result,
			const Algebra::FlagVariable& isMemOp,
			const Algebra::Variable& address,
			const Algebra::FlagVariable& isLoadOp,
			const Algebra::Variable& value);

	ALUOutput(const Algebra::Variable& flag,
		const Algebra::FlagVariable& isMemOp,
		const Algebra::Variable& address,
		const Algebra::FlagVariable& isLoadOp,
		const Algebra::Variable& value);

	ALUOutput(const Algebra::Variable& flag);

	ALUOutput();
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					MemoryTraceVariables	 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

struct MemoryTraceVariables{
	Algebra::Variable isMemOp_;
	Algebra::Variable address_;
	Algebra::Variable value_;
	Algebra::FlagVariable isLoad_;
	Algebra::Variable timeStamp_;

	MemoryTraceVariables(const Algebra::Variable& isMemOp,
						const Algebra::Variable& address,
						const Algebra::Variable& value,
						const Algebra::FlagVariable& isLoad,
						const Algebra::Variable& timeStamp);

	MemoryTraceVariables(const Algebra::Variable& timeStamp);
	MemoryTraceVariables();
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************				MemoryFollowingTraceVariables 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

struct MemoryFollowingTraceVariables{
	MemoryTraceVariables first_;
	MemoryTraceVariables second_;

	MemoryFollowingTraceVariables();
	MemoryFollowingTraceVariables(const Algebra::Variable& timeStamp1, 
								  const Algebra::Variable& timeStamp2);
};

#endif
