#include "MemoryConsraints.hpp"
#include <algebraLib/variable_operators.hpp>
#include "TinyRAMtoBair/RamToContraintSystem/ALU.hpp"



	MemoryConstraints::MemoryConstraints(ProtoboardPtr pb,
		const  MemoryFollowingTraceVariables& followingTraceVariables) :
		Gadget(pb),
		unpackedaddr1_(unpackedArg1_),
        unpackedaddr2_(unpackedArg2_),
		unpackedTimestamp1_(opcodeAux1_),
        unpackedTimestamp2_(opcodeAux2_),
		addressFlags_(opcodeAux3_),
        timestampFlags_(opcodeAux4_),
		addressIsGEQ_(opcodeAux3_[0]),
        timestampIsGEQ_(opcodeAux4_[0]),
		multPartials1_(opcodeAux5_), 
        multPartials2_(opcodeAux6_),
        first_(followingTraceVariables.first_),
        second_(followingTraceVariables.second_){};

GadgetPtr MemoryConstraints::create(ProtoboardPtr pb,
				const  MemoryFollowingTraceVariables& followingTraceVariables){
	GadgetPtr pGadget(new MemoryConstraints(pb, followingTraceVariables));
	pGadget->init();
	return pGadget;
}

void MemoryConstraints::init(){
	unpackAddr1_g_ = CompressionPacking_Gadget::create(pb_, unpackedaddr1_, first_.address_, PackingMode::UNPACK ,Opcode::MEMORY);
	unpackAddr2_g_ = CompressionPacking_Gadget::create(pb_, unpackedaddr2_, second_.address_, PackingMode::UNPACK ,Opcode::MEMORY);
	//Checks if addr1 >= addr2
	compareAddresses_ = GreaterEqual_Gadget::create(pb_, unpackedaddr2_, unpackedaddr1_, addressFlags_, addressIsGEQ_, false, Opcode::MEMORY);
	unpackTimestamp1_g_ = MultiplicationPacking_Gadget::create(pb_, unpackedTimestamp1_, multPartials1_, first_.timeStamp_, false, PackingMode::UNPACK, Opcode::MEMORY);
	unpackTimestamp2_g_ = MultiplicationPacking_Gadget::create(pb_, unpackedTimestamp2_, multPartials2_, second_.timeStamp_, false, PackingMode::UNPACK, Opcode::MEMORY);
	compareTimestamps_g_ = GreaterEqual_Gadget::create(pb_, unpackedTimestamp2_, unpackedTimestamp1_, timestampFlags_, timestampIsGEQ_, false, Opcode::MEMORY);
}

void MemoryConstraints::generateConstraints(){
	//std::cout << "csMEM: " << pb_->constraintSystem(Opcode::MEMORY).getNumberOfConstraints() << std::endl;
	GADGETLIB_ASSERT(0 == pb_->constraintSystem(Opcode::MEMORY).getNumberOfConstraints(), "op::MEMORY cs is clear");
	unpackAddr1_g_->generateConstraints();
	unpackAddr2_g_->generateConstraints();
	compareAddresses_->generateConstraints();
	unpackTimestamp1_g_->generateConstraints();
	unpackTimestamp2_g_->generateConstraints();
	compareTimestamps_g_->generateConstraints();
	auto cons = pb_->constraintSystem(Opcode::MEMORY).getConstraints();
	pb_->clearConstraintSystem(Opcode::MEMORY);
	for (Constraint constraint : cons){
		addGeneralConstraint(second_.isMemOp_ * constraint.constraint(), "isMemOp*isMemOp' * " + constraint.asString(), Opcode::MEMORY);
	}
	const Algebra::FElem g = Algebra::xFE();
	pb_->addMemoryConstraint((first_.isMemOp_ + Algebra::one()) * second_.isMemOp_, "(1 - isMemOp)* isMemOp'");
	pb_->addMemoryConstraint(second_.isMemOp_ * (g + addressIsGEQ_) * (Algebra::one() + addressIsGEQ_),"isMemOp * isMemOp' * (1 + addrflagEqual) * (1 + addrflagGreater)");
	pb_->addMemoryConstraint(second_.isMemOp_ * addressIsGEQ_ * (Algebra::one() + addressIsGEQ_) * second_.isLoad_ * (first_.value_ + second_.value_), "isMemOp* isMemOp' *addrflagEqual*isLoad' *(value - value') = 0");
	pb_->addMemoryConstraint(second_.isMemOp_ * addressIsGEQ_ * (Algebra::one() + addressIsGEQ_) * (Algebra::one() + timestampIsGEQ_), "isMemOp * isMemOp' * addrflagEqual *(1 +  timestampflagGreater)");
}

void MemoryConstraints::generateWitness(){
	unpackAddr1_g_->generateWitness();
	unpackAddr2_g_->generateWitness();
	compareAddresses_->generateWitness();
	unsigned int degree1 = pb_->getDegreeOfFElem(pb_->val(first_.timeStamp_));
	unsigned int degree2 = pb_->getDegreeOfFElem(pb_->val(second_.timeStamp_));
	std::dynamic_pointer_cast<MultiplicationPacking_Gadget>(unpackTimestamp1_g_)->generateWitness(degree1);
	std::dynamic_pointer_cast<MultiplicationPacking_Gadget>(unpackTimestamp2_g_)->generateWitness(degree2);
	compareTimestamps_g_->generateWitness();
}


