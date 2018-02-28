#ifndef _ALGEBRALIB_FFT__HPP
#define _ALGEBRALIB_FFT__HPP

#include "FieldElement.hpp"
#include <vector>

namespace Algebra{

/**
 * Calculates the inverse FFT (a specific kind of interpolation)
 * Given the basis size is \f$n\f$ the expected size of polyEvaluation is \f$ 2^n \f$
 * it finds the single polynomial \f$p \in \mathbb{F}[x]\f$ of degree at most \f$ 2^n -1 \f$ such that
 * for any \f$ a_0,a_1,\dots,a_{n-1} \in \{0,1\} \f$ it holds that:
 * \f$ p( \sum_{i=0}^{i=n-1} a_i \cdot b_i ) = v_{\sum_{i=0}^{i=n-1} a_i \cdot 2_i} \f$
 * where \f$ b_i \f$ is the i'th element of the ordered basis
 * and \f$ v_i \f$ is the i'th element of the polyEvaluation
 *
 * The return value is the coefficients vector of the polynomial \f$ p \f$
 */
std::vector<FieldElement> IFFT(const std::vector<FieldElement>& polyEvaluation, const std::vector<FieldElement>& orderedBasis, const FieldElement& shift);

/**
 * IFFT, but the result is stored in the input polyEvaluation
 */
void IFFT_inplace(FieldElement* polyEvaluation, const std::vector<FieldElement>& orderedBasis, const FieldElement& shift);

/**
 * Calculates the FFT (a specific kind of fast evaluation of a polynomial over a vector space)
 * Given the basis size is \f$n\f$ it evaluates \f$poly\f$ over the space spenned be the basis
 * The result ordered in the following way:
 * For any \f$ a_0,a_1,\dots,a_{n-1} \in \{0,1\} \f$ it holds that:
 * \f$ poly( \sum_{i=0}^{i=n-1} a_i \cdot b_i ) = r_{\sum_{i=0}^{i=n-1} a_i \cdot 2_i} \f$
 * where \f$ b_i \f$ is the i'th element of the ordered basis
 * and \f$ r_i \f$ is the i'th element of the result
 *
 * The return value is the coefficients vector of the polynomial \f$ p \f$
 */
std::vector<FieldElement> FFT(const std::vector<FieldElement>& poly, const std::vector<FieldElement>& orderedBasis, const FieldElement& shift);

/**
 * FFT, but the result is stored in the input poly
 */
void FFT_inplace(FieldElement* poly, const std::vector<FieldElement>& orderedBasis, const FieldElement& shift, const size_t polyDeg_ceilLog);

/**
 * Low Degree Extension.
 * Same as FFT(IFFT(polyEvaluations,orderedBasis_src),orderedBasis_dst)
 */
std::vector<FieldElement> LDE(const std::vector<FieldElement>& polyEvaluation, const std::vector<FieldElement>& orderedBasis_src, const FieldElement& shift_src, const std::vector<FieldElement>& orderedBasis_dst, const FieldElement& shift_dst);

/**
 * LDE, but the result is stored in the input polyEvaluation
 */
void LDE_inplace(FieldElement* polyEvaluation, const std::vector<FieldElement>& orderedBasis_src, const FieldElement& shift_src, const std::vector<FieldElement>& orderedBasis_dst, const FieldElement& shift_dst);

} //namespace Algebra

#endif // _ALGEBRALIB_FFT__HPP
