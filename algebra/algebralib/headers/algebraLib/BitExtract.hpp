/** @file
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef ALGEBRALIB_TRACE_HPP_
#define ALGEBRALIB_TRACE_HPP_

#include <algebraLib/FieldElement.hpp>
#include <array>

namespace Algebra {

#define invExtrArrSize 27
class invExtrType {
	private:
		static const FieldElement invExtrArr[invExtrArrSize];
	public:
		const FieldElement& operator[](const int i)const;
};
extern const invExtrType invExtrConsts;
	
typedef std::array<size_t,63> matBitExt_t;
matBitExt_t matForBitExtr(const int bitNum);
FieldElement extractBit(const FieldElement& elem, const int bitNum, const std::array<size_t,63>& invExtrMat);
FieldElement extractBit(const FieldElement& elem, const int bitNum);

} // namespace Algebra

#endif // ALGEBRALIB_TRACE_HPP_
