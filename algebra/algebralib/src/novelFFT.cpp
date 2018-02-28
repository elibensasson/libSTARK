#include "algebraLib/novelFFT.hpp"
#include "algebraLib/ErrorHandling.hpp"
#include <FFT.h>
#include <string.h>

namespace Algebra{

using std::vector;

namespace{
    vector<SubspacePolynomial> calc_X_exp(const vector<FieldElement>& orderedBasis){
        vector<SubspacePolynomial> X_exp;
        {
            for(unsigned int i=0; i<orderedBasis.size(); i++){
                const elementsSet_t currBasis(orderedBasis.begin(),orderedBasis.begin()+i);
                X_exp.push_back(SubspacePolynomial(currBasis));
                SubspacePolynomial& currPoly = X_exp[X_exp.size()-1];
                const FieldElement currElem = orderedBasis[i];
                const FieldElement factor = one()/currPoly.eval(currElem);
                currPoly.multiplyByConstant(factor);
            }
        }

        return X_exp;
    }
    
    vector<FieldElement> novelIFFT(const vector<FieldElement> orderedBasis, const vector<SubspacePolynomial> X_exp, vector<FieldElement>&& polysVals,const size_t numPolys, const size_t width){
        const unsigned short basisSize = orderedBasis.size();
        const size_t spaceSize = 1UL<<basisSize;
        
        for (int i=0; i < basisSize; i++){
            const unsigned short currShift_c = i;
            const size_t currMask_m = (1UL<<currShift_c)-1;
            const size_t oddMask_c = 1UL<<currShift_c;
            const unsigned short len_c = basisSize-currShift_c;

            vector<FieldElement> X_exp_precomp(1UL<<(len_c-1));
#pragma omp parallel for
            for(unsigned long long c=0; c < X_exp_precomp.size(); c++){
                const FieldElement c_elem = getSpaceElementByIndex(orderedBasis,zero(),c<<(currShift_c+1));
                X_exp_precomp[c] = X_exp[i].eval(c_elem);
            }

#pragma omp parallel for
            for(unsigned long long mc=0; mc < (spaceSize>>1); mc++){

                const size_t m = mc & currMask_m;
                const size_t c = (mc>>(currShift_c))<<1;

                
                for(size_t polyIdx = 0; polyIdx < numPolys; polyIdx++){
                    FieldElement* currPoly = &polysVals[polyIdx];
                    //handle case (18)
                    {
                        const size_t mc_case18 = m | ((c|1UL)<<currShift_c);
                        const size_t innerIdx = mc_case18 ^ oddMask_c;

                        currPoly[width*mc_case18] = currPoly[width*innerIdx] + currPoly[width*mc_case18];
                    }

                    //handle case (17)
                    {
                        const size_t mc_case17 = m | (c<<currShift_c);
                        const size_t prevIdx2 = mc_case17 ^ oddMask_c;

                        currPoly[width*mc_case17] = currPoly[width*mc_case17] + X_exp_precomp[c>>1] * currPoly[width*prevIdx2];
                    }
                }
            }
        }

        return polysVals;
    }

    vector<FieldElement> convertPolysBasis(const vector<FieldElement> orderedBasis, const vector<SubspacePolynomial> X_exp, vector<vector<FieldElement>>&& polysCoeffs, const size_t width, const FieldElement& pad_value){

    ALGEBRALIB_ASSERT(width >= polysCoeffs.size(), "Width must be at least as the number of polys");
    const size_t numPolys = polysCoeffs.size();
    const size_t spaceSize = 1UL<<orderedBasis.size();
    
    //evaluate using standard FFT on the space
    vector<FieldElement> res(width * spaceSize, pad_value);
    {
        FFF::Element* basis_vec = (FFF::Element*)(&(orderedBasis[0]));
        FFF::Element shift_fff(zero());
        FFF::Basis basis(basis_vec, orderedBasis.size(),shift_fff);
        FFF::FFT fftInstance(basis,FFF::FFT_OP);

#pragma omp parallel for
        for(unsigned int i=0; i< numPolys; i++){
            
            vector<FieldElement>& currCoeffs = polysCoeffs[i];
            
            ALGEBRALIB_ASSERT(currCoeffs.size() <= spaceSize, "FFT is supported only for evaluation spaces of size at least as the polynomial degree");
            
            if(currCoeffs.size() < spaceSize){
                currCoeffs.resize(spaceSize,zero());
            }

            auto c_poly = (FFF::Element*)(&currCoeffs[0]);
            fftInstance.AlgFFT(&c_poly,spaceSize);

            //copy to res matrix
            for(unsigned long long x=0; x< spaceSize; x++){
                res[x*width + i] = currCoeffs[x];
            }
        }
    }

    //execute novel IFFT on evaluation to get coefficients in novel polynomial basis
    return novelIFFT(orderedBasis,X_exp,std::move(res),numPolys,width);
    }

} // anonymous namespace
    
novelFFT::novelFFT(const vector<FieldElement>& orderedBasis, const vector<SubspacePolynomial> X_exp, std::vector<std::vector<FieldElement>>&& polysCoeffs, const size_t width, const FieldElement& pad_value):
    X_exp_(X_exp),
    polys_(convertPolysBasis(orderedBasis,X_exp,std::move(polysCoeffs),width,pad_value)),
    orderedBasis_(orderedBasis),
    numPolys_(polysCoeffs.size()),
    width_(width){};
    
novelFFT::novelFFT(const vector<FieldElement>& orderedBasis, const vector<SubspacePolynomial> X_exp, std::vector<FieldElement>&& polyEval):
    X_exp_(X_exp),
    polys_(novelIFFT(orderedBasis,X_exp,std::move(polyEval),1UL,1UL)),
    orderedBasis_(orderedBasis),
    numPolys_(1),
    width_(1){};

novelFFT::novelFFT(const vector<FieldElement>& orderedBasis, vector<FieldElement>&& srcEval) :
    novelFFT(orderedBasis, calc_X_exp(orderedBasis), std::move(srcEval)){};

novelFFT::novelFFT(const vector<FieldElement>& orderedBasis, vector<vector<FieldElement>>&& polysCoeffs, const size_t width, const FieldElement& pad_value) :
    novelFFT(orderedBasis, calc_X_exp(orderedBasis), std::move(polysCoeffs), width, pad_value){};
 
void novelFFT::FFT(const vector<FieldElement>& affineShift, FieldElement* dst, const size_t diff_coset)const{

    const unsigned short basisSize = orderedBasis_.size();
    const size_t spaceSize = 1UL<<basisSize;

    //copy coefficient to destination
    {
        const unsigned int max_threads_machine = omp_get_max_threads();
        const size_t bufferBytesLen = polys_.size() * sizeof(FieldElement);
        const size_t blockBytesLen = bufferBytesLen / max_threads_machine;
        const size_t blockRemeinder = bufferBytesLen % max_threads_machine;
#pragma omp parallel for
        for(long long blockIdx = 0; blockIdx < max_threads_machine; blockIdx++){
            for(unsigned int cosetIdx = 0; cosetIdx < affineShift.size(); cosetIdx++){
                memcpy((char*)(dst + (cosetIdx*diff_coset)) + (blockIdx*blockBytesLen), ((char*)&polys_[0]) + (blockIdx*blockBytesLen), blockBytesLen);
            }
        }
        
        if(blockRemeinder > 0){
            for(unsigned int cosetIdx = 0; cosetIdx < affineShift.size(); cosetIdx++){
                memcpy((char*)(dst + (cosetIdx*diff_coset)) + (bufferBytesLen - blockRemeinder), (char*)(&polys_[0]) + (bufferBytesLen - blockRemeinder), blockRemeinder);
            }
        }

    }

    //execute the FFT
    {
        for (int i=basisSize-1; i >= 0; i--){
            const unsigned short currShift_c = i;
            const size_t currMask_m = (1UL<<currShift_c)-1;
            const size_t oddMask_c = 1UL<<currShift_c;
            const unsigned short len_c = basisSize-currShift_c;
            const unsigned short logNumPrecompute = len_c-1;
            const size_t numPrecompute = 1UL<<(logNumPrecompute);
            const size_t numPrecomputeMask = numPrecompute-1;
            const size_t numShifts = affineShift.size();

            vector<vector<FieldElement>> X_exp_precomp(affineShift.size(),vector<FieldElement>(numPrecompute));
#pragma omp parallel for
            for(unsigned long long expIdx=0; expIdx < (numShifts<<logNumPrecompute); expIdx++){
                const size_t c = expIdx & numPrecomputeMask;
                const size_t cosetId = expIdx >> logNumPrecompute;
                const FieldElement c_elem = getSpaceElementByIndex(orderedBasis_,affineShift[cosetId],c<<(currShift_c+1));
                X_exp_precomp[cosetId][c] = X_exp_[i].eval(c_elem);
            }

#pragma omp parallel for
            for(unsigned long long mc=0; mc < spaceSize/2; mc++){

                const size_t m = mc & currMask_m;
                const size_t c = (mc>>(currShift_c))<<1;

                {
                    const size_t mc_case17 = m | (c<<currShift_c);
                    const size_t mc_case18 = mc_case17 ^ oddMask_c;

                    const size_t mc_case17index = width_*mc_case17;
                    const size_t mc_case18index = width_*mc_case18;
                    const long long cs1 = c>>1;
                    for(size_t cosetId=0; cosetId < affineShift.size(); cosetId++){
                        
                        const FieldElement Xpre = X_exp_precomp[cosetId][cs1];
                        const FFF::Element XpreElem = *(FFF::Element*)(&Xpre);
                        
                        const size_t cosetOffset = diff_coset*cosetId;
                        FieldElement* baseVec = dst + cosetOffset;
                        FFF::Element* currVecFFF = (FFF::Element*)(baseVec);
                        
                        //
                        // Irreducible specific implementation
                        //
                        FFF::Element::do_FFT_step(XpreElem, &currVecFFF[mc_case18index], &currVecFFF[mc_case17index],numPolys_);
                            
                        
                        /*
                         * General code - less field specific optimizations
                         */
                        
                        //const size_t ub = polys_.size() * diff_poly_fixed;
                        //for(size_t polyOffset=0; polyOffset < ub; polyOffset+=(diff_poly_fixed*2)){
                            //FieldElement* currVec = baseVec + polyOffset;
                            
                            //handle case (17)
                            //currVec[mc_case17index] = currVec[mc_case17index] + Xpre * currVec[mc_case18index];
                            
                            //handle case (18)
                            //currVec[mc_case18index] = currVec[mc_case17index] + currVec[mc_case18index];
                        //}
                    }
                }
            }
        }
    }
}

} // namespace Algebra
