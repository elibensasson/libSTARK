#include "commonMappings.hpp"

using Algebra::FieldElement;
using Algebra::mapIntegerToFieldElement;
using Infrastructure::Log2;
using std::max;
using std::vector;

namespace libstark{
namespace BairToAcsp{

commonMappings::commonMappings(const common& commonInfo):
    bitsForRowId_(commonInfo.heightSpaceDimension()),
    bitsForColumnId_(commonInfo.widthSpaceDimension()),
    bitsForLayerId_(Algebra::ExtensionDegree - (commonInfo.heightSpaceDimension()+commonInfo.widthSpaceDimension())),
    bitsForNonPermutationElementId_(Algebra::ExtensionDegree - commonInfo.heightSpaceDimension()),
    firstNonPermUsableIndex_(commonInfo.imageWidth()*commonInfo.amountOfPermutationLayers()),
    rowsModulus_(commonInfo.rowsModulus()),
    column_spaceIndex_(column_spaceIndex_init(commonInfo.columnsModulus(),commonInfo.heightSpaceDimension())){};

FieldElement commonMappings::mapNonPermutationElement(const size_t elementId)const{
    return mapIntegerToFieldElement(bitsForRowId_,bitsForNonPermutationElementId_, firstNonPermUsableIndex_ + elementId);
}

commonMappings::witnessElement_t commonMappings::mapNonPermutationElement_witness(const size_t elementId)const{
    const spaceIndex_t spaceIdx = (firstNonPermUsableIndex_ + elementId) << bitsForRowId_;
    return map_spaceIndex_to_witnessElement(spaceIdx);
}

commonMappings::spaceIndex_t commonMappings::mapNonPermutationVariable_spaceIndex(const size_t elementId)const{
//#define USE_MICHAELS_NEIGHBORS_TRICK
#ifdef USE_MICHAELS_NEIGHBORS_TRICK
    const size_t elementId_clearedLSB = (elementId>>1)<<1;
    const spaceIndex_t offset = (elementId%2 == 0? 0 : rowsModulus_);
    return ((firstNonPermUsableIndex_ + elementId_clearedLSB)<<bitsForRowId_) ^ offset;

#else
    return ((firstNonPermUsableIndex_ + elementId)<<bitsForRowId_);
#endif
}

FieldElement commonMappings::mapNonPermutationVariable_spaceElement(const size_t elementId)const{
    const auto spaceIndex = mapNonPermutationVariable_spaceIndex(elementId);
    return map_spaceIndex_to_fieldElement(spaceIndex);
}

commonMappings::witnessElement_t commonMappings::mapNonPermutationVariable_witnessElement(const size_t elementId)const{
    const auto spaceIndex = mapNonPermutationVariable_spaceIndex(elementId);
    return map_spaceIndex_to_witnessElement(spaceIndex);
}

commonMappings::spaceIndex_t commonMappings::mapPermutationColumnId_spaceIndex(const size_t columnId)const{
    return column_spaceIndex_[columnId];
}

FieldElement commonMappings::mapPermutationColumnId_spaceElement(const size_t columnId)const{
    const auto spaceIndex= mapPermutationColumnId_spaceIndex(columnId);
    return map_spaceIndex_to_fieldElement(spaceIndex);
}

commonMappings::spaceIndex_t commonMappings::mapPermutationLayerId_spaceIndex(const size_t layerId)const{
    const size_t bitsOffset = bitsForRowId_ + bitsForColumnId_;
    return layerId<<bitsOffset;
}

FieldElement commonMappings::mapPermutationLayerId_spaceElement(const size_t layerId)const{
    const auto spaceIndex= mapPermutationLayerId_spaceIndex(layerId);
    return map_spaceIndex_to_fieldElement(spaceIndex);
}

commonMappings::spaceIndex_t commonMappings::mapPermutationElement_spaceIndex(const size_t columnId, const size_t layerId)const{
    return mapPermutationColumnId_spaceIndex(columnId) ^ mapPermutationLayerId_spaceIndex(layerId);
}

FieldElement commonMappings::mapPermutationElement_spaceElement(const size_t columnId, const size_t layerId)const{
    const auto spaceIndex= mapPermutationElement_spaceIndex(columnId,layerId);
    return map_spaceIndex_to_fieldElement(spaceIndex);
}
    
commonMappings::witnessElement_t commonMappings::mapPermutationElement_witnessElement(const size_t columnId, const size_t layerId)const{
    const auto spaceIndex= mapPermutationElement_spaceIndex(columnId,layerId);
    return map_spaceIndex_to_witnessElement(spaceIndex);
}
    
FieldElement commonMappings::map_x_power_modulu_poly(const size_t x_power, const int64_t modulus){

    const size_t modulus_deg = floor(Log2(modulus));
    const size_t overflow_mask = 1UL<<modulus_deg;
    
    size_t res = 1;
    for(size_t i=1; i<= x_power; i++){
        res <<= 1;
        if( res & overflow_mask){
            res ^= modulus;
        }
    }

    const FieldElement res_elem =  mapIntegerToFieldElement(0,modulus_deg,res);
    return res_elem;
}

FieldElement commonMappings::map_spaceIndex_to_fieldElement(const spaceIndex_t& i)const{
    const size_t numBits = bitsForRowId_ + bitsForColumnId_ + bitsForLayerId_;
    return mapIntegerToFieldElement(0 , numBits, i);
}

commonMappings::witnessIndex_t commonMappings::map_spaceIndex_to_witnessIndex(const spaceIndex_t& i)const{
    const size_t numBits_spaceIndex = bitsForRowId_ + bitsForColumnId_;
    const size_t spaceIdx_mask = (1<<numBits_spaceIndex)-1;

    const size_t layerIdx = i>>numBits_spaceIndex;
    const size_t spaceIdx = i & spaceIdx_mask;

    return witnessIndex_t(layerIdx,spaceIdx);
}

commonMappings::witnessElement_t commonMappings::map_spaceIndex_to_witnessElement(const spaceIndex_t& i)const{
    const witnessIndex_t idx = map_spaceIndex_to_witnessIndex(i);
    return witnessElement_t(idx.first, map_spaceIndex_to_fieldElement(idx.second));
}
     
vector<commonMappings::spaceIndex_t> commonMappings::column_spaceIndex_init(const int64_t columnsModulus, const size_t shiftSize){
    const int modulusDeg = floor(Infrastructure::Log2(columnsModulus));
    const spaceIndex_t overflow_mask = Infrastructure::POW2(modulusDeg);
    
    vector<spaceIndex_t> res;
    res.push_back(1L<<shiftSize);
    
    //if the modulus is of degree at most 0,
    //there should be only one element (the field is GF(2))
    if(modulusDeg < 1){
        return res;
    }
    
    while(true){
        //next element is multiplication by x (done by shift left)
        //modulu the modulus (done by xor if overflow)
        spaceIndex_t nextElem = (res[res.size()-1]>>shiftSize)<<1;
        if (nextElem & overflow_mask){
            nextElem ^= columnsModulus;
        }

        //if we got back to 1, we finished going over all the elements
        if(nextElem == 1){
            return res;
        }

        //add curr result
        res.push_back(nextElem<<shiftSize);
    }
}
    
} //namespace BairToAcsp
} // namespace libstark
