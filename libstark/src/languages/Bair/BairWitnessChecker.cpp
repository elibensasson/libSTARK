#include "BairWitnessChecker.hpp"
#include "common/Utils/TaskReporting.hpp"

using std::vector;
using Algebra::FieldElement;

namespace libstark {

static bool isPermutation(size_t const numElements, const BairWitness::permutation_t& mapping){
	//We check the mapping is a bijection (permutation)
	//by checking that it is injective
	
	//inImage[i] will be set to 'true' iff
	//there exists j that is mapped to it
	vector<bool> inImage(numElements);
	
	//initialization
	for(size_t i=0; i< numElements; i++) inImage[i] = false;

	//Check if is an injection
	for(size_t i=0; i< numElements; i++){
		size_t img = mapping.getElementByIndex(i);

		//check the image is inside {0 ... numElements-1}
		if ((img < 0) || (img >= numElements)) return false;

		//check no element was mapped to img before (validate injectivity)
		if (inImage[img]) return false;

		//mark in image
		inImage[img] = true;
	}
	
	return true;
}

static bool isSatisfied(const ConstraintSys& sys, const BairWitness::color_t& c1, const BairWitness::color_t& c2){
	vector<FieldElement> assignment(c1.begin(),c1.begin() + (sys.numVars()/2));
    assignment.insert(assignment.end(),c2.begin(),c2.begin() + (sys.numVars()/2));
	return sys.verify(assignment);
}

bool BairWitnessChecker::verify(const BairInstance& instance, const BairWitness& witness){
	TASK("Executing Bair determenistic witness checker");
    if(!verify_permutation(instance,witness)) return false;
	if(!verify_constraintsAssignment(instance,witness))return false;
    if(!verify_constraintsPermutation(instance,witness))return false;
	if(!verify_boundary(instance,witness))return false;
    return true;
}

bool BairWitnessChecker::verify_permutation(const BairInstance& instance, const BairWitness& witness){
	TASK("verifying mapping is permutation");
	size_t numElements = instance.domainSize();
	return isPermutation(numElements,witness.permutation());
}

bool BairWitnessChecker::verify_constraintsAssignment(const BairInstance& instance, const BairWitness& witness){
	TASK("verifying assignment constraints");
	size_t domainSize = instance.domainSize();
	const ConstraintSys& constraintsAssignment = instance.constraintsAssignment();
    for (size_t i=0; i < domainSize-1; i++){
        BairWitness::color_t c1 = witness.get_color(i);
        BairWitness::color_t c2 = witness.get_color(i+1);
        bool currSatisfied = isSatisfied(constraintsAssignment,c1,c2);
        if (!currSatisfied) {
            return false;
        }
    }

	return true;
}

bool BairWitnessChecker::verify_constraintsPermutation(const BairInstance& instance, const BairWitness& witness){
	TASK("verifying permutation constraints");
	size_t domainSize = instance.domainSize();
	const ConstraintSys& constraintsPermutation = instance.constraintsPermutation();
	const BairWitness::permutation_t& permutation = witness.permutation();
    for (size_t i=0; i < domainSize-1; i++){
        BairWitness::color_t c1 = witness.get_color(i);
        size_t perm_next = permutation.getElementByIndex(i);
		BairWitness::color_t c2 = witness.get_color(perm_next);
        bool currSatisfied = isSatisfied(constraintsPermutation,c1,c2);
        if (!currSatisfied) {
            return false;
        }
    }

	return true;
}

bool BairWitnessChecker::verify_boundary(const BairInstance& instance, const BairWitness& witness){
	TASK("verifying boundary constraints");

	for(const auto& p : instance.boundaryConstraints()){
		const BairInstance::point_t inputLocation = p.first;
		const size_t vector_index = inputLocation.first;
		const size_t element_index = inputLocation.second;

		const  BairWitness::color_t color = witness.get_color(vector_index);
		assert(color.size() == instance.vectorsLen());
		if(!(p.second == color[element_index])) {
			return false;
		}
	}
	return true;
}

} // namespace libstark
