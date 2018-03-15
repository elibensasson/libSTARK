#include "AcspWitnessChecker.hpp"
#include "common/Utils/TaskReporting.hpp"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;
using Algebra::FieldElement;

namespace libstark {


/**
 * @class notRootPredicate
 * @brief A predicate class
 * that checks if a given field element is
 * not a root of some internally kept polynomial
 */
class notRootPredicate : public ::Algebra::FieldElementPredicate {
public:
	/**
	 * @brief   The constructor
	 * @param   instance that contains a constraint polynomial
	 * \f$P:\mathbb{F}^n \to \mathbb{F}\f$ and a neighbors polynomials
	 * vector \f$\vec{N}:\mathbb{F}^n \to  \mathbb{F}^n\f$
	 * @param   witness that contains an assignment polynomial \f$A:\mathbb{F} \to \mathbb{F}\f$
	 */
	notRootPredicate(const AcspInstance& instance, const AcspWitness& witness): instance_(instance), witness_(instance.prepareForWitnessChecker(witness)){};

	/**
	 * @brief   checks that the field element is not a root of some polynomial
	 * @param   x the field element to check
	 * @return  True iff \f$ 0 \neq P(x,A(N_1(x),A(N_2(x),\dots,A(N_n(x)) \f$
	 */
	bool test(const ::Algebra::FieldElement& x)const {
        vector<FieldElement> assignment;
        assignment.push_back(x);
        
        for(size_t wIndex = 0; wIndex < witness_.size(); wIndex++){
            for(const auto& n : instance_.neighborPolys()[wIndex]){
                assignment.push_back(witness_[wIndex]->eval(n->eval(x)));
            }
        }
        return !instance_.constraintPoly().isRoot(assignment);
    }

private:
    const AcspInstance& instance_;
    const std::vector<std::unique_ptr<AcspInstance::uniPoly>>  witness_;
};



bool AcspWitnessChecker::verify(const AcspInstance& instance, const AcspWitness& witness){
	
	TASK("Executes deterministic  Acsp witness checker");
	
	if (!verify_vanishing(instance,witness)) return false;

	if (!verify_witness_degree(instance,witness)) return false;
		
	if (!verify_boundary(instance,witness)) return false;

    return true;
}

bool AcspWitnessChecker::verify_vanishing(const AcspInstance& instance, const AcspWitness& witness){
	TASK("Tests vanishing constraint");

	const AcspInstance::set& vanisinigSet = instance.vanishingSet();
	notRootPredicate* predicate = new notRootPredicate(instance,witness);
	
	std::unique_ptr<const ::Algebra::FieldElementPredicate> pred_ptr(predicate);
	return !(vanisinigSet.exist(pred_ptr));

}

bool AcspWitnessChecker::verify_witness_degree(const AcspInstance& instance, const AcspWitness& witness){
	TASK("Tests witness degree constraint");
    
    bool isOK = true;

    for(size_t wIndex = 0; wIndex < witness.assignmentPolys().size(); wIndex++){
        isOK &= !(instance.witnessDegreeBound()[wIndex] < witness.assignmentPolys()[wIndex]->getDegree());
    }

    return isOK;
}

bool AcspWitnessChecker::verify_boundary(const AcspInstance& instance, const AcspWitness& witness){
	TASK("Tests boundary constraints");

    for(size_t wIndex = 0; wIndex < witness.assignmentPolys().size(); wIndex++){
        const AcspWitness::polynomial& assignment = *(witness.assignmentPolys()[wIndex]);

        for(const auto& p : instance.boundaryConstraints()[wIndex]){
            if(assignment.eval(p.first) != p.second) {
                return false;
            }
        }
    }
	return true;
}

} // namespace libstark
