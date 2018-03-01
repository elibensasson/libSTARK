#include "LOCI_AES160hashchain_instance.hpp"
#include "common/Utils/ErrorHandling.hpp"

namespace stark_dpm{
namespace ACSP_FOR_AES160LOCIhashchain{

using std::vector;
using Algebra::FieldElement;
using namespace AES160LOCIhashchain;

namespace{
    class idPermutation : public libstark::BairWitness::permutation_t{
        public:
            size_t getElementByIndex(index_t index)const{
                return index;
            }
    };

    class coloringClass : public libstark::BairWitness::assignment_t{
		private:
			std::vector<libstark::BairWitness::color_t> coloring_;
        public:
            coloringClass(const AES160LOCIhashcCommonParams& commonParams, const witnessType hashC, const fingerprint_t& fprint):
				coloring_((size_t(1) << AES160LOCIhashchain::getDim(commonParams.length))-1, vector<FieldElement>(AES160LOCIhashchain::NUMREGS))
            {
				AES160LOCIhashchain::genWitnessLOCIHashcAES160WithPadding(coloring_, hashC,	commonParams.length, fprint);
					//std::cout << "done " << coloring_[0][AES160hashchain::reg::STATE] << std::endl;
            }
            
            libstark::BairWitness::color_t getElementByIndex(index_t index)const{
                _COMMON_ASSERT(index < coloring_.size(),"index of color out of range");
                return coloring_[index];
            }
    };
}

libstark::BairWitness buildBairWitness(const AES160LOCIhashcCommonParams& commonParams, const witnessType hashC, const AES160LOCIhashchain::fingerprint_t& fprint){
    using libstark::BairWitness;
    BairWitness::permutation_ptr perm(new idPermutation());
    BairWitness::assignment_ptr assignment(new coloringClass(commonParams, hashC, fprint)); 

    return BairWitness(std::move(assignment), std::move(perm));
}

} // namespace ACSP_FOR_AES160LOCIhashchain
} // namespace stark_dpm
