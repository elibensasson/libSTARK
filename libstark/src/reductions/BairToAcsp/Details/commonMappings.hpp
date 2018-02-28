#ifndef _COMMON_BairToAcsp_COMMONMAPPINGS_HPP__
#define _COMMON_BairToAcsp_COMMONMAPPINGS_HPP__

#include "common.hpp"

namespace libstark{
namespace BairToAcsp{
    
class commonMappings{

public:
     typedef size_t spaceIndex_t;
     typedef std::pair<size_t,spaceIndex_t> witnessIndex_t;
     typedef std::pair<size_t,Algebra::FieldElement> witnessElement_t;

     commonMappings(const common& commonInfo);
     Algebra::FieldElement mapNonPermutationElement(const size_t elementId)const;
     witnessElement_t mapNonPermutationElement_witness(const size_t elementId)const;
     
     spaceIndex_t mapPermutationColumnId_spaceIndex(const size_t columnId)const;
     Algebra::FieldElement mapPermutationColumnId_spaceElement(const size_t columnId)const;
     
     spaceIndex_t mapPermutationLayerId_spaceIndex(const size_t layerId)const;
     Algebra::FieldElement mapPermutationLayerId_spaceElement(const size_t layerId)const;
     
     spaceIndex_t mapPermutationElement_spaceIndex(const size_t columnId, const size_t layerId)const;
     Algebra::FieldElement mapPermutationElement_spaceElement(const size_t columnId, const size_t layerId)const;
     witnessElement_t mapPermutationElement_witnessElement(const size_t columnId, const size_t layerId)const;
     ///
     /// Let \f$ \eta \f$ be the rows modulus, of degree \f$ d \f$
     /// This function maps the elementId \f$ \sum_{i=0}^{n} b_i 2^i \f$
     /// to the field element \f$ b_0 \cdot \eta + x^d \cdot \sum_{i=1}^{n} b_i x^i \f$
     ///
     /// For more details ask Michael Riabzev (this method reduces maximal number of neighbors)
     ///
     spaceIndex_t mapNonPermutationVariable_spaceIndex(const size_t elementId)const;
     Algebra::FieldElement mapNonPermutationVariable_spaceElement(const size_t elementId)const;
     witnessElement_t mapNonPermutationVariable_witnessElement(const size_t elementId)const;
    
     Algebra::FieldElement map_spaceIndex_to_fieldElement(const spaceIndex_t& i)const;
     witnessIndex_t map_spaceIndex_to_witnessIndex(const spaceIndex_t& i)const;
     witnessElement_t map_spaceIndex_to_witnessElement(const spaceIndex_t& i)const;
    
protected:
    const size_t bitsForRowId_;
    const size_t bitsForColumnId_;
    const size_t bitsForLayerId_;
    const size_t bitsForNonPermutationElementId_;
    const size_t firstNonPermUsableIndex_;
    const spaceIndex_t rowsModulus_;
    const std::vector<spaceIndex_t> column_spaceIndex_;

    static Algebra::FieldElement map_x_power_modulu_poly(const size_t x_power, const int64_t modulus);
    static std::vector<spaceIndex_t> column_spaceIndex_init(const int64_t columnsModulus, const size_t shiftSize);
};

} //namespace BairToAcsp
} //namespace libstark


#endif // _COMMON_BairToAcsp_COMMONMAPPINGS_HPP__
