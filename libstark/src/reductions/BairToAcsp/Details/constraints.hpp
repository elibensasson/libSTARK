#ifndef BairToAcsp_CONSTRAINTS_HPP__
#define BairToAcsp_CONSTRAINTS_HPP__

#include "neighborsConstructor.hpp"
#include "spaces.hpp"
#include "common.hpp"
#include "instanceMappings.hpp"

namespace libstark{
namespace BairToAcsp{

class constraints{
public:
    static std::unique_ptr<Algebra::PolynomialInterface> getConstraintsPoly(
const BairInstance& instance,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping,
const spaces& spacesGenerator,
const CS_testLocations& testLocations);
};

} //namespace BairToAcsp
} //namespace libstark

#endif //#ifndef BairToAcsp_CONSTRAINTS_HPP__
