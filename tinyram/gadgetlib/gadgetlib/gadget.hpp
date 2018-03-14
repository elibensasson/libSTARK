/** @file
 *****************************************************************************
 Interfaces and basic gadgets for R1P (Rank 1 prime characteristic)
 constraint systems.

 These interfaces have been designed to allow later adding other fields or constraint
 structures while allowing high level design to stay put.

 A gadget represents (and generates) the constraints, constraint "wiring", and
 witness for a logical task. This is best explained using the physical design of a printed
 circuit. The Protoboard is the board on which we will "solder" our circuit. The wires
 (implemented by Variables) can hold any element of the underlying field. Each constraint
 enforces a relation between wires. These can be thought of as gates.

 The delegation of tasks is as follows:

 -   Constructor - Allocates all Variables to a Protoboard. Creates all sub-gadgets
     that will be needed and wires their inputs and outputs.
     generateConstraints - Generates the constraints which define the
     necessary relations between the previously allocated Variables.

 -   generateWitness - Generates an assignment for all non-input Variables which is
     consistent with the assignment of the input Variables and satisfies
     all of the constraints. In essence, this computes the logical
     function of the Gadget.

 -   create - A static factory method used for construction of the Gadget. This is
     used in order to create a Gadget without explicit knowledge of the
     underlying algebraic field.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef GADGETLIB3_GADGETLIB3_GADGET_HPP_
#define GADGETLIB3_GADGETLIB3_GADGET_HPP_

#include <vector>
#include <algebraLib/variable.hpp>
#include <algebraLib/CircuitPolynomial.hpp>
#include <gadgetlib/protoboard.hpp>
#include <gadgetlib/infrastructure.hpp>
#include "gadgetMacros.hpp"

namespace gadgetlib {

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         class Gadget                       ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

/** 
 Gadget class, representing the constraints and witness generation for a logical task.
 
 Gadget hierarchy:
      LD2 = Low Degree constraints over characteristic 2 extension field.

 Gadgets have a somewhat cumbersome class hierarchy, for the sake of clean gadget construction.
 (1) A field agnostic, concrete (as opposed to interface) gadget will derive from Gadget. For
     instance NAND needs only AND and NOT and does not care about the field, thus it derives from
     Gadget.
 (2) Field specific interface class R1P_Gadget derives from Gadget using virtual
     inheritance, in order to avoid the Dreaded Diamond problem (see
     http://stackoverflow.com/a/21607/1756254 for more info)
 (3) Functional interface classes such as LooseMUX_GadgetBase virtually derive from Gadget and 
     define special gadget functionality. For gadgets with no special interfaces we use the macro
     CREATE_GADGET_BASE_CLASS() for the sake of code consistency (these gadgets can work the same
     without this base class). This is an interface only and the implementation of AND_Gadget is
     field specific.
 (4) These field specific gadgets will have a factory class with static method create, such as
     AND_Gadget::create(...) in order to agnostically create this gadget for use by a field
     agnostic gadget.
 (5) Concrete field dependent gadgets derive via multiple inheritance from two interfaces.
     e.g. R1P_AND_Gadget derives from both AND_Gadget and R1P_Gadget. This was done to allow usage
     of AND_Gadget's field agnostic create() method and R1P_Gadget's field specific val() method.
*/
class Gadget {
private:
    DISALLOW_COPY_AND_ASSIGN(Gadget);
protected:
    ProtoboardPtr pb_;
public:
    Gadget(ProtoboardPtr pb);
    virtual void init() = 0;    
    /* generate constraints must have this interface, however generateWitness for some gadgets
       (like CTime) will take auxiliary information (like memory contents). We do not want to force
       the interface for generateWitness but do want to make sure it is never invoked from base
       class.
    */
    virtual void generateConstraints() = 0;
    virtual void generateWitness(); // Not abstract as this method may have different signatures.
	void addGeneralConstraint(const Algebra::CircuitPolynomial& polynomial, 
							  const ::std::string& name, Opcode opcode = Opcode::NONE);
	void enforceBooleanity(const Algebra::Variable& var,Opcode opcode = Opcode::NONE) {pb_->enforceBooleanity(var,opcode);}
    Algebra::FElem& val(const Algebra::Variable& var) {return pb_->val(var);}
    const Algebra::FElem val(const Algebra::Variable& var) const { return pb_->val(var); }
    const Algebra::FElem val(const Algebra::LinearCombination& lc) const {return pb_->val(lc);}
    bool flagIsSet(const Algebra::FlagVariable& flag) const {return pb_->flagIsSet(flag);}
};

typedef ::std::shared_ptr<Gadget> GadgetPtr; // Not a unique_ptr because sometimes we need to cast
                                             // these pointers for specific gadget operations.
/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         Packing Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
enum class PackingMode : bool { PACK, UNPACK };

class CompressionPacking_Gadget : public Gadget{
private:
	PackingMode packingMode_;
	Algebra::VariableArray unpacked_;
	Algebra::Variable packed_;
	Opcode opcode_;
	CompressionPacking_Gadget(ProtoboardPtr pb,
								const Algebra::VariableArray& unpacked,
								const Algebra::Variable& packed,
								PackingMode packingMode,
								Opcode opcode);
	virtual void init(); // private in order to force programmer to invoke from a Gadget* only
public:
	
	void generateConstraints();
	void generateWitness();
 	static GadgetPtr create(ProtoboardPtr pb,
					const Algebra::VariableArray& unpacked,
					const Algebra::Variable& packed,
					PackingMode packingMode,
					Opcode opcode);
private:
	DISALLOW_COPY_AND_ASSIGN(CompressionPacking_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                   Double-Unpack Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class DoubleUnpack_Gadget : public Gadget{
private:
	Algebra::VariableArray unpacked1_;
	Algebra::VariableArray unpacked2_;
	Algebra::Variable packed_;
	Opcode opcode_;
	DoubleUnpack_Gadget(ProtoboardPtr pb,
								const Algebra::VariableArray& unpacked1,
								const Algebra::VariableArray& unpacked2,
								const Algebra::Variable& packed,
								const Opcode opcode);
	virtual void init(); // private in order to force programmer to invoke from a Gadget* only
public:
	void generateConstraints();
	void generateWitness();
 	static GadgetPtr create(ProtoboardPtr pb,
					const Algebra::VariableArray& unpacked1,
					const Algebra::VariableArray& unpacked2,
					const Algebra::Variable& packed,
					const Opcode opcode);
private:
	DISALLOW_COPY_AND_ASSIGN(DoubleUnpack_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                        Addition Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class Addition_Gadget : public Gadget{
private:
	Algebra::VariableArray input1_;
	Algebra::VariableArray input2_;
	Algebra::VariableArray result_;
	Algebra::VariableArray carry_;
	Algebra::Variable flag_;
	Opcode opcode_;
	Addition_Gadget(ProtoboardPtr pb,
								const Algebra::VariableArray& input1,
								const Algebra::VariableArray& input2,
								const Algebra::VariableArray& result,
								const Algebra::VariableArray& carry,
								const Algebra::Variable& flag,
								const Opcode opcode);
	virtual void init(); // private in order to force programmer to invoke from a Gadget* only
public:
	void generateConstraints();
	void generateWitness();
 	static GadgetPtr create(ProtoboardPtr pb,
					const Algebra::VariableArray& input1,
					const Algebra::VariableArray& input2,
					const Algebra::VariableArray& result,
					const Algebra::VariableArray& carry,
					const Algebra::Variable& flag,
					const Opcode opcode);
private:
	DISALLOW_COPY_AND_ASSIGN(Addition_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************           MultiplicationPacking Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class MultiplicationPacking_Gadget : public Gadget{
private:
	MultiplicationPacking_Gadget(ProtoboardPtr pb,
								const Algebra::VariableArray& unpacked,
								const Algebra::VariableArray& partials,
								const Algebra::Variable& packed,
								const bool isSigned,
								const PackingMode packingMode,
								const Opcode opcode);
	virtual void init(); 
	PackingMode packingMode_;
	Algebra::VariableArray unpacked_;
	Algebra::VariableArray partials_;
	Algebra::Variable packed_;
	bool isSigned_;
	Opcode opcode_;
public:
	void generateConstraints();
	void generateWitness(int n = -1);
	static GadgetPtr create(ProtoboardPtr pb,
								const Algebra::VariableArray& unpacked,
								const Algebra::VariableArray& partials,
								const Algebra::Variable& packed,
								const bool isSigned,
								const PackingMode packingMode,
								Opcode opcode);
private:
	DISALLOW_COPY_AND_ASSIGN(MultiplicationPacking_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************           DoubleMultiplicationPacking Gadget               ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class DoubleMultPack_Gadget : public Gadget{
private:
	DoubleMultPack_Gadget(ProtoboardPtr pb,
		const Algebra::VariableArray& unpacked1,
		const Algebra::VariableArray& unpacked2,
		const Algebra::VariableArray& partials1,
		const Algebra::VariableArray& partials2,
		const Algebra::Variable& packed,
		const bool isSigned,
		const Opcode opcode);
	virtual void init();
	Algebra::VariableArray unpacked1_;
	Algebra::VariableArray unpacked2_;
	Algebra::VariableArray partials1_;
	Algebra::VariableArray partials2_;
	Algebra::Variable packed_;
	bool isSigned_;
	Opcode opcode_;
public:
	void generateConstraints();
	void generateWitness();
	static GadgetPtr create(ProtoboardPtr pb,
		const Algebra::VariableArray& unpacked1,
		const Algebra::VariableArray& unpacked2,
		const Algebra::VariableArray& partials1,
		const Algebra::VariableArray& partials2,
		const Algebra::Variable& packed,
		const bool isSigned,
		const Opcode opcode);
private:
	DISALLOW_COPY_AND_ASSIGN(DoubleMultPack_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                  Multiplication Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class Multiplication_Gadget : public Gadget{
private:
	Multiplication_Gadget(ProtoboardPtr pb,
								const Algebra::VariableArray& unpacked1,
								const Algebra::VariableArray& unpacked2,
								const Algebra::VariableArray& partials1,
								const Algebra::VariableArray& partials2,
								const bool isSigned,
								const Opcode opcode);
	virtual void init();
	Algebra::VariableArray unpacked1_;
	Algebra::VariableArray unpacked2_;
	Algebra::VariableArray partials1_;
	Algebra::VariableArray partials2_;
	Opcode opcode_;
	bool isSigned_;
	int idxLast;
	GadgetPtr multPack1_g_;
public:
	void generateConstraints();
	void generateWitness();
	static GadgetPtr create(ProtoboardPtr pb,
								const Algebra::VariableArray& unpacked1,
								const Algebra::VariableArray& unpacked2,
								const Algebra::VariableArray& partials1,
								const Algebra::VariableArray& partials2,
								const bool isSigned,
								const Opcode opcode);
private:
	DISALLOW_COPY_AND_ASSIGN(Multiplication_Gadget);
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         Greater Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

class GreaterEqual_Gadget : public Gadget{
private:
	GreaterEqual_Gadget(ProtoboardPtr pb,
					const Algebra::UnpackedWord& var1,
					const Algebra::UnpackedWord& var2,
					const Algebra::UnpackedWord& flags,
					const Algebra::FlagVariable& flagGEQ,
					const bool isSigned,
					const Opcode opcode);
	virtual void init(); // private in order to force programmer to invoke from a Gadget* only

	Algebra::UnpackedWord var1_;
	Algebra::UnpackedWord var2_;
	Algebra::UnpackedWord flags_;
	Algebra::FlagVariable flagGEQ_;
	bool isSigned_;
	Opcode opcode_;
	unsigned int unpakcedSize_;
public:
	static GadgetPtr create(ProtoboardPtr pb,
						const Algebra::UnpackedWord& var1,
						const Algebra::UnpackedWord& var2,
						const Algebra::UnpackedWord& flags,
						const Algebra::FlagVariable& flagGEQ,
						const bool isSigned,
						const Opcode opcode);
	void generateConstraints();
	void generateWitness();
};

} // namespace gadgetlib

#endif // GADGETLIB3_GADGETLIB3_GADGET_HPP_
