#ifndef _COMMON_CONSTRAINTSLIB2_RAMTOCOSTRAINTSYSTEM_TRACECONSISTENCY_HPP_
#define _COMMON_CONSTRAINTSLIB2_RAMTOCOSTRAINTSYSTEM_TRACECONSISTENCY_HPP_

#include <gadgetlib/gadget.hpp>
#include <gadgetlib/protoboard.hpp>
#include "generalPurpose.hpp"
#include "TinyRAM/TinyRAMInstance.hpp"
using gadgetlib::Gadget;
using gadgetlib::ProtoboardPtr;



class TraceConsistency : public Gadget{
private:
	ALUOutput aluOutput_;
	FollowingTraceVariables followingTraceVariables_;
	TinyRAMProgram program_;

	TraceConsistency(ProtoboardPtr pb,
					const ALUOutput& aluOutput,
					const FollowingTraceVariables& followingTraceVariables);
	virtual void init(){};


	void timeStampConsistency();
	void pcConsistency();
	void registerConsistency();
	void timeStampWitness();
	void pcWitness(unsigned int nextPC);
	void registersWitness(unsigned int programLine);

public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUOutput& aluOutput,
							const FollowingTraceVariables& followingTraceVariables);
	void setProgram(const TinyRAMProgram& program);
	void generateConstraints();
	void generateWitness(unsigned int programLine);

};


#endif //_COMMON_CONSTRAINTSLIB2_RAMTOCOSTRAINTSYSTEM_TRACECONSISTENCY_HPP_
