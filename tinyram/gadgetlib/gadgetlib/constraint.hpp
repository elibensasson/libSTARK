/** @file
 *****************************************************************************
 Declaration of the Constraint class.

 A constraint is an algebraic equation which can be either satisfied by an assignment,
 (the equation is true with that assignment) or unsatisfied. For instance the rank-1
 constraint (X * Y = 15) is satisfied by {X=5 Y=3} or {X=3 Y=5}
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef GADGETLIB3_CONSTRAINT_HPP_
#define GADGETLIB3_CONSTRAINT_HPP_

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <algebraLib/variable.hpp>
#include <algebraLib/CircuitPolynomial.hpp>
#include <algebraLib/PolynomialInterface.hpp>



namespace gadgetlib {

using Algebra::FieldElement;
typedef Algebra::CircuitPolynomial Polynomial;

enum class PrintOptions {
    DBG_PRINT_IF_NOT_SATISFIED,
    DBG_PRINT_IF_TRUE,
    DBG_PRINT_IF_FALSE,
    NO_DBG_PRINT
};

class Constraint{
	Polynomial constraint_;
	std::string name_;

public:
	Constraint(Polynomial polynomial, std::string name = "") :constraint_(polynomial), name_(name){};
	std::string asString() const { return name_; }
	Algebra::Variable::set getUsedVariables() const { return constraint_.getUsedVariables(); }
	Polynomial constraint() const { return constraint_; }
	bool isSatisfied(const Algebra::VariableAssignment& assignment,
					 const PrintOptions& printOnFail = PrintOptions::NO_DBG_PRINT);
	void setNewIndices(std::map<Algebra::VarIndex_t, Algebra::VarIndex_t>& old2New) { constraint_.setNewIndices(old2New); }
	void getConsts(map<FieldElement, unsigned long long, Algebra::classCompElements>& FElemMap) const { return constraint_.getConsts(FElemMap); }
};

/*************************************************************************************************/
/*************************************************************************************************/
/*******************                                                            ******************/
/*******************                   class ConstraintSystem                   ******************/
/*******************                                                            ******************/
/*************************************************************************************************/
/*************************************************************************************************/
class ConstraintSystem {
protected:
    //typedef ::std::shared_ptr<Polynomial> ConstraintPtr;
    ::std::vector<Constraint> constraintsPtrs_;

public:
    ConstraintSystem() : constraintsPtrs_() {};

    /**
        Checks if all constraints are satisfied by an assignment.
        @param[in] assignment  - An assignment of field elements for each variable.
        @param[in] printOnFail - when set to true, an unsatisfied constraint will print to stderr
                                 information explaining why it is not satisfied.
        @returns true if constraint is satisfied by the assignment
    **/
    bool isSatisfied(const Algebra::VariableAssignment& assignment,
                     const PrintOptions& printOnFail = PrintOptions::NO_DBG_PRINT) const;

	void addConstraint(const Polynomial c, std::string name = "");
    Algebra::Variable::set getUsedVariables() const;

	::std::vector<Constraint> getConstraints(){
		return constraintsPtrs_;
	}

    typedef std::vector< std::unique_ptr<Algebra::PolynomialInterface> > PolyPtrSet;
    /// Required for interfacing with BREX. Should be optimized in the future
    PolyPtrSet getConstraintPolynomials() const {
        PolyPtrSet retset;
		for(const Constraint& pConstraint : constraintsPtrs_) {
		    retset.push_back(pConstraint.constraint().clone());  
        }
        return retset;
    }

	size_t getNumberOfConstraints() { return constraintsPtrs_.size(); }
	void setNewIndices(std::map<Algebra::VarIndex_t, Algebra::VarIndex_t>& old2New);
    friend class GadgetLibAdapter;
}; // class ConstraintSystem

/***********************************/
/***   END OF CLASS DEFINITION   ***/
/***********************************/

} // namespace gadgetlib

#endif //GADGETLIB3_CONSTRAINT_HPP_
