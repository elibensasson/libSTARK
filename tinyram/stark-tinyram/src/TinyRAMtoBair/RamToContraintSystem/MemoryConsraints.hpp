#ifndef _COMMON_CONSTRAINTSLIB2_RAMTOCOSTRAINTSYSTEM_MEMORY_CONSTRAINTS_HPP_
#define _COMMON_CONSTRAINTSLIB2_RAMTOCOSTRAINTSYSTEM_MEMORY_CONSTRAINTS_HPP_

#include <gadgetlib/gadget.hpp>
#include <gadgetlib/protoboard.hpp>
#include "TinyRAMtoBair/RamToContraintSystem/generalPurpose.hpp"



class MemoryConstraints : public Gadget{
private:
	MemoryConstraints(ProtoboardPtr pb,
		const  MemoryFollowingTraceVariables& followingTraceVariables);
	
	void init();

	Algebra::UnpackedWord unpackedaddr1_;
	Algebra::UnpackedWord unpackedaddr2_;
	Algebra::UnpackedWord unpackedTimestamp1_;
	Algebra::UnpackedWord unpackedTimestamp2_;
	Algebra::UnpackedWord addressFlags_;
	Algebra::UnpackedWord timestampFlags_;
	Algebra::Variable addressIsGEQ_;
	Algebra::Variable timestampIsGEQ_;
	Algebra::UnpackedWord multPartials1_;
	Algebra::UnpackedWord multPartials2_;

	GadgetPtr unpackAddr1_g_;
	GadgetPtr unpackAddr2_g_;
	GadgetPtr compareAddresses_;
	GadgetPtr unpackTimestamp1_g_;
	GadgetPtr unpackTimestamp2_g_;
	GadgetPtr compareTimestamps_g_;

	MemoryTraceVariables first_;
	MemoryTraceVariables second_;
public:
	static GadgetPtr create(ProtoboardPtr pb,
				const  MemoryFollowingTraceVariables& followingTraceVariables);

	void generateConstraints();
	void generateWitness();

};	


#endif //_COMMON_CONSTRAINTSLIB2_RAMTOCOSTRAINTSYSTEM_MEMORY_CONSTRAINTS_HPP_
