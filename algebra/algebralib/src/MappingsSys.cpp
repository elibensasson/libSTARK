#include "algebraLib/MappingsSys.hpp"
#include "algebraLib/ErrorHandling.hpp"

namespace Algebra{

using std::vector;
using std::unique_ptr;

namespace{
    class linearCombFunc : public multivarFunc{
    public:
        linearCombFunc(const MappingsSys& mappings, const vector<FieldElement>& coeffs):
            mappings_(mappings.clone()), coeffs_(coeffs){};
        
        size_t numVars() const{
            return mappings_->numVars();
        }

        FieldElement eval(const vector<FieldElement>& assignment) const{
            const vector<FieldElement> multiRes = mappings_->eval(assignment);
            FieldElement res = Algebra::zero();
            for(unsigned int i=0; i<coeffs_.size(); i++){
                res += coeffs_[i]*multiRes[i];
            }

            return res;
        }
    
    private:
        const unique_ptr<MappingsSys> mappings_;
        const vector<FieldElement> coeffs_;
    };
}

multivarFunc* MappingsSys::getLinearComb(const vector<FieldElement>& coeffs) const{
    ALGEBRALIB_ASSERT(coeffs.size() == numMappings(), "number of coefficients is expected to be same as number of mappings");
    return new linearCombFunc(*this,coeffs);
}

} // namespace Algebra
