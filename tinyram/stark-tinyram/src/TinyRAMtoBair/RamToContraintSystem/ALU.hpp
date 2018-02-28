#ifndef _RAMTOCOSTRAINTSYSTEM_ALU_HPP_
#define _RAMTOCOSTRAINTSYSTEM_ALU_HPP_
#include "generalPurpose.hpp"
#include <gadgetlib/gadget.hpp>
#include <gadgetlib/infrastructure.hpp>
#include "TinyRAM/TinyRAMInstance.hpp"


using namespace gadgetlib;



extern UnpackedWord unpackedArg1_;
extern UnpackedWord unpackedArg2_;
extern UnpackedWord opcodeResult_;
extern UnpackedWord opcodeCarry_;
extern UnpackedWord & opcodeAux1_;
extern UnpackedWord & opcodeAux2_;
extern UnpackedWord opcodeAux3_;
extern UnpackedWord opcodeAux4_;
extern UnpackedWord opcodeAux5_;
extern UnpackedWord opcodeAux6_;
extern UnpackedWord opcodeAux7_;
extern UnpackedWord memAux1_;
extern UnpackedWord memAux2_;
extern UnpackedWord memAux3_;
extern UnpackedWord memAux4_;
extern UnpackedWord memAux5_;
extern UnpackedWord memAux6_;
extern UnpackedWord memAux7_;
extern UnpackedWord memAux8_;

extern unsigned int prngseed;
extern bool standAlone_;
extern FElem program_output;
extern unsigned int ROMSIZE;

/**
 * The function resetALU_GadgetGlobalState() is
 * a hacked added by Michael (2018-02-14),
 * after finding the reason tests involving reductions from TinyRAM to Bair
 * behave differently if executed alone, or after other tests executing this reduction.
 * Technically, they had a big probability to fail.
 * It was found the reduction has some global variables (?!?).
 * As it is not clear what they role exactly is, so a better design could be made,
 * the easiest hack was to add this function, and execute it before invoking any
 * instance of the reduction
 */
void resetALU_GadgetGlobalState();


/********************************************************/
/**************** Class Writing Helpers *****************/
/********************************************************/
// A macro to disallow any non-defined constructors
// This should be used in the private: declarations for a class
#define DISALLOW_CONSTRUCTION(TypeName) \
  TypeName();               

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      ALU_Component_Gadget                  ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_Component_Gadget : virtual public Gadget {
public:
	virtual void init() = 0;
	virtual void generateConstraints() = 0;
protected:
	ALU_Component_Gadget(const ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results)
		: Gadget(pb), inputs_(inputs), results_(results){}
	
	typedef ::std::shared_ptr<const TinyRAMProtoboardParams> TRParamsPtr;
	const TRParamsPtr tinyRAMparams() const;
	// external variables
	
	const ALUInput inputs_;
	const ALUOutput results_;
private:
	DISALLOW_COPY_AND_ASSIGN(ALU_Component_Gadget);
}; // class ALU_Component_Gadget


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                            ALU_Gadget                      ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_Gadget : public Gadget {
public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputVariables,
							const ALUOutput& resultVariables);
	void generateConstraints();
	void generateWitness(unsigned int i);
	void setProgram(const TinyRAMProgram& program);
	//void applyConstraintsToPB();



private:
	ALU_Gadget(ProtoboardPtr pb,
				const ALUInput& inputVariables,
				const ALUOutput& resultVariables);
	virtual void init();
	void createInternalComponents();

	//internal variables
	TinyRAMProgram program_;
	GadgetPtr 	unpackArg1_g_, unpackArg2_g_;  //Putting these here rather than in internal gadgets so booleanity checks will not be done separately for each pc value
	// externals
	const ALUInput inputVariables_;
	const ALUOutput resultVariables_;

	//internal gadgets
	::std::map<Opcode, GadgetPtr> components_; ///< Internal gadgets for the opcodes

	DISALLOW_COPY_AND_ASSIGN(ALU_Gadget);
};


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_XOR_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_XOR_Gadget : public ALU_Component_Gadget{
private:
	ALU_XOR_Gadget(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	virtual void init();

	
public:
	static GadgetPtr create(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

private:
	DISALLOW_COPY_AND_ASSIGN(ALU_XOR_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_AND_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_AND_Gadget : public ALU_Component_Gadget{
private:
	ALU_AND_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_AND_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_OR_Gadget	                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_OR_Gadget : public ALU_Component_Gadget{
private:
	ALU_OR_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_OR_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_NOT_Gadget	                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_NOT_Gadget : public ALU_Component_Gadget{
private:
	ALU_NOT_Gadget(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

	DISALLOW_COPY_AND_ASSIGN(ALU_NOT_Gadget);

};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_ADD_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_ADD_Gadget : public ALU_Component_Gadget {
private:
	ALU_ADD_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);

	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
	
private:
	// internal gadgets
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;
	GadgetPtr packResult_g_;
	GadgetPtr add_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_ADD_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SUB_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_SUB_Gadget : public ALU_Component_Gadget {
private:
	ALU_SUB_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);

	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
	
private:
	// internal gadgets
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;
	GadgetPtr packResult_g_;
	GadgetPtr add_g_;
	DISALLOW_COPY_AND_ASSIGN(ALU_SUB_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_MULL_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_MULL_Gadget : public ALU_Component_Gadget {
private:
	ALU_MULL_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);

	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
	
private:
	Algebra::UnpackedWord multPartials1_;
	Algebra::UnpackedWord multPartials2_;
	Algebra::UnpackedWord unpackedResult_;
	Algebra::UnpackedWord witnessHighBits_;
	Algebra::UnpackedWord dmultPartials1_;
	Algebra::UnpackedWord dmultPartials2_;
	// internal gadgets
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;
	GadgetPtr unpackResult_g_;
	GadgetPtr mult_g_;
	GadgetPtr dmultPack_g_;
	GadgetPtr packHighBits_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_MULL_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_UMULH_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_UMULH_Gadget : public ALU_Component_Gadget {
private:
	ALU_UMULH_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);

	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
	
private:
	Algebra::UnpackedWord multPartials1_;
	Algebra::UnpackedWord multPartials2_;
	Algebra::UnpackedWord unpackedResult_;
	Algebra::UnpackedWord witnessLowBits_;
	Algebra::UnpackedWord dmultPartials1_;
	Algebra::UnpackedWord dmultPartials2_;
	// internal gadgets
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;
	GadgetPtr unpackResult_g_;
	GadgetPtr mult_g_;
	GadgetPtr dmultPack_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_UMULH_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SMULH_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_SMULH_Gadget : public ALU_Component_Gadget {
private:
	ALU_SMULH_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
	
private:
	Algebra::UnpackedWord multPartials1_;
	Algebra::UnpackedWord multPartials2_;
	Algebra::UnpackedWord unpackedResult_;
	Algebra::UnpackedWord witnessLowBits_;
	Algebra::UnpackedWord dmultPartials1_;
	Algebra::UnpackedWord dmultPartials2_;
	// internal gadgets
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;
	GadgetPtr unpackResult_g_;
	GadgetPtr mult_g_;
	GadgetPtr dmultPack_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_SMULH_Gadget);
};


/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_UDIV_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_UDIV_Gadget : public ALU_Component_Gadget {
private:
	ALU_UDIV_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);

	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
	
private:
	Algebra::UnpackedWord cmpFlags_;
	Algebra::Variable isGEQ_;
	Algebra::UnpackedWord unpackedResult_;
	Algebra::UnpackedWord multPartials1_;
	Algebra::UnpackedWord multPartials2_;
	Algebra::UnpackedWord mpartialsArg1_;
	Algebra::Variable mpackArg1_;
	Algebra::UnpackedWord mpartialsRemainder_;
	Algebra::Variable mpackRemainder_;
	Algebra::UnpackedWord witnessRemainder_;
	// internal gadgets
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;
	GadgetPtr unpackResult_g_;
	GadgetPtr mult_g_;
	GadgetPtr multPackArg1_g_;
	GadgetPtr multPackRemainder_g_;
	GadgetPtr GEQ_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_UDIV_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_UMOD_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_UMOD_Gadget : public ALU_Component_Gadget {
private:
	ALU_UMOD_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);

	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
	
private:
	Algebra::UnpackedWord cmpFlags_;
	Algebra::Variable isGEQ_;
	Algebra::UnpackedWord multPartials1_;
	Algebra::UnpackedWord multPartials2_;
	Algebra::UnpackedWord mpartialsArg1_;
	Algebra::Variable mpackArg1_;
	Algebra::UnpackedWord mpartialsRemainder_;
	Algebra::Variable mpackRemainder_;
	Algebra::UnpackedWord witnessRemainder_;
	Algebra::UnpackedWord witnessResult_;
	// internal gadgets
	GadgetPtr unpackArg1_g_;
	GadgetPtr unpackArg2_g_;
	GadgetPtr packResult_g_;
	GadgetPtr mult_g_;
	GadgetPtr multPackArg1_g_;
	GadgetPtr multPackRemainder_g_;
	GadgetPtr GEQ_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_UMOD_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPE_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_CMPE_Gadget : public ALU_Component_Gadget {
private:
	ALU_CMPE_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);

	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
								const ALUInput& inputs,
								const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

private:
	Algebra::Variable pInverse;
	DISALLOW_COPY_AND_ASSIGN(ALU_CMPE_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPA_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_CMPA_Gadget : public ALU_Component_Gadget {
private:
	ALU_CMPA_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
								const ALUInput& inputs,
								const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	Algebra::UnpackedWord cmpFlags_;
	Algebra::Variable isGEQ_;
	GadgetPtr unpack1_g_;
	GadgetPtr unpack2_g_;
	GadgetPtr compareArgs_;
	DISALLOW_COPY_AND_ASSIGN(ALU_CMPA_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPAE_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_CMPAE_Gadget : public ALU_Component_Gadget {
private:
	ALU_CMPAE_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
								const ALUInput& inputs,
								const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	Algebra::UnpackedWord cmpFlags_;
	Algebra::Variable isGEQ_;
	GadgetPtr unpack1_g_;
	GadgetPtr unpack2_g_;
	GadgetPtr compareArgs_;
	DISALLOW_COPY_AND_ASSIGN(ALU_CMPAE_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPG_Gadget                    ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_CMPG_Gadget : public ALU_Component_Gadget {
private:
	ALU_CMPG_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
								const ALUInput& inputs,
								const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	Algebra::UnpackedWord cmpFlags_;
	Algebra::Variable isGEQ_;
	GadgetPtr unpack1_g_;
	GadgetPtr unpack2_g_;
	GadgetPtr compareArgs_;
	DISALLOW_COPY_AND_ASSIGN(ALU_CMPG_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CMPGE_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_CMPGE_Gadget : public ALU_Component_Gadget {
private:
	ALU_CMPGE_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
								const ALUInput& inputs,
								const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	Algebra::UnpackedWord cmpFlags_;
	Algebra::Variable isGEQ_;
	GadgetPtr unpack1_g_;
	GadgetPtr unpack2_g_;
	GadgetPtr compareArgs_;
	DISALLOW_COPY_AND_ASSIGN(ALU_CMPGE_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SHL_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_SHL_Gadget : public ALU_Component_Gadget {
private:
	ALU_SHL_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();
public:
	enum idxs {dRes=1, rem=2, invRem=3, witnessFlag=4};
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	Algebra::UnpackedWord auxArr_;
	// internal gadgets
	GadgetPtr unpackDouble_g_;
	GadgetPtr packResult_g_;

	DISALLOW_COPY_AND_ASSIGN(ALU_SHL_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_SHR_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_SHR_Gadget : public ALU_Component_Gadget {
private:
	ALU_SHR_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();
public:
	enum idxs {dRes=1, rem=2, invRem=3, witnessFlag=4};
	static GadgetPtr create(ProtoboardPtr pb,
						const ALUInput& inputs,
						const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	Algebra::UnpackedWord auxArr_;
	// internal gadgets
	GadgetPtr unpackDouble_g_;
	GadgetPtr packResult_g_;
	DISALLOW_COPY_AND_ASSIGN(ALU_SHR_Gadget);
};

/*********************************/
/***       END OF Gadget       ***/
/*********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_JMP_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_JMP_Gadget : public ALU_Component_Gadget {
private:
	ALU_JMP_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputs,
							const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	DISALLOW_COPY_AND_ASSIGN(ALU_JMP_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_CJMP_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_CJMP_Gadget : public ALU_Component_Gadget{
private:
	ALU_CJMP_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();

public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputs,
							const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

	DISALLOW_COPY_AND_ASSIGN(ALU_CJMP_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************			           ALU_CNJMP_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_CNJMP_Gadget : public ALU_Component_Gadget{
private:
	ALU_CNJMP_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputs,
							const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

	DISALLOW_COPY_AND_ASSIGN(ALU_CNJMP_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_STOEW_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_STOREW_Gadget : public ALU_Component_Gadget {
private:
	ALU_STOREW_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);	
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputs,
							const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

	DISALLOW_COPY_AND_ASSIGN(ALU_STOREW_Gadget);
};

/*********************************/
/***       END OF Gadget       ***/
/*********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_LOADW_Gadget                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ALU_LOADW_Gadget : public ALU_Component_Gadget {
private:
	ALU_LOADW_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputs,
							const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

	DISALLOW_COPY_AND_ASSIGN(ALU_LOADW_Gadget);
};

/*********************************/
/***       END OF Gadget       ***/
/*********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_XOR_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_ANSWER_Gadget : public ALU_Component_Gadget{
private:
	ALU_ANSWER_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputs,
							const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

	DISALLOW_COPY_AND_ASSIGN(ALU_ANSWER_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         ALU_MOV_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_MOV_Gadget : public ALU_Component_Gadget{
private:
	ALU_MOV_Gadget(ProtoboardPtr pb,
					const ALUInput& inputs,
					const ALUOutput& results);
	virtual void init();

public:
	static GadgetPtr create(ProtoboardPtr pb,
							const ALUInput& inputs,
							const ALUOutput& results);
	void generateConstraints();
	void generateWitness();

	DISALLOW_COPY_AND_ASSIGN(ALU_MOV_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************          ALU_RESERVED_OPCODE_24_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class ALU_RESERVED_OPCODE_24_Gadget : public ALU_Component_Gadget {
private:
	ALU_RESERVED_OPCODE_24_Gadget(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	virtual void init();
public:
	static GadgetPtr create(ProtoboardPtr pb,
		const ALUInput& inputs,
		const ALUOutput& results);
	void generateConstraints();
	void generateWitness();
private:
	GadgetPtr unpackArg1_g_;
	DISALLOW_COPY_AND_ASSIGN(ALU_RESERVED_OPCODE_24_Gadget);
};





#endif //_RAMTOCOSTRAINTSYSTEM_ALU_HPP_
