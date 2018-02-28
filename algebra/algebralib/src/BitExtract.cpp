/** @file
*****************************************************************************
* @author     This file is part of libsnark, developed by SCIPR Lab
*             and contributors (see AUTHORS).
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/

#include "algebraLib/BitExtract.hpp"
#include "algebraLib/ErrorHandling.hpp"
#include <array>

const int DIM=64;
const int matDim = DIM-1;

namespace Algebra {
	
    const FieldElement invExtrType::invExtrArr[invExtrArrSize] = {
		mapIntegerToFieldElement(0, DIM, 1 + 2 + 4 + 8), //-1=xor @0
		mapIntegerToFieldElement(0, DIM, 8 + 2), //0 @1
		mapIntegerToFieldElement(0, DIM, 8 + 4), //1 @2
		mapIntegerToFieldElement(0, DIM, 16 + 8), //2 @3
		mapIntegerToFieldElement(0, DIM, 0x5CB972E5CB972E52), //3 @4
		mapIntegerToFieldElement(0, DIM, 0xDB6DB6DB6DB6DB6A), //4 @5
		mapIntegerToFieldElement(0, DIM, 0xB6DB6DB6DB6DB6CF), //5 @6
		mapIntegerToFieldElement(0, DIM, 0x6DB6DB6DB6DB6D85), //6 @7
		mapIntegerToFieldElement(0, DIM, 0xDB6DB6DB6DB6DB0A), //7 @8
		mapIntegerToFieldElement(0, DIM, 0xB6DB6DB6DB6DB00F), //11 @9
		mapIntegerToFieldElement(0, DIM, 0xDB6DB6DB6DB6C00A), //13 @10
		mapIntegerToFieldElement(0, DIM, 0x6DB6DB6DB6DB0005), //15 @11
		mapIntegerToFieldElement(0, DIM, 0x6DB6DB6DB6D80005), //18 @12
		mapIntegerToFieldElement(0, DIM, 0xDB6DB6DB6D80000A), //22 @13
		mapIntegerToFieldElement(0, DIM, 0xDB6DB6DB6C00000A), //25 @14
		mapIntegerToFieldElement(0, DIM, 0xB6DB6DB60000000F), //32 @15
		mapIntegerToFieldElement(0, DIM, 0xB6DB6D800000000F), //38 @16
		mapIntegerToFieldElement(0, DIM, 0x6DB6D80000000005), //42 @17
		mapIntegerToFieldElement(0, DIM, 0xDB6DB0000000000A), //43 @18
		mapIntegerToFieldElement(0, DIM, 0xDB6C00000000000A), //49 @19
		mapIntegerToFieldElement(0, DIM, 0x6DB0000000000005), //51 @20
		mapIntegerToFieldElement(0, DIM, 0x6C00000000000005),  //57 @21
		mapIntegerToFieldElement(0, DIM, 0xB6DB6DB6DB6DB60F),//8 @22
		mapIntegerToFieldElement(0, DIM, 0x6DB6DB6DB6DB6C05),//9 @23
		mapIntegerToFieldElement(0, DIM, 0xDB6DB6DB6DB6D80A),//10 @24
		mapIntegerToFieldElement(0, DIM, 0x6DB6DB6DB6DB6005),//12 @25
		mapIntegerToFieldElement(0, DIM, 0xB6DB6DB6DB6D800F)//14 @26

	};
	/*inline*/ const FieldElement& invExtrType::operator[](const int i)const{
		switch (i) {
			case -1: return invExtrArr[0];
			case 0: return invExtrArr[1];
			case 1: return invExtrArr[2];
			case 2: return invExtrArr[3];
			case 3: return invExtrArr[4];
			case 4: return invExtrArr[5];
			case 5: return invExtrArr[6];
			case 6: return invExtrArr[7];
			case 7: return invExtrArr[8];
			case 11: return invExtrArr[9];
			case 13: return invExtrArr[10];
			case 15: return invExtrArr[11];
			case 18: return invExtrArr[12];
			case 22: return invExtrArr[13];
			case 25: return invExtrArr[14];
			case 32: return invExtrArr[15];
			case 38: return invExtrArr[16];
			case 42: return invExtrArr[17];
			case 43: return invExtrArr[18];
			case 49: return invExtrArr[19];
			case 51: return invExtrArr[20];
			case 57: return invExtrArr[21];
			case 8: return invExtrArr[22];
			case 9: return invExtrArr[23];
			case 10: return invExtrArr[24];
			case 12: return invExtrArr[25];
			case 14: return invExtrArr[26];

			default: throw;
		}
	}

	const invExtrType invExtrConsts;
	
    /*
     * Boolean matrix inverse
     * Using Gaussian elimination
     */
    std::array<size_t,matDim> getInv(const std::array<size_t,matDim>& src){
        std::array<size_t,matDim> srcMat(src);
        std::array<size_t,matDim> unitMat;
        for(int i=0; i< matDim; i++){
            unitMat[i] = 1UL<<i;
        }

        for(int currCol=0; currCol < matDim; currCol++){
            
            const size_t colMask = 1UL<<currCol;

            //find a row with the column bit set
            if((srcMat[currCol] & colMask) == 0UL) {
                for(int r= currCol+1; r< matDim; r++){
                    if ((srcMat[r] & colMask) != 0UL){
                        srcMat[currCol] ^= srcMat[r];
                        unitMat[currCol] ^= unitMat[r];
                        break;
                    }
                }
            }

            ALGEBRALIB_ASSERT((srcMat[currCol] & colMask) != 0UL, "This should not happen. Could it be the matrix is not invertible?");

            //clear all other bits in this column
            for(int r= 0; r< matDim; r++){
                if (r == currCol)continue;
                if ((srcMat[r] & colMask) != 0UL){
                    srcMat[r] ^= srcMat[currCol];
                    unitMat[r] ^= unitMat[currCol];
                }
            }
        }

        //return the result
        return unitMat;
    }

	std::array<size_t,matDim> matForBitExtr(const int bitNum) {
		std::array<size_t,matDim> M;
        M.fill(0UL);
		const FieldElement invExtractBit = invExtrConsts[bitNum];
		const FieldElement x = xFE();
		FieldElement y = x; //not one;
		
        for (int i = 0; i < matDim; ++i) {
			FieldElement tmp = invExtractBit*(y*y + y);
			size_t v = mapFieldElementToInteger(0,ExtensionDegree,tmp);
			for (int j = 0; j < DIM - 1; ++j){
                if (j < bitNum){
                    M[i] |= (v & 1)<<j;
                }
                else{
                    M[i] |= ((v & 2)>>1)<<j;
                }
                v>>=1;
            }
            y *= x;
		}
		
        return getInv(M);
	}

	//TODO: half-trace method ii.2.4
	FieldElement extractBit(const FieldElement& elem, const int bitNum, const std::array<size_t,matDim>& invM) {
		static const FieldElement x = xFE();
		const size_t tvec = mapFieldElementToInteger(0,ExtensionDegree,elem);
		size_t vcoeffs = 0;
		for (int j = 0; j < DIM - 1; ++j){
			const size_t entryMask = 1UL<<j;
            if (j < bitNum){
				vcoeffs |= tvec & entryMask;
            }
			else{
				vcoeffs |= (tvec>>1) & entryMask;
            }
        }
		
        size_t v = 0;
        // v= vcoeffs * invM
        for(int i=0; i< matDim; i++){
            if((vcoeffs>>i) & 1UL){
                v ^= invM[i];
            }
        }
		FieldElement y = zero();
		FieldElement tmp = x; //1coeff dontcare
		for (int j = 0; j < DIM - 1; ++j) {
			if ((v & (1UL<<j)) != 0){
				y += tmp;
            }
			tmp *= x;
		}
		return y;
	}

	FieldElement extractBit(const FieldElement& elem, const int bitNum) {
		return extractBit(elem, bitNum, matForBitExtr(bitNum));
	}

} // namespace Algebra
