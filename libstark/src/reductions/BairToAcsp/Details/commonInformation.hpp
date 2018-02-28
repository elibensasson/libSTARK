/**
 * @file BEXtoAcsp_commonInforamtion.hpp
 *
 * Declares the BairToAcsp reduction common information,
 * as descried at Acsp Spec document.
 */

#ifndef _COMMON_BairToAcsp_COMMONINFORMATION_HPP_
#define _COMMON_BairToAcsp_COMMONINFORMATION_HPP_

#include "commonDeffinitions.hpp"
#include "common/Utils/ErrorHandling.hpp"

namespace libstark{
namespace BairToAcsp{


/**
 * @class commonInformation
 * @brief represents common values that are restricted, but may be chosen with some freedom
 *
 * We use the BairPartialInstance : \f$(\mathbb{F},d,\mathcal{V},\mathcal{C}_\mathcal{A}, \mathcal{C}_\pi, \mathscr{I})\f$
 */
class commonInformation{
public:
    commonInformation(const commonDeffinitions& commonDef):
        rowsPrimitivePoly_(findPrimitive(commonDef.heightSpaceDimension())),
        columnsPrimitivePoly_(findPrimitive(commonDef.widthSpaceDimension()))
        { };

    /// The modulus for row ids field simulation
    const int64_t rowsModulus()const {return rowsPrimitivePoly_;}

    /// The modulus for column ids field simulation
    const int64_t columnsModulus()const {return columnsPrimitivePoly_;}

private:
    const int64_t rowsPrimitivePoly_;
    const int64_t columnsPrimitivePoly_;

/** 
* Returns a primitive polynomial 
* The primitive polynomial is found in a lookup table, according to the requested degree.
*/
static int64_t findPrimitive(const int degree) {
	
    int64_t polyRepresentation;

	//Step 1 - Finding the representation of the polynomial in the lookup table:
	switch(degree) {
	case 0: polyRepresentation = 01; break;
	case 1: polyRepresentation = 02; break;
	case 2: polyRepresentation = 07; break;
	case 3: polyRepresentation = 013; break;
	case 4: polyRepresentation = 023; break;
	case 5: polyRepresentation = 045; break;
	case 6: polyRepresentation = 0103; break;
	case 7: polyRepresentation = 0203; break;
	case 8: polyRepresentation = 0435; break;
	case 9: polyRepresentation = 01021; break;
	case 10: polyRepresentation = 02011; break;
	case 11: polyRepresentation = 04005; break;
	case 12: polyRepresentation = 010123; break;
	case 13: polyRepresentation = 020033; break;
	case 14: polyRepresentation = 042103; break;
	case 15: polyRepresentation = 0100003; break;
	case 16: polyRepresentation = 0210013; break;
	case 17: polyRepresentation = 0400011; break;
	case 18: polyRepresentation = 01000201; break;
	case 19: polyRepresentation = 02000047; break;
	case 20: polyRepresentation = 04000011; break;
	case 21: polyRepresentation = 010000005; break;
	case 22: polyRepresentation = 020000003; break;
	case 23: polyRepresentation = 040000041; break;
	case 24: polyRepresentation = 0100000207; break;
	case 25: polyRepresentation = 0200000011; break;
	case 26: polyRepresentation = 0400000107; break;
	case 27: polyRepresentation = 01000000047; break;
	case 28: polyRepresentation = 02000000011; break;
	case 29: polyRepresentation = 04000000005; break;
	case 30: polyRepresentation = 010040000007; break;
	case 31: polyRepresentation = 020000000011; break;
	case 32: polyRepresentation = 040020000007; break;
	case 33: polyRepresentation = 0100000020001; break;
	case 34: polyRepresentation = 0201000000007; break;
	case 35: polyRepresentation = 0400000000005; break;
	case 36: polyRepresentation = 01000000004001; break;
	case 37: polyRepresentation = 02000000012005; break;
	case 38: polyRepresentation = 04000000000143; break;
	case 39: polyRepresentation = 010000000000021; break;
	case 40: polyRepresentation = 020000012000005; break;
    case 41: polyRepresentation = 0x20000000009; break;
    case 42: polyRepresentation = 0x404c0000001; break;
    case 43: polyRepresentation = 0x80008400021; break;
    case 44: polyRepresentation = 0x108800040001; break;
    case 45: polyRepresentation = 0x208010000011; break;
    case 46: polyRepresentation = 0x410080040001; break;
    case 47: polyRepresentation = 0x800000000021; break;
    case 48: polyRepresentation = 0x1000000080203; break;
    case 49: polyRepresentation = 0x2000000000201; break;
    case 50: polyRepresentation = 0x4000480020001; break;
    case 51: polyRepresentation = 0x8400001008001; break;
    case 52: polyRepresentation = 0x10000000000009; break;
    case 53: polyRepresentation = 0x24020000100001; break;
    case 54: polyRepresentation = 0x62000020000001; break;
    case 55: polyRepresentation = 0x80000001000001; break;
    case 56: polyRepresentation = 0x100028020000001; break;
    case 57: polyRepresentation = 0x200000000000081; break;
    case 58: polyRepresentation = 0x400000000080001; break;
    case 59: polyRepresentation = 0x840400004000001; break;
    case 60: polyRepresentation = 0x1000000000000003; break;

	default: _COMMON_FATAL("No primitive polynomial found for the requested degree");
	}
    
    return polyRepresentation;
}
    
};
    
} //namespace BairToAcsp
} //namespace libstark

#endif //_COMMON_BairToAcsp_COMMONINFORMATION_HPP_
