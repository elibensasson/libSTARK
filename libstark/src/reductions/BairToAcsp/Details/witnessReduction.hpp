#ifndef Bair_TO_Acsp_WITNESS_REDUCTION_HPP__
#define Bair_TO_Acsp_WITNESS_REDUCTION_HPP__

#include "common.hpp"
#include "commonMappings.hpp"
#include "languages/Acsp/AcspWitness.hpp"
#include "languages/Bair/BairWitness.hpp"
#include "languages/Bair/BairInstance.hpp"
#include "witnessMappings.hpp"

#include <algebraLib/UnivariatePolynomialGeneral.hpp>
#include <memory>

namespace libstark{
namespace BairToAcsp{

class witnessReduction{
public:
    static std::unique_ptr<AcspWitness> reduceWitness(const BairInstance& instance, const BairWitness& witness);
    
protected:
    typedef std::vector<Algebra::FieldElement> evaluation_t;
    
    static std::vector<evaluation_t> getEmbeddingMapping( const BairInstance& instance, const BairWitness& witness, const common& commonDef, const witnessMappings& witnessMapping);
    static void mapChi(const BairInstance& instance, const BairWitness& witness, std::vector<evaluation_t>& mappings, const common& commonDef, const witnessMappings& witnessMapping);
    static void mapNetwork(const BairInstance& instance, const BairWitness& witness, std::vector<evaluation_t>& mappings, const common& commonDef, const witnessMappings& witnessMapping);
};
    
} //namespace BairToAcsp
} //namespace libstark


#endif // Bair_TO_Acsp_WITNESS_REDUCTION_HPP__
