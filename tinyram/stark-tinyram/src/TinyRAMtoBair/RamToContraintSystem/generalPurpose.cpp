#include "generalPurpose.hpp"



/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         TraceVariables	                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

TraceVariables::TraceVariables(unsigned int pcLength, unsigned int numberOfRegisters) :
	timeStamp_("timeStamp"), flag_("flag"), pc_(pcLength, "pc"), registers_(numberOfRegisters, "register"){}

TraceVariables::TraceVariables(const Algebra::Variable timeStamp,const  Algebra::FlagVariable flag,
	const Algebra::UnpackedWord pc,const Algebra::VariableArray registers) :
	timeStamp_(timeStamp), flag_(flag), pc_(pc), registers_(registers){}

int TraceVariables::size() const { return 2 + pc_.size() + registers_.size(); }
/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                  FollowingTraceVariables					******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

FollowingTraceVariables::FollowingTraceVariables(unsigned int pcLength, unsigned int numberOfRegisters) :
						first_(pcLength, numberOfRegisters), second_(pcLength, numberOfRegisters){}


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					ALUInput				 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/


ALUInput::ALUInput(const Algebra::Variable& arg1_val, const Algebra::Variable& arg2_val,
	const Algebra::Variable& dest_val, const Algebra::FlagVariable& flag, const Algebra::UnpackedWord& pc) :
	arg1_val_(arg1_val), arg2_val_(arg2_val), dest_val_(dest_val), flag_(flag),pc_(pc){};


ALUInput::ALUInput() : arg1_val_("ALU arg1_val"), arg2_val_("ALU arg2_val"), dest_val_("ALU dest_val"),
						flag_("ALU flag"),pc_(){}

ALUInput::ALUInput(const Algebra::FlagVariable& flag, const Algebra::UnpackedWord& pc) :
					arg1_val_("ALU arg1_val"), arg2_val_("ALU arg2_val"), dest_val_("ALU dest_val"),
					flag_(flag),pc_(pc){}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					ALUOutput				 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

ALUOutput::ALUOutput(const Algebra::Variable& flag, const Algebra::Variable& result, 
					const Algebra::FlagVariable& isMemOp, const Algebra::Variable& address,
					const Algebra::FlagVariable& isLoadOp,const Algebra::Variable& value) :
	flag_(flag), result_(result),isMemOp_(isMemOp),address_(address), isLoadOp_(isLoadOp),value_(value){};

ALUOutput::ALUOutput(const Algebra::Variable& flag) : flag_(flag), result_("ALU result"), isMemOp_("isMemOp"),
													address_("ALU adress"),isLoadOp_("isLoadOp"),value_("address value"){}

ALUOutput::ALUOutput(const Algebra::Variable& flag, const Algebra::FlagVariable& isMemOp, const Algebra::Variable& address,
					const Algebra::FlagVariable& isLoadOp, const Algebra::Variable& value) :
			flag_(flag), result_("ALU result"), isMemOp_(isMemOp), address_(address), isLoadOp_(isLoadOp), value_(value){};

ALUOutput::ALUOutput() : flag_("ALU flag"), result_("ALU result"), isMemOp_("isMemOP"), address_("ALU adress"),
						isLoadOp_("isLoadOp"), value_("address value_"){}


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************					MemoryTraceVariables	 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

MemoryTraceVariables::MemoryTraceVariables(const Algebra::Variable& isMemOp,
											const Algebra::Variable& address,
											const Algebra::Variable& value,
											const Algebra::FlagVariable& isLoad,
											const Algebra::Variable& timeStamp) :
	isMemOp_(isMemOp),address_(address), value_(value), isLoad_(isLoad), timeStamp_(timeStamp){}

MemoryTraceVariables::MemoryTraceVariables(const Algebra::Variable& timeStamp) : isMemOp_("Memory isMemOp"), 
	address_("Memory Address"), value_("Memory Value"),	isLoad_("Memory isLoad"), timeStamp_(timeStamp){}

MemoryTraceVariables::MemoryTraceVariables() :isMemOp_("Memory isMemOp"), address_("Memory Address"), value_("Memory Value"), 
											isLoad_("Memory isLoad"), timeStamp_("Memory timeStamp"){}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************				MemoryFollowingTraceVariables 				******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

MemoryFollowingTraceVariables::MemoryFollowingTraceVariables() : first_(), second_(){}

MemoryFollowingTraceVariables::MemoryFollowingTraceVariables(const Algebra::Variable& timeStamp1,
															const Algebra::Variable& timeStamp2) :
	first_(timeStamp1), second_(timeStamp2){}





