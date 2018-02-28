#include "common.hpp"
#include <algebraLib/SubspacePolynomial.hpp>

namespace libstark{
namespace Protocols{
namespace Fri{
namespace common{

using std::vector;
using Algebra::FieldElement;
using Algebra::zero;
using Algebra::elementsSet_t;
using Algebra::SubspacePolynomial;

vector<FieldElement> getL0Basis(const vector<FieldElement>& BasisL, const bool L0isMSB){
	if(L0isMSB){
        return vector<FieldElement>(BasisL.end() - SoundnessParameters::dimReduction, BasisL.end());
    }
    
    return vector<FieldElement>(BasisL.begin(), BasisL.begin() + SoundnessParameters::dimReduction);
}

vector<FieldElement> getL1Basis(const vector<FieldElement>& BasisL, const bool L0isMSB){
	if(L0isMSB){
        return vector<FieldElement>(BasisL.begin(), BasisL.end() - SoundnessParameters::dimReduction);
    }
    return vector<FieldElement>(BasisL.begin() + SoundnessParameters::dimReduction, BasisL.end());
}

size_t getBasisLIndex_byL0L1indices(const vector<FieldElement>& BasisL, const size_t idxL0, const size_t idxL1, const bool L0isMSB){
	if(L0isMSB){
        const size_t BasisL1_size = BasisL.size() - SoundnessParameters::dimReduction;
        return idxL1 | (idxL0<<BasisL1_size);
    }
    
    return idxL0 | (idxL1<<SoundnessParameters::dimReduction);
}

vector<FieldElement> getColumnBasis(const vector<FieldElement>& L, const bool L0isMSB){
    const vector<FieldElement> L0Basis = getL0Basis(L, L0isMSB);
    const elementsSet_t rowsBasis(L0Basis.begin(), L0Basis.end());
    
    vector<FieldElement> basisForColumn(getL1Basis(L, L0isMSB));
    {
        const SubspacePolynomial q(rowsBasis);
        const FieldElement q_on_ZERO = q.eval(zero());
        for(FieldElement& e : basisForColumn){
            e = q.eval(e + q_on_ZERO);
        }
    }

    return basisForColumn;
}

unsigned short dimOfColumn(const unsigned short dimOfL){
    return dimOfL - SoundnessParameters::dimReduction;
}

} //namespace common
} //nmasepace Fri
} //namespace Protocols
} //namespace libstark
