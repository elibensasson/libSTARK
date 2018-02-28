#ifndef BairToAcsp_INPUTINDICATOR_HPP__
#define BairToAcsp_INPUTINDICATOR_HPP__

namespace libstark{
namespace BairToAcsp{

class boundaryMapping : private instanceMappings{
public:
    boundaryMapping(const common& commonInfo):
        instanceMappings(commonInfo){};

    witnessElement_t mapPoint(const size_t rowId, const size_t varId)const{
        commonMappings::witnessElement_t res = mapVariable_witnessElement(varId);
        res.second += mapRowId(rowId);
        
        return res;
    }

private:
    //Methods
    Algebra::FieldElement mapRowId(const size_t rowId)const{
        return map_x_power_modulu_poly(rowId, commonInfo_.rowsModulus());
    }
};

std::vector<AcspInstance::boundaryConstraints_t> reduceBoundaryConstraints(const BairInstance::boundaryConstraints_t& bairBoundary, const common& commonDef){
    boundaryMapping mapping(commonDef);
    std::vector<AcspInstance::boundaryConstraints_t> boundaryConstraints(commonDef.witnessLayersAmount());
    for(const auto& p : bairBoundary){
        const auto currPoint = p.first;
        const size_t rowId = currPoint.first;
        const size_t varId = currPoint.second;
        const auto x = mapping.mapPoint(rowId,varId);
        boundaryConstraints[x.first][x.second] = p.second;
    }

    return boundaryConstraints;
}

} //namespace BairToAcsp
} //namespace libstark

#endif //#ifndef BairToAcsp_INPUTINDICATOR_HPP__
