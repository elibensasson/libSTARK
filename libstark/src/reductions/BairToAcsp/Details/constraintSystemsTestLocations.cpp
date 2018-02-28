#include "constraintSystemsTestLocations.hpp"

namespace libstark{
namespace BairToAcsp{
    
CS_testLocations::CS_testLocations(const common& commonDef){
    size_t currIndex = 0;

    for(size_t i=0; i< commonDef.getConstraintsPi().size();i++){
        indexesPermutation_[i] = currIndex++;
    }

    for(size_t i=0; i< commonDef.getConstraintsChi().size();i++){
        indexesAssignment_[i] = currIndex++;
    }
}

size_t CS_testLocations::indexOfConstraint_Assignment(polynomialIndicator_t poly)const{
    return indexesAssignment_.at(poly);
}

size_t CS_testLocations::indexOfConstraint_Permuation(polynomialIndicator_t poly)const{
    return indexesPermutation_.at(poly);
}

} //namespace BairToAcsp
} //namespace libstark
