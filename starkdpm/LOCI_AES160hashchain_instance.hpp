#ifndef __ACSP_FOR_AES160LOCIhashchain_INSTANCE_HPP__
#define __ACSP_FOR_AES160LOCIhashchain_INSTANCE_HPP__

#include "LOCI_AES160hashchain.hpp"
#include "languages/Bair/ConstraintsSys.hpp"

namespace stark_dpm{
namespace ACSP_FOR_AES160LOCIhashchain{

	class AES160LOCIhashchain_CS : public libstark::ConstraintSys{
    public:
		AES160LOCIhashchain_CS();
    
    /// the amount of inputs each polynomial expects
    size_t numVars() const{return 2*AES160LOCIhashchain::NUMREGS;}
    size_t numMappings() const{return polys_.size();}
    
    AES160LOCIhashchain_CS* clone() const;

    /// the constraint polynomials
	const polySet_t& constraints() const{return polys_;}
    std::vector<FieldElement> eval(const std::vector<FieldElement>& assignment) const;
    
    private:
    polySet_t polys_;
};

} // namespace ACSP_FOR_AES160hashchaine
} // namespace stark_dpm

#endif // __ACSP_FOR_AES160LOCIhashchain_INSTANCE_HPP__
