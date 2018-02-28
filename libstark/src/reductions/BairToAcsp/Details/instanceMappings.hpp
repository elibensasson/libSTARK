#ifndef _COMMON_BairToAcsp_INSTANCEMAPPINGS_HPP__
#define _COMMON_BairToAcsp_INSTANCEMAPPINGS_HPP__

#include "commonMappings.hpp"

namespace libstark{
namespace BairToAcsp{
    
class instanceMappings : public commonMappings{
public:
    instanceMappings(const common& commonInfo);
    Algebra::FieldElement mapVariable(const size_t varId)const;
    witnessElement_t mapVariable_witnessElement(const size_t varId)const;
    Algebra::FieldElement mapNonPermutation_zeroRow(const size_t elementId)const;
    Algebra::FieldElement mapNonPermutation_lastRow(const size_t elementId)const;
protected:
    const common& commonInfo_;

private:
    Algebra::FieldElement getLastRowIndicator()const;
};

} //namespace BairToAcsp
} //namespace libstark


#endif // #ifndef _COMMON_BairToAcsp_INSTANCEMAPPINGS_HPP__
