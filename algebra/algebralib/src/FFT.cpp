#include "algebraLib/FFT.hpp"
#include "algebraLib/ErrorHandling.hpp"
#include "FFT.h"
#include <algorithm>
#include <cstring>

/// Important : all this assumes Matans irreducible is used for the context field

namespace Algebra{
using std::vector;
using std::max;

vector<FieldElement> IFFT(const vector<FieldElement>& polyEvaluation, const vector<FieldElement>& orderedBasis, const FieldElement& shift){
    
    ALGEBRALIB_ASSERT(polyEvaluation.size() == (1UL<<orderedBasis.size()), "IFFT is supported only for evaluation exactly of the size of the evaluation space");
    
    vector<FieldElement> vals_vec(polyEvaluation);
    FieldElement* values = (FieldElement*)(&(vals_vec[0]));
    IFFT_inplace(values,orderedBasis,shift);
    return vals_vec;
}

void IFFT_inplace(FieldElement* polyEvaluation, const vector<FieldElement>& orderedBasis, const FieldElement& shift){
    
    FFF::Element* basis_vec = (FFF::Element*)(&(orderedBasis[0]));
    FFF::Element shift_fff(shift);
    
    FFF::Basis basis(basis_vec, orderedBasis.size(),shift_fff);

    FFF::FFT fftInstance(basis,FFF::IFFT_OP);
    fftInstance.AlgIFFT((FFF::Element*)polyEvaluation);
}

namespace{
    //assumes x > 0
    short correct_ceilLog2(const size_t x){
        //just for disabling an infinite loop
        if (x == 0){
            return 0;
        }
        short log=0;
        size_t y = x;
        while (y != 1){
            log++;
            y >>= 1;
        }

        if (x > (1UL<<log)){
            log++;
        }

        return log;
    }
}

vector<FieldElement> FFT(const vector<FieldElement>& poly, const vector<FieldElement>& orderedBasis, const FieldElement& shift){
    
    const size_t evaluationSapceSize = (1L<<orderedBasis.size());

    ALGEBRALIB_ASSERT(poly.size() <= evaluationSapceSize, "FFT is supported only for evaluation spaces of size at least as the polynomial degree");

    vector<FieldElement> vals_vec(poly);
    //enlarge if needed to hold result
    vals_vec.resize(evaluationSapceSize,zero());
    
    FieldElement* values = (FieldElement*)(&(vals_vec[0]));
    FFT_inplace(values,orderedBasis,shift,max(int(correct_ceilLog2(poly.size())),1));
     
    return vals_vec;
}

void FFT_inplace(FieldElement* poly, const vector<FieldElement>& orderedBasis, const FieldElement& shift, const size_t polyDeg_ceilLog){
    const size_t evaluationSapceSize = (1L<<polyDeg_ceilLog);
    FFF::Element* basis_vec = (FFF::Element*)(&(orderedBasis[0]));
    const vector<FieldElement> shiftsBasis(orderedBasis.begin()+polyDeg_ceilLog,orderedBasis.end());
    const size_t numShifts = 1L<<shiftsBasis.size();

    //evaluate coset after coset, for cosets of minimal size possible (at least the poly degree)
    for (size_t i=0; i< numShifts; i++){
        //before destroing the poly, copy it to next evaluation if needed
        if( i+1 < numShifts){
            memcpy(poly + evaluationSapceSize, poly, evaluationSapceSize*sizeof(FieldElement));
        }
        const FieldElement cosetShift = getSpaceElementByIndex(shiftsBasis,shift,i);
        FFF::Element shift_fff(cosetShift);
        FFF::Basis basis(basis_vec, polyDeg_ceilLog, shift_fff);

        FFF::FFT fftInstance(basis,FFF::FFT_OP);
        auto c_poly = (FFF::Element*)(poly);
        fftInstance.AlgFFT(&c_poly,evaluationSapceSize);
        
        //change poly pointer to next needed value
        poly += evaluationSapceSize;
    }
}

vector<FieldElement> LDE(const vector<FieldElement>& polyEvaluation, const vector<FieldElement>& orderedBasis_src ,const FieldElement& shift_src, const vector<FieldElement>& orderedBasis_dst, const FieldElement& shift_dst){
    ALGEBRALIB_ASSERT(orderedBasis_src.size() <= orderedBasis_dst.size(), "LDE is supported only for destination evaluation spaces of size at least as the source evaluation space");
    ALGEBRALIB_ASSERT(polyEvaluation.size() == (1UL<<orderedBasis_src.size()), "LDE is supported only for evaluation exactly of the size of the evaluation source space");
    
    const size_t evaluationSapceSize = (1L<<orderedBasis_dst.size());

    vector<FieldElement> vals_vec(polyEvaluation);
    //enlarge if needed to hold result
    if(vals_vec.size() < evaluationSapceSize){
        vals_vec.resize(evaluationSapceSize,zero());
    }
    
    FieldElement* values = (FieldElement*)(&(vals_vec[0]));
    LDE_inplace(values, orderedBasis_src,shift_src,orderedBasis_dst,shift_dst);
    
    return vals_vec;
}

void LDE_inplace(FieldElement* polyEvaluation, const vector<FieldElement>& orderedBasis_src ,const FieldElement& shift_src, const vector<FieldElement>& orderedBasis_dst, const FieldElement& shift_dst){
    
    IFFT_inplace(polyEvaluation,orderedBasis_src,shift_src);
    FFT_inplace(polyEvaluation,orderedBasis_dst,shift_dst,orderedBasis_src.size());
}
} //namespace Algebra
