#ifndef _ALGEBRALIB_NOVEL_FFT__HPP
#define _ALGEBRALIB_NOVEL_FFT__HPP

#include "FieldElement.hpp"
#include "SubspacePolynomial.hpp"
#include <vector>

namespace Algebra{

///
/// This is implementation of the novel FFT
/// introduced in:
/// "Novel Polynomial Basis and Its Application to Reed-Solomon Erasure Codes"
/// By : Sian-Jheng Lin, Wei-Ho Chung, Yunghsiang S. Han
///
/// This algorithm has O(n \log n ) time complexity, with very low constants
/// It is very easy to use to evaluate many polynomials at one shot over the same space.
/// It's additional space complexity is less than the evaluation size.
///
/// As this algorithm uses special polynomial basis,
/// it first transforms coefficients over the standard basis to coefficients
/// for the special basis, this is done by evaluation (using Matans FFT) and
/// IFFT using the novel IFFT algorithm.
///
/// The coefficients in the special basis are kept for future evaluation.
/// 
/// The evaluation function FFT evaluates multiple polynomials at the same time,
/// and writes their result into a matrix.
///
/// It is optimized for the usage of the project:
/// It performs evaluation only on consecutive blocks of size
/// 2^{orderedBasis.size()} * width
/// Each block it hough as a matrix, where the point (x,y)
/// is at memory address y*width + x.
/// The i'th polynomial evaluation is written on the i'th column.
/// All columns not containing polynomial evaluations set with pad_value.

class novelFFT{
public:
    
    //Initialize with formal sums
    novelFFT(const std::vector<FieldElement>& orderedBasis, std::vector<std::vector<FieldElement>>&& polysCoeffs, const size_t width, const FieldElement& pad_value);
    
    //Initialize with evaluations for LDE - implemented for a single poly only because of laziness
    novelFFT(const std::vector<FieldElement>& orderedBasis, std::vector<FieldElement>&& srcEval); 

    void FFT(const std::vector<FieldElement>& affineShift, FieldElement* dst, const size_t diff_coset)const;
private:
    const std::vector<SubspacePolynomial> X_exp_;
    const std::vector<FieldElement> polys_;
    const std::vector<FieldElement> orderedBasis_;
    const size_t numPolys_;
    const size_t width_;
   
    //For initialization with formal sums
    novelFFT(const std::vector<FieldElement>& orderedBasis, const std::vector<SubspacePolynomial> X_exp, std::vector<std::vector<FieldElement>>&& polysCoeffs, const size_t width, const FieldElement& pad_value);
    
    //Initialize with evaluations for LDE - implemented for a single poly only because of laziness
    novelFFT(const std::vector<FieldElement>& orderedBasis, const std::vector<SubspacePolynomial> X_exp, std::vector<FieldElement>&& polyVals);
};

} //namespace Algebra

#endif // _ALGEBRALIB_NOVEL_FFT__HPP
