#include "instanceMappings.hpp"

namespace libstark{
namespace BairToAcsp{

using Algebra::FieldElement;
    
instanceMappings::instanceMappings(const common& commonInfo):
    commonMappings(commonInfo),
    commonInfo_(commonInfo){};

FieldElement instanceMappings::mapVariable(const size_t varId)const{
    const auto location = commonInfo_.getVarLocation(varId);
    FieldElement res;

    if (location.inPerm){
        res = mapPermutationElement_spaceElement(0,2*location.index);
    }
    else{
        res = mapNonPermutationVariable_spaceElement(location.index); 
    }

    return res;
}

commonMappings::witnessElement_t instanceMappings::mapVariable_witnessElement(const size_t varId)const{
    const auto location = commonInfo_.getVarLocation(varId);
    witnessElement_t res;

    if (location.inPerm){
        res = mapPermutationElement_witnessElement(0,2*location.index);
    }
    else{
        res = mapNonPermutationVariable_witnessElement(location.index); 
    }

    return res;
}

FieldElement instanceMappings::mapNonPermutation_zeroRow(const size_t elementId)const{
    return mapNonPermutationElement(elementId);
}

FieldElement instanceMappings::mapNonPermutation_lastRow(const size_t elementId)const{
    return mapNonPermutationElement (elementId) + getLastRowIndicator();
}

FieldElement instanceMappings::getLastRowIndicator()const{
    ///
    /// Let \f$ p \f$ be the modulus polynomial used for row Id embedding.
    /// Let \f$ x \f$ be the generator, so the last row id is mapped to \f$ \alpha = x^{-1} \f$,
    /// in particular, \f$ x \cdot \alpha \equiv 1 ( \mod p ) \f$
    /// (\f$ \alpha \f$ is a polynomial with degree strictly less than \f$ \deg p\f$).
    /// We conclude that \f$ x \cdot \alpha = p + 1 \f$,
    /// hence \f$ \alpha = \frac{p+1}{x} \f$.
    /// We notice \f$ p(0) \ne 0 \f$ because it is irreducible,
    /// (it is required for the above to be defined, of course),
    /// So we conclude that if \f$ p = \sum_{i=0}^d \beta_i x^i \f$ than
    /// \f$ \alpha = \sum_{i=0}^{d-1} \beta_{i+1} x^i \f$.
    /// This is exactly the way we compute \f$ \alpha = x^{-1} \f$
    ///
    
    const int64_t irred = commonInfo_.rowsModulus();
    const int64_t res = irred>>1;
    return Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,res);
}

} //namespace BairToAcsp
} //namespace libstark
