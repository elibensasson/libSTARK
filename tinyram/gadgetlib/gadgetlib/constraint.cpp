/** @file
 *****************************************************************************
 Implementation of the Constraint class.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <algorithm>
#include <cassert>
#include <set>
#include <iostream>
#include <memory>
#include "constraint.hpp"

using ::std::string;
using ::std::vector;
using ::std::set;
using ::std::cout;
using ::std::cerr;
using ::std::endl;
using ::std::shared_ptr;

namespace gadgetlib {

typedef Algebra::CircuitPolynomial Polynomial;


bool Constraint::isSatisfied(const Algebra::VariableAssignment& assignment,
							const PrintOptions& printOnFail){

	bool aEval = constraint_.isSatisfied(assignment);
	if (aEval == false){
		if (printOnFail == PrintOptions::DBG_PRINT_IF_NOT_SATISFIED) {
			cerr << "Constraint named:" << name_ << "not satisfied" << endl;
			cerr << "Expecting: " << aEval << endl;
			cerr << "Variable assignments are:" << endl;
			const Algebra::Variable::set varSet = getUsedVariables();
			for (const Algebra::Variable& var : varSet) {
				cerr << var.name() << ": " << assignment.at(var).asString() << endl;
			}
		}
	}
	return true;
}


void ConstraintSystem::addConstraint(Polynomial c, std::string name) {
	Constraint newConstraint(c, name);
	constraintsPtrs_.emplace_back(newConstraint);
}

bool ConstraintSystem::isSatisfied(const Algebra::VariableAssignment& assignment,
                                   const PrintOptions& printOnFail) const {
    for(size_t i = 0; i < constraintsPtrs_.size(); ++i) {
		if (!(constraintsPtrs_[i].constraint().isSatisfied(assignment))){
		#ifdef _DEBUG
			std::cout << "Wrong: " << constraintsPtrs_[i].asString() << std::endl;
//			std::cout << constraintsPtrs_[i].constraint().eval(assignment) << std::endl;
//			std::cout << "Constraint: " << constraintsPtrs_[i].constraint().asString() << std::endl;
		#endif
			return false;
        }
    }
    return true;
}

Algebra::Variable::set ConstraintSystem::getUsedVariables() const {
    Algebra::Variable::set retSet;
    for(auto& pConstraint : constraintsPtrs_) {
		const Algebra::Variable::set curSet = pConstraint.getUsedVariables();
        retSet.insert(curSet.begin(), curSet.end());
    }
    return retSet;
}

void ConstraintSystem::setNewIndices(std::map<Algebra::VarIndex_t, Algebra::VarIndex_t>& old2New) {
	for (auto& pConstraint : constraintsPtrs_){
		pConstraint.setNewIndices(old2New);
	}
}
} // namespace gadgetlib
