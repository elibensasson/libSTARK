/** @file
*****************************************************************************
Implementation of Protoboard, a "memory manager" for building arithmetic constraints
*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#include <gadgetlib/protoboard.hpp>
#include <algebraLib/variable_operators.hpp>
#include <algebraLib/variable.hpp>


namespace gadgetlib {

unsigned int Protoboard::numConstraints() const{
	return numConstratins_;
}

Algebra::Variable::set Protoboard::getUsedVariables() const{
	Algebra::Variable::set constraintsVariables = constraintSystem_.getUsedVariables();
	for (ConstraintSystem opcodeConstraintSystem : opcodeConstraintSystem_){
		Algebra::Variable::set opcodeConstraintVariable = opcodeConstraintSystem.getUsedVariables();
		constraintsVariables.insert(opcodeConstraintVariable.begin(), opcodeConstraintVariable.end());
	}
	return constraintsVariables;
}

Algebra::FElem& Protoboard::val(const Algebra::Variable& var){
	return assignment_[var];
}

const Algebra::FElem Protoboard::val(const Algebra::Variable& var) const{
	const Algebra::FElem& retval = assignment_.at(var);
	return retval;
}

const Algebra::FElem Protoboard::val(const Algebra::LinearCombination& lc) const{
	return lc.eval(assignment_);
}

void Protoboard::addGeneralConstraint(const Polynomial& a,
										const ::std::string& name,
										Opcode opcode){
	if (opcode == Opcode::MEMORY){
		addMemoryConstraint(a, name);
		return;
	}
	if (opcode == Opcode::NONE){
		constraintSystem_.addConstraint(a, name);
		return;
	}
	else{
		int value = int(opcode);
		ConstraintSystem& cs = opcodeConstraintSystem_[value];
		cs.addConstraint(a, name);
	}
}

void Protoboard::addMemoryConstraint(const Polynomial& a,
									const ::std::string& name){
	memoryConstraintSystem_.addConstraint(a, name);
}


bool Protoboard::isSatisfied(Opcode opcode,const PrintOptions& printOnFail){
	if (opcode == Opcode::NONE){
		return constraintSystem_.isSatisfied(assignment_, printOnFail);
	}
	else{
		if (opcode == Opcode::MEMORY){
			return memoryConstraintSystem_.isSatisfied(assignment_, printOnFail);
		}
		else{
			int value = int(opcode);
			return opcodeConstraintSystem_[value].isSatisfied(assignment_, printOnFail);
		}
	}
	

}

	
	//Ariel: Changed implementation to vector of LC's for faster evaluation
	void Protoboard::enforceBooleanity(const Algebra::Variable& var, Opcode opcode){
		Algebra::LinearCombination l1(Algebra::one() + var);
		Polynomial constraint({ l1, Algebra::LinearCombination(var) });
		addGeneralConstraint(constraint, "" + var.name() + "* ( 1 + " + var.name() + ")", opcode);

	}


ConstraintSystem Protoboard::constraintSystem(Opcode opcode) const{
	if (opcode == Opcode::NONE){
		return constraintSystem_;
	}
	else if (opcode == Opcode::MEMORY){
		return memoryConstraintSystem_;
	}
	else{
		return opcodeConstraintSystem_[int(opcode)];
	}
}

void Protoboard::multiplyConstraintSystem(Opcode opcode, const Algebra::CircuitPolynomial& mul){
	ConstraintSystem cs = constraintSystem(opcode);
	for (Constraint constraint : cs.getConstraints()){
		addGeneralConstraint(mul * constraint.constraint(), constraint.asString() + "* selectorProgram", Opcode::NONE);
	}
}

void Protoboard::clearConstraintSystem(Opcode opcode){
	if (opcode == Opcode::NONE){
		constraintSystem_ = ConstraintSystem();
	}
	else if(opcode ==Opcode::MEMORY){
		memoryConstraintSystem_ = ConstraintSystem();
	}
	else {
		opcodeConstraintSystem_[int(opcode)] = ConstraintSystem();
	}
}

void Protoboard::setNewIndicesForTranslation(::std::vector<Algebra::Variable> translationVector){	
	// Init new indecis
	translation_ = translationVector;
	for (unsigned int i = 0; i < translation_.size(); i++){
		if (i > 0 && (translation_[i - 1] == translation_[i])){
			translation_[i].setNewIndex(translation_[i - 1].getNewIndex());
			continue;
		}
		Algebra::Variable& v = translation_[i];
		v.setNewIndex(i);
		old2newID_[v.getIndex()] = i;

	}
	constraintSystem_.setNewIndices(old2newID_);
	memoryConstraintSystem_.setNewIndices(old2newID_);
}


void Protoboard::addDegreeTranslation(const Algebra::FElem& elem, unsigned int degree){
	// Add assert that checks if g^degree  = elem
	degreeTranslation_.insert(std::pair<Algebra::FElem, int>(elem, degree));
}

unsigned int Protoboard::getDegreeOfFElem(const Algebra::FElem& elem){
	return degreeTranslation_.at(elem);
}

void Protoboard::storeValue(const Algebra::FElem& address, const Algebra::FElem& value){
	memory_[address] = value;
}

Algebra::FElem Protoboard::loadValue(const Algebra::FElem& address){
	std::map<Algebra::FElem, Algebra::FElem>::iterator it = memory_.find(address);
	GADGETLIB_ASSERT(it != memory_.end(), "Address(Memory) doesn't hold any value");
	return it->second;
}

void Protoboard::addBoundaryConstraint(const Algebra::Variable& var, const size_t timeStamp, const Algebra::FElem& assignment){
	addBoundaryVariable(var);
	addBoundaryTimestamp(timeStamp);
	addBoundaryAssignment(assignment);
}

///*************************************************************************************************/
///*************************************************************************************************/
///*******************                                                            ******************/
///*******************                     class ProtoboardParams                 ******************/
///*******************                                                            ******************/
///*************************************************************************************************/
///*************************************************************************************************/

ProtoboardParams::~ProtoboardParams() {}

} // namespace gadgetlib
