#ifndef __ACSP_FOR_AES160hashcLOCI_INSTANCE_HPP__
#define __ACSP_FOR_AES160hashcLOCI_INSTANCE_HPP__

#include "AES160hashcLOCI.hpp"
#include "languages/Bair/ConstraintsSys.hpp"

namespace stark_dpm{
namespace ACSP_FOR_AES160hashcLOCI{

	class AES160hashcLOCI_CS : public libstark::ConstraintSys{
    public:
		AES160hashcLOCI_CS();
    
    /// the amount of inputs each polynomial expects
    size_t numVars() const{return 2*AES160hashcLOCI::NUMREGS;}
    size_t numMappings() const{return polys_.size();}

    AES160hashcLOCI_CS* clone() const;

    /// the constraint polynomials
	const polySet_t& constraints() const{return polys_;}
    std::vector<FieldElement> eval(const std::vector<FieldElement>& assignment) const;
    
    private:
    polySet_t polys_;
};

} // namespace ACSP_FOR_AES160hashcLOCI
} // namespace stark_dpm

#endif // __ACSP_FOR_AES160hashcLOCI_INSTANCE_HPP__
