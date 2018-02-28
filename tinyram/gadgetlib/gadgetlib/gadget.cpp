/** @file
 *****************************************************************************
 Declarations of the interfaces and basic gadgets for R1P (Rank 1 prime characteristic)
 constraint systems.

 See details in gadget.hpp .
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <cmath>
#include <memory>
#include <math.h>
#include <stdexcept>
#include <gadgetlib/gadget.hpp>
#include <gadgetlib/common_use.hpp>
#include <algebraLib/variable_operators.hpp>
#include <algebraLib/CircuitPolynomial.hpp>
#include <iostream>

using ::std::shared_ptr;
using ::std::string;
using ::std::vector;
using ::std::cout;
using ::std::cerr;
using ::std::endl;


namespace gadgetlib {

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                      Gadget Interfaces                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

/***********************************/
/***          Gadget             ***/
/***********************************/

Gadget::Gadget(ProtoboardPtr pb) : pb_(pb) {
    GADGETLIB_ASSERT(pb != NULL, "Attempted to create gadget with uninitialized Protoboard.");
}

void Gadget::generateWitness() {
    GADGETLIB_FATAL("Attempted to generate witness for an incomplete Gadget type.");
}

void Gadget::addGeneralConstraint(const Algebra::CircuitPolynomial& polynomial,
								const ::std::string& name, Opcode opcode){
	pb_->addGeneralConstraint(polynomial, name, opcode);
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************       Gadget CompressionPacking_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

CompressionPacking_Gadget::CompressionPacking_Gadget(ProtoboardPtr pb,
														const Algebra::VariableArray& unpacked,
														const Algebra::Variable& packed,
														PackingMode packingMode,
														Opcode opcode) :
    Gadget(pb), 
    packingMode_(packingMode),
    unpacked_(unpacked),
    packed_(packed),
    opcode_(opcode)
{};

GadgetPtr CompressionPacking_Gadget::create(ProtoboardPtr pb,
											const Algebra::VariableArray& unpacked,
											const Algebra::Variable& packed,
											PackingMode packingMode, 
											Opcode opcode){
	GadgetPtr pGadget(new CompressionPacking_Gadget(pb,unpacked,packed,packingMode,opcode));
	pGadget->init();
	return pGadget;
}

void CompressionPacking_Gadget::init(){}

//we either add the constraints that the unpacking is boolean (In UNPACK mode this gadget is really just a booleanity check),
//and in pack mode we only check that 
void CompressionPacking_Gadget::generateConstraints(){
	const int numBits = unpacked_.size();
	const int bitsPerFieldElem = Algebra::ExtensionDegree;
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	Algebra::LinearCombination packedPolyLC;
	for (int i = 0; i < bitsPerFieldElem && i < numBits; ++i) {
		packedPolyLC = packedPolyLC + x_i * unpacked_[i];
		x_i *= x;
		if (packingMode_ == PackingMode::UNPACK) { enforceBooleanity(unpacked_[i], opcode_); }
	}
	Algebra::CircuitPolynomial packedPoly(packedPolyLC);
	if (packingMode_ == PackingMode::PACK)
		

	pb_->addGeneralConstraint(packedPoly + packed_, "packed = packedPoly", opcode_);
}

void CompressionPacking_Gadget::generateWitness(){
	const int n = unpacked_.size();
	const int bitsPerFieldElem = Algebra::ExtensionDegree;
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	if (packingMode_ == PackingMode::PACK) {
		int currentBit = 0;
		Algebra::FElem packedVal = Algebra::zero();
		Algebra::FElem x_i = Algebra::one(); // will hold x^i
		for (int j = 0; j < bitsPerFieldElem && currentBit < n; ++j, ++currentBit) {
			packedVal += x_i * val(unpacked_[currentBit]);
			x_i *= x;
		}
		val(packed_) = packedVal;
		return;
	}
	// else (UNPACK)
	GADGETLIB_ASSERT(packingMode_ == PackingMode::UNPACK,
		"Packing gadget created with unknown packing mode.");
	for (int i = 0; i < n; ++i) {
		Algebra::FElem value = pb_->val(packed_);
		pb_->val(unpacked_[i]) = Algebra::mapFieldElementToInteger(i, 1, value) ? Algebra::one() : Algebra::zero();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************             Gadget DoubleUnpack_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
DoubleUnpack_Gadget::DoubleUnpack_Gadget(ProtoboardPtr pb,
														const Algebra::VariableArray& unpacked1,
														const Algebra::VariableArray& unpacked2,
														const Algebra::Variable& packed,
														const Opcode opcode)
	:Gadget(pb), unpacked1_(unpacked1), unpacked2_(unpacked2), packed_(packed), opcode_(opcode){}

GadgetPtr DoubleUnpack_Gadget::create(ProtoboardPtr pb,
											const Algebra::VariableArray& unpacked1,
											const Algebra::VariableArray& unpacked2,
											const Algebra::Variable& packed,
											const Opcode opcode){
	GadgetPtr pGadget(new DoubleUnpack_Gadget(pb,unpacked1,unpacked2,packed,opcode));
	pGadget->init();
	return pGadget;
}

void DoubleUnpack_Gadget::init(){}

void DoubleUnpack_Gadget::generateConstraints(){
	const int numBits1 = unpacked1_.size();
	const int numBits2 = unpacked2_.size();
	const int bitsPerFieldElem = Algebra::ExtensionDegree;
	const Algebra::FElem x = Algebra::FElem(getGF2E_X());
	Algebra::FElem x_i = Algebra::one(); // will hold x^i
	Algebra::LinearCombination packedPoly;
	for (int i = 0; i < bitsPerFieldElem && i < numBits1; ++i) {
		packedPoly = packedPoly + x_i * unpacked1_[i];
		x_i *= x;
		enforceBooleanity(unpacked1_[i], opcode_);
	}
	for (int i = 0; i+numBits1 < bitsPerFieldElem && i < numBits2; ++i) {
		packedPoly = packedPoly + x_i * unpacked2_[i];
		x_i *= x;
		enforceBooleanity(unpacked2_[i], opcode_);
	}
	
	Algebra::CircuitPolynomial P(packedPoly+packed_);

	pb_->addGeneralConstraint(P, "packed = packedPoly", opcode_);
}

void DoubleUnpack_Gadget::generateWitness(){
	const size_t n1 = unpacked1_.size();
	const size_t n2 = unpacked2_.size();
	const size_t bitsPerFieldElem = Algebra::ExtensionDegree;
	const size_t v = Algebra::mapFieldElementToInteger(0, bitsPerFieldElem, pb_->val(packed_));
	for (size_t i = 0; i < bitsPerFieldElem && i < n1; ++i) {
		pb_->val(unpacked1_[i]) = (v & (size_t(1) << i)) ? Algebra::one() : Algebra::zero();
	}
	for (size_t i = 0; i + n1 < bitsPerFieldElem && i < n2; ++i) {
		pb_->val(unpacked2_[i]) = (v & ((size_t(1) << n1) << i)) ? Algebra::one() : Algebra::zero();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                 Gadget Addition_Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
Addition_Gadget::Addition_Gadget(ProtoboardPtr pb,
		const Algebra::VariableArray& input1,
		const Algebra::VariableArray& input2,
		const Algebra::VariableArray& result,
		const Algebra::VariableArray& carry,
		const Algebra::Variable& flag,
		const Opcode opcode)
	:Gadget(pb), input1_(input1), input2_(input2), result_(result), carry_(carry), flag_(flag), opcode_(opcode){}

GadgetPtr Addition_Gadget::create(ProtoboardPtr pb,
		const Algebra::VariableArray& input1,
		const Algebra::VariableArray& input2,
		const Algebra::VariableArray& result,
		const Algebra::VariableArray& carry,
		const Algebra::Variable& flag,
		const Opcode opcode){
	GadgetPtr pGadget(new Addition_Gadget(pb, input1, input2, result, carry, flag, opcode));
	pGadget->init();
	return pGadget;
}

void Addition_Gadget::init(){}

void Addition_Gadget::generateConstraints(){
	size_t wordLength = result_.size();
	const Algebra::Variable a = input1_[0];
	const Algebra::Variable b = input2_[0];
	const Algebra::Variable d = result_[0];
	pb_->addGeneralConstraint(d + a + b, "input1[0] + input2[0] = result[0]", opcode_);
	pb_->addGeneralConstraint(a * b + carry_[1], "carry1", opcode_);
	for (unsigned int i = 1; i < wordLength; ++i){
		const Algebra::Variable a = input1_[i];
		const Algebra::Variable b = input2_[i];
		const Algebra::Variable c = carry_[i];
		const Algebra::Variable d = result_[i];
		const Algebra::Variable nextCarry = (i < wordLength - 1) ? carry_[i + 1] : flag_;
		pb_->addGeneralConstraint(a + b + c + d, "input1[i] + input2[i] = result[i]" , opcode_);
		pb_->addGeneralConstraint(nextCarry + a*b + a*c + b*c, "ADD Check next carry/flag", opcode_);
	}
}

void Addition_Gadget::generateWitness(){
	size_t wordLength = result_.size();
	pb_->val(result_[0]) = pb_->val(input1_[0]) + pb_->val(input2_[0]);
	pb_->val(carry_[1]) = pb_->val(input1_[0]) * pb_->val(input2_[0]);
	for (unsigned int i = 1; i < wordLength; ++i){
		const bool b1 = (val(input1_[i]) == Algebra::one());
		const bool b2 = (val(input2_[i]) == Algebra::one());
		const bool b3 = (val(carry_[i]) == Algebra::one());
		pb_->val(result_[i]) = (b1 ^ b2 ^ b3) ? Algebra::one() : Algebra::zero();
		const Algebra::Variable nextCarry = (i < wordLength - 1) ? carry_[i + 1]: flag_;
		pb_->val(nextCarry) = (b1 ? (b2 || b3) : (b2 && b3)) ? Algebra::one() : Algebra::zero();
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************           MultiplicationPacking Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
MultiplicationPacking_Gadget::MultiplicationPacking_Gadget(ProtoboardPtr pb,
	const Algebra::VariableArray& unpacked,
	const Algebra::VariableArray& partials,
	const Algebra::Variable& packed,
	const bool isSigned,
	const PackingMode packingMode,
	const Opcode opcode)
	: 
        Gadget(pb),
        packingMode_(packingMode),
        unpacked_(unpacked),
        partials_(partials),
        packed_(packed),
        isSigned_(isSigned), 
        opcode_(opcode){
		GADGETLIB_ASSERT(unpacked_.size() == partials_.size(), "MultPack: array size equality");
}

GadgetPtr MultiplicationPacking_Gadget::create(ProtoboardPtr pb,
											const Algebra::VariableArray& unpacked,
											const Algebra::VariableArray& partials,
											const Algebra::Variable& packed,
											const bool isSigned,
											const PackingMode packingMode,
											const Opcode opcode){
	GadgetPtr pGadget(new MultiplicationPacking_Gadget(pb, unpacked, partials, packed, isSigned, packingMode, opcode));
	pGadget->init();
	return pGadget;
}

void MultiplicationPacking_Gadget::init(){}

void MultiplicationPacking_Gadget::generateConstraints(){
	Algebra::LinearCombination prev = unpacked_[0] * Algebra::xFE() + Algebra::one() + unpacked_[0];
	unsigned int i; Algebra::FElem g_2_i; Algebra::LinearCombination c;
	for (i = 1; i < unpacked_.size()-1 ; i++){
		g_2_i = getGenerator2toTheNthPower(i);
		c = ((unpacked_[i] * g_2_i) + (Algebra::one() + unpacked_[i]));
		pb_->addGeneralConstraint(partials_[i] + prev * c, "partial[i] = prev*(g ^ (2 ^ i * unpacked[i]))", opcode_);
		prev = partials_[i];
	}
	g_2_i = isSigned_ ? getGenerator2toTheNthPower(i).inverse() : getGenerator2toTheNthPower(i);
	c = ((unpacked_[i] * g_2_i) + (Algebra::one() + unpacked_[i]));
	pb_->addGeneralConstraint(packed_ + prev * c, "packed_ = prev*g^(2^(regSize-1) * unpacked[(regSize-1)])", opcode_);
	if (packingMode_ == PackingMode::PACK)
		return;
	GADGETLIB_ASSERT(packingMode_ == PackingMode::UNPACK,
		"MultiplicationPacking_Gadget: created with unknown packing mode.");
	for (unsigned int i = 0; i < unpacked_.size(); i++)
		enforceBooleanity(unpacked_[i], opcode_);
}

void MultiplicationPacking_Gadget::generateWitness(int n){
	if (PackingMode::UNPACK == packingMode_){
		GADGETLIB_ASSERT(n >= 0, "MultiplicationPacking_Gadget: n should be >= 0.");
		unsigned int numberOfDigits = n == 0 ? 0 : floor(log2(n)) + 1;
		GADGETLIB_ASSERT(numberOfDigits <= unpacked_.size(), "MultiplicationPacking_Gadget: number of bits in n should be less than the unpakced.size()");
		for (unsigned int i = 0; i < unpacked_.size(); ++i){
			pb_->val(unpacked_[i]) = (n & 1u) ? Algebra::one() : Algebra::zero();
			n >>= 1u;
		}
		//return; //don't return!
	}
	else {
		GADGETLIB_ASSERT(packingMode_ == PackingMode::PACK,
			"MultiplicationPacking_Gadget: created with unknown packing mode.");
	}
	Algebra::FElem	prev = val(unpacked_[0]) * Algebra::xFE() + (Algebra::one() + val(unpacked_[0]));
	unsigned int i; Algebra::FElem g_2_i, c;
	for (i = 1; i < unpacked_.size()-1 ; i++){
		g_2_i = getGenerator2toTheNthPower(i);
		c = ((val(unpacked_[i]) * g_2_i) + (Algebra::one() + val(unpacked_[i])));
		prev = pb_->val(partials_[i]) = prev*c;
	}
	if (PackingMode::PACK == packingMode_){
		g_2_i = isSigned_ ? getGenerator2toTheNthPower(i).inverse() : getGenerator2toTheNthPower(i);
		c = ((val(unpacked_[i]) * g_2_i) + (Algebra::one() + val(unpacked_[i])));
		pb_->val(packed_) = prev*c;
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************           DoubleMultiplicationPacking Gadget               ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
DoubleMultPack_Gadget::DoubleMultPack_Gadget(ProtoboardPtr pb,
	const Algebra::VariableArray& unpacked1,
	const Algebra::VariableArray& unpacked2,
	const Algebra::VariableArray& partials1,
	const Algebra::VariableArray& partials2,
	const Algebra::Variable& packed,
	const bool isSigned,
	const Opcode opcode)
	: Gadget(pb), unpacked1_(unpacked1), unpacked2_(unpacked2),
	  partials1_(partials1), partials2_(partials2), packed_(packed), isSigned_(isSigned), opcode_(opcode){
	GADGETLIB_ASSERT(unpacked1_.size() == unpacked2_.size(), "DMultPack: array size equality");
	GADGETLIB_ASSERT(unpacked1_.size() == partials1_.size(), "DMultPack: array size equality");
	GADGETLIB_ASSERT(unpacked1_.size() == partials2_.size(), "DMultPack: array size equality");
}

GadgetPtr DoubleMultPack_Gadget::create(ProtoboardPtr pb,
											const Algebra::VariableArray& unpacked1,
											const Algebra::VariableArray& unpacked2,
											const Algebra::VariableArray& partials1,
											const Algebra::VariableArray& partials2,
											const Algebra::Variable& packed,
											const bool isSigned,
											const Opcode opcode){
	GadgetPtr pGadget(new DoubleMultPack_Gadget(pb, unpacked1, unpacked2, partials1, partials2, packed, isSigned, opcode));
	pGadget->init();
	return pGadget;
}

void DoubleMultPack_Gadget::init(){}

void DoubleMultPack_Gadget::generateConstraints(){
	unsigned int j; Algebra::FElem g_2_i; Algebra::LinearCombination c;
	Algebra::LinearCombination prev = unpacked1_[0] * Algebra::xFE() + (Algebra::one() + unpacked1_[0]);
	for (unsigned int i = 1; i < unpacked1_.size(); i++){
		g_2_i = getGenerator2toTheNthPower(i);
		c = ((unpacked1_[i] * g_2_i) + (Algebra::one() + unpacked1_[i]));
		pb_->addGeneralConstraint(partials1_[i] + prev*c, "DMultPack partials1", opcode_);
		prev = partials1_[i];
	}
	for (j = 0; j < unpacked2_.size()-1 ; j++){
		g_2_i = getGenerator2toTheNthPower(unpacked1_.size()+j);
		c = ((unpacked2_[j] * g_2_i) + (Algebra::one() + unpacked2_[j]));
		pb_->addGeneralConstraint(partials2_[j] + prev*c, "DMultPack partials2", opcode_);
		prev = partials2_[j];
	}
	g_2_i = isSigned_ ?
		getGenerator2toTheNthPower(unpacked1_.size()+j).inverse() :
		getGenerator2toTheNthPower(unpacked1_.size()+j);
	c = ((unpacked2_[j] * g_2_i) + (Algebra::one() + unpacked2_[j]));
	pb_->addGeneralConstraint(packed_ + prev*c, "DMultPack result", opcode_);
}

void DoubleMultPack_Gadget::generateWitness(){
	Algebra::FElem prev = val(unpacked1_[0]) * Algebra::FElem(getGF2E_X()) + (Algebra::one() + val(unpacked1_[0]));
	for (unsigned int i = 1; i < unpacked1_.size(); i++){
		const Algebra::FElem g_2_i = Algebra::FElem(getGenerator2toTheNthPower(i));
		const Algebra::FElem c = ((val(unpacked1_[i]) * g_2_i) + (Algebra::one() + val(unpacked1_[i])));
		prev = pb_->val(partials1_[i]) = prev*c;
	}
	for (unsigned int j = 0; j < unpacked2_.size()-1; j++){
		const Algebra::FElem g_2_i = Algebra::FElem(getGenerator2toTheNthPower(unpacked1_.size()+j));
		const Algebra::FElem c = ((val(unpacked2_[j]) * g_2_i) + (Algebra::one() + val(unpacked2_[j])));
		prev = pb_->val(partials2_[j]) = prev*c;
	}
	//pb_->val(packed_) = prev*c; //don't? TODO: replace with assert
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                  Multiplication Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
Multiplication_Gadget::Multiplication_Gadget(ProtoboardPtr pb,
	const Algebra::VariableArray& unpacked1,
	const Algebra::VariableArray& unpacked2,
	const Algebra::VariableArray& partials1,
	const Algebra::VariableArray& partials2,
	const bool isSigned,
	const Opcode opcode)
	:
        Gadget(pb),
        unpacked1_(unpacked1),
        unpacked2_(unpacked2),
        partials1_(partials1),
        partials2_(partials2),
        opcode_(opcode),
        isSigned_(isSigned),
        idxLast(unpacked1.size() - 1){
	GADGETLIB_ASSERT(unpacked1_.size() == unpacked2_.size(), "Mult: array size equality");
	GADGETLIB_ASSERT(unpacked1_.size() == partials1_.size(), "Mult: array size equality");
	GADGETLIB_ASSERT(unpacked1_.size() == partials2_.size(), "Mult: array size equality");
}

GadgetPtr Multiplication_Gadget::create(ProtoboardPtr pb,
											const Algebra::VariableArray& unpacked1,
											const Algebra::VariableArray& unpacked2,
											const Algebra::VariableArray& partials1,
											const Algebra::VariableArray& partials2,
											const bool isSigned,
											const Opcode opcode){
	GadgetPtr pGadget(new Multiplication_Gadget(pb, unpacked1, unpacked2, partials1, partials2, isSigned, opcode));
	pGadget->init();
	return pGadget;
}

void Multiplication_Gadget::init(){
	multPack1_g_ = MultiplicationPacking_Gadget::create(pb_, unpacked1_, partials1_, partials1_[idxLast], isSigned_, PackingMode::PACK, opcode_);
}

void Multiplication_Gadget::generateConstraints(){
	multPack1_g_->generateConstraints();
	const Algebra::Variable h = partials1_[idxLast];
	Algebra::CircuitPolynomial c; //not LinearCombination
	if (isSigned_){
		const Algebra::Variable invh = partials1_[0];
		pb_->addGeneralConstraint(h * invh + Algebra::one(), "sMult inv", opcode_);
		c = unpacked2_[idxLast] * invh + Algebra::one() + unpacked2_[idxLast];
	}
	else {
		c = unpacked2_[idxLast] * h + Algebra::one() + unpacked2_[idxLast];
	}
	pb_->addGeneralConstraint(partials2_[idxLast] + c, "Mult partials2 init", opcode_);
	for (int i = idxLast - 1; i >= 0; --i){
		c = unpacked2_[i] * h + Algebra::one() + unpacked2_[i];
		pb_->addGeneralConstraint(partials2_[i] + partials2_[i + 1] * partials2_[i + 1] * c, "Mult partials2", opcode_);
	}
}

void Multiplication_Gadget::generateWitness(){
	std::dynamic_pointer_cast<MultiplicationPacking_Gadget>(multPack1_g_)->generateWitness();
	const Algebra::FElem h = val(partials1_[idxLast]);
	Algebra::FElem c;
	const Algebra::Variable invh = partials1_[0];
	if (isSigned_) {
		c = h.inverse();
		pb_->val(invh) = c;
	}
	else {
		c = h;
		pb_->val(invh) = Algebra::zero();
	}
	pb_->val(partials2_[idxLast]) = Algebra::one() != val(unpacked2_[idxLast]) ?  Algebra::one() : c;
	for (int i = idxLast - 1; i >= 0; --i){
		c = Algebra::one() == val(unpacked2_[i]) ? h : Algebra::one();
		pb_->val(partials2_[i]) = val(partials2_[i + 1])*val(partials2_[i + 1])*c;
	}
}

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                         Greater Gadget                     ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/

GreaterEqual_Gadget::GreaterEqual_Gadget(ProtoboardPtr pb,
								const Algebra::UnpackedWord& var1,
								const Algebra::UnpackedWord& var2,
								const Algebra::UnpackedWord& flags,
								const Algebra::FlagVariable& flagGEQ,
								const bool isSigned,
								const Opcode opcode)
	: Gadget(pb), var1_(var1), var2_(var2), flags_(flags), flagGEQ_(flagGEQ), isSigned_(isSigned), opcode_(opcode) {
	GADGETLIB_ASSERT(var2_.size() == var1_.size(), "Greater Gadget: The length of the unpacked representations should be equal");
	GADGETLIB_ASSERT(flags_.size() == var1_.size(), "Greater Gadget: size of the flags array should be equal to unpacked representation size");
}

void GreaterEqual_Gadget::init(){}

GadgetPtr GreaterEqual_Gadget::create(ProtoboardPtr pb,
								const Algebra::UnpackedWord& var1,
								const Algebra::UnpackedWord& var2,
								const Algebra::UnpackedWord& flags,
								const Algebra::FlagVariable& flagGEQ,
								const bool isSigned,
								const Opcode opcode){
	GadgetPtr pGadget(new GreaterEqual_Gadget(pb, var1, var2, flags, flagGEQ, isSigned, opcode));
	pGadget->init();
	return pGadget;
}

void GreaterEqual_Gadget::generateConstraints(){
	/* Break Down of the constraints:
	flags_[unpacked.size()] = g;

	for all i from unpacked.size() - 1 to 0:
	(1)	flags_[i] +
	(2)				+ flags_[i+1] * (1 + flags_[i+1]) *(g* (1 + g))^-1 *[ g*(a_i + b_i + 1) + a_i*(b_i + 1)]
	(3)				+ flags_[i+1] * (g + flags_[i+1]) *(1+g) ^ -1
	
	if flag[i+1] = 0 then flag[i] = 0. Lines(1) and (2) are immediately 0
	if flag[i+1] = 1 then flag[i] = 1. Line (1) is 0 Line (2) equals (1*(g+1)*(1+g)^-1) == 1
	if flag[i+1] = g and a_i = b_i then flag[i] = g. Line (2) is 0 and line (1) equals (1*(1+g) * (1+g)^-1) * g = g
	if flag[i+1] = g and a_i = 1 and b_1=0 then flag[i] = 1. Line (2) is 0 and line (1) equals (1*(1+g) * (1+g)^-1) *[1] = 1
	if flag[i+1] = g and a_i = 0 and b_1=1 then flag[i] = 0. Line (2) is 0 and line (1) equals (1*(1+g) * (1+g)^-1) *[0] = 1
	*/
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	const Algebra::FElem invOnePlusG = (g + Algebra::one()).inverse(); // (1+g)^-1
	const Algebra::FElem invGPlusGG = (g*(g + Algebra::one())).inverse(); // (g*(1+g))^-1
	int first = flags_.size() - 1 - int(isSigned_);
	Algebra::Variable currFlag;
	for (int i = first; i >= 0; i--){
		Algebra::CircuitPolynomial tmpPoly = g * (var1_[i] + var2_[i] + Algebra::one()) + (var1_[i] * (var2_[i] + Algebra::one()));
		if (first == i) {
			pb_->addGeneralConstraint(flags_[i] + tmpPoly, std::to_string(i) + ":flags", opcode_); //": flag[i] + g*(var1_[i] + var2_[i] + 1) + var1_[i] * (var2_[i] + 1)", opcode_);
			continue;
		}
		tmpPoly = tmpPoly * ((flags_[i + 1] * (Algebra::one() + flags_[i + 1])) * invGPlusGG);
		tmpPoly = tmpPoly + flags_[i + 1] * (flags_[i + 1] + g) * invOnePlusG;
		if (i > 0)
			currFlag = flags_[i];
		else {
			currFlag = flagGEQ_;
			if (isSigned_) {
				++first;
				tmpPoly = tmpPoly*(Algebra::one()+var1_[first]+var2_[first]) + (Algebra::one()+var1_[first])*var2_[first];
			}
		}
		pb_->addGeneralConstraint(currFlag + tmpPoly, std::to_string(i) + ":flags", opcode_); //": flag[i] + flag[i+1]*(flag[i+1] + 1)*(g*(g+1))^-1[g*(var1_[i]+var2_[i] +1) +var1_[i]*(var2_[i] + 1)] + flag[i+1]*(flag[i+1] + g) * (1 + g)^-1", opcode_);
	}
}

void GreaterEqual_Gadget::generateWitness(){
	const Algebra::FElem g = Algebra::FElem(getGF2E_X());
	int i = flags_.size() - 1 - int(isSigned_);
	pb_->val(flags_[i]) = val(var1_[i])==val(var2_[i]) ? g : val(var1_[i]);
	Algebra::Variable currFlag;
	Algebra::FElem a, b;
	while (--i >= 0){
		const Algebra::FElem prevFlag = pb_->val(flags_[i + 1]);
		if (i > 0)
			currFlag = flags_[i];
		else {
			if (isSigned_) {
				a = pb_->val(var1_[flags_.size() - 1]);
				b = pb_->val(var2_[flags_.size() - 1]);
				if (a != b) {
					pb_->val(flagGEQ_) = b; //positive b -> flag=0, negative b -> flag=1
					return;
				}
			}
			currFlag = flagGEQ_;
		}
		if (prevFlag == Algebra::one() || prevFlag == Algebra::zero()){
			pb_->val(currFlag) = prevFlag;
		}
		else{ //prevFlag == g;
			a = pb_->val(var1_[i]);
			b = pb_->val(var2_[i]);
			if (a == b){
				pb_->val(currFlag) = prevFlag;
			}
			else{ // a != b
				pb_->val(currFlag) = a; // if a = 1 and b = 0 -> flags = 1. 
				                        // if a = 0 and b = 1 -> flags = 0.
			}
		}
	}
}

} // namespace gadgetlib
