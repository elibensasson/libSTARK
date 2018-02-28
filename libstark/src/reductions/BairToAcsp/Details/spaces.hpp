#ifndef BairToAcsp_SPACES_HPP__
#define BairToAcsp_SPACES_HPP__

#include "common.hpp"

#include <vector>

namespace libstark{
namespace BairToAcsp{

class spaces{
public:
    spaces(const common& commonDef):commonDef_(commonDef){};
    
    //returns the basis of the vanishing space
    Algebra::elementsSet_t getVanishingSpaceBasis()const;

    //returns the basis of a pair of network layers (the basis of the linear space of the first two layers)
    Algebra::elementsSet_t getNetworkLayersBasis()const;
   
    //returns the basis of a column space
    Algebra::elementsSet_t getNetworkColumnBasis()const;
    
    //returns the basis of half a column space
    //it contains on the parts of the columns where the vectorId
    //has a 0 value in it most significant bit
    Algebra::elementsSet_t getNetworkColumnAssignmentNoCarryBasis()const;

    //Returns the basis of a subspace of a layer space
    //Such that all the vertexes in the DeBruijn network
    //in that space has the same 2 neighbours
    Algebra::elementsSet_t getDeBruijnNeighborsPairSpaceBasis()const;

    //calculate shifts for DeBruijn routing test
    std::vector<Algebra::FieldElement> getDeBruijnLayerCosets()const;
private:
    const common& commonDef_;
    
    size_t offsetOfNetworkLayerId()const;
    std::vector<size_t> getBitIndexesForDeBruijnNeighboursPairIndicator()const;
};

} //namespace BairToAcsp
} //namespace libstark

#endif // #ifndef BairToAcsp_SPACES_HPP__
