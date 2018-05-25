#include "AES160hashcLOCI_instance.hpp"

namespace stark_dpm{
namespace ACSP_FOR_AES160hashcLOCI{

namespace{

using std::vector;

class empty_CS : public libstark::ConstraintSys{
    public:

    /// the amount of inputs each polynomial expects
	size_t numVars() const{ return 2 * AES160hashcLOCI::NUMREGS; }
    size_t numMappings() const{return polys.size();}
    empty_CS* clone() const{return new empty_CS();}

    /// the constraint polynomials
	virtual const polySet_t& constraints() const{return polys;}
    vector<FieldElement> eval(const vector<FieldElement>& assignment) const{
        vector<FieldElement> res;
        for(const auto& p : polys){
            res.push_back(p->eval(assignment));
        }

        return res;
    }

private:
	polySet_t polys;
};
}

libstark::BairInstance buildBairInstance(const AES160hashcLOCICommonParams& params){
    using Algebra::FieldElement;
    using Algebra::zero;
    using Algebra::mapIntegerToFieldElement;
    using libstark::BairInstance;

	const size_t vectorsLen = AES160hashcLOCI::NUMREGS;
    const short domainSizeIndicator = AES160hashcLOCI::getDim(params.length);
	BairInstance::constraintsPtr_t constraints_assignment(new AES160hashcLOCI_CS());
    BairInstance::constraintsPtr_t constraints_permutation(new empty_CS());
    
    BairInstance::boundaryConstraints_t boundary;
    boundary[BairInstance::point_t(0, AES160hashcLOCI::reg::PHASE)] = Algebra::zero();
    boundary[BairInstance::point_t(0, AES160hashcLOCI::reg::ST2)] = Algebra::xFE()*Algebra::xFE();
    boundary[BairInstance::point_t(0, AES160hashcLOCI::reg::MATCH)] = Algebra::zero();
    const unsigned long lastTimestamp = (1UL<<domainSizeIndicator)-2;
    boundary[BairInstance::point_t(lastTimestamp, AES160hashcLOCI::reg::MATCH)] = Algebra::zero();

	 return BairInstance(vectorsLen,domainSizeIndicator,
		 std::move(constraints_assignment), std::move(constraints_permutation), boundary,
		 std::vector<Algebra::FieldElement>(AES160hashcLOCI::NUMREGS, zero()));
}

} // namespace ACSP_FOR_AES160hashcLOCI
} // namespace stark_dpm
