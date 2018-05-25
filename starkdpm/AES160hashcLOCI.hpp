#ifndef __ACSP_FOR_AES160hashcLOCI_HPP__
#define __ACSP_FOR_AES160hashcLOCI_HPP__

#include "languages/Bair/BairInstance.hpp"
#include "languages/Bair/BairWitness.hpp"

#include <algebraLib/BitExtract.hpp>

#define PRNMSG(str) do { std::cout << str << std::endl; } while( false )
//#define DBGMSG(str) do { std::cout << str << std::endl; } while( false )
#define DBGMSG(str) do { } while ( false )

#define EXTDIM 64

using namespace Algebra;

namespace AES160hashcLOCI {
    typedef std::array<uint8_t,2> dpmpair_t;
    typedef std::array<dpmpair_t,20> fingerprint_t;
	typedef std::vector<fingerprint_t> database_t;

	//FieldElement eval(const std::vector<FieldElement>& vars, const std::vector<FieldElement> RootHash, const FieldElement values[][2], FieldElement lastPow);
	FieldElement evalCPoly(const std::vector<FieldElement> & vars,
		const std::vector<FieldElement> & RootHash, const FieldElement& gN);


	class evalp
	{
		public:
			static void setParams(const std::vector<FieldElement>&, const FieldElement&, const int);
			static FieldElement ep(const std::vector<FieldElement>&);
		private:
			static std::vector<FieldElement> rHash;
			static FieldElement last_leaf_index;
	};

	const short NUMREGS = 81;

	namespace reg {
		typedef enum RegType{

			B00 = 0, B01, B02, B03, B04,
			B10, B11, B12, B13, B14,
			B20, B21, B22, B23, B24,
			B30, B31, B32, B33, B34,

			K00, K01, K02, K03, K04,
			K10, K11, K12, K13, K14,
			K20, K21, K22, K23, K24,
			K30, K31, K32, K33, K34,

			inv1, inv2, inv3, inv4, inv5,
			W11, W12, W13,
			W21, W22, W23,
			W31, W32, W33,
			W41, W42, W43,
			W51, W52, W53,
			
			FLAG1, FLAG2, RC, invRC,

			A, B, C, STATE,
			K,
			MATCH, isSecondPhaseComparingLOCI, PartialMATCH, PHASE,
			L1, L2, L3, L4, L5, L6,
			ST2, ST3,
		} RegType;
	}

	typedef std::vector< std::vector<FieldElement> > & witnessType;
	short getDim(long long);
    std::vector<FieldElement> genHashchain(witnessType, const database_t&);
	void genWitnessLOCIHashcAES160WithPadding(witnessType, const witnessType, int, const fingerprint_t&);

	namespace consts {
#define zFE (mapIntegerToFieldElement(0, EXTDIM, 0))

		const short ROUNDS = 10;
		const FieldElement xFEinv = xFE().inverse();
		const FieldElement minus1 = mapIntegerToFieldElement(0, EXTDIM, 0xfffff);
		const FieldElement oneTransformed = mapIntegerToFieldElement(0, EXTDIM, 0x1);
		const FieldElement xFETransformed = mapIntegerToFieldElement(0, EXTDIM, 0x33CE8BEDDC8A656);
		const FieldElement xFEAndOneTransformed = mapIntegerToFieldElement(0, EXTDIM, 0x33CE8BEDDC8A657);
		const FieldElement B_Transformed = mapIntegerToFieldElement(0, EXTDIM, 0xFF6C97771E353011);


		const FieldElement Rcon_round11 =  xFETransformed *xFETransformed*xFETransformed*xFETransformed*
			xFETransformed*xFETransformed*xFETransformed*xFETransformed*xFETransformed*xFETransformed;//

		const FieldElement xFE_4 = mapIntegerToFieldElement(0, EXTDIM, size_t(1) << 4);
		const FieldElement xFE_8 = mapIntegerToFieldElement(0, EXTDIM, size_t(1) << 8);
		const FieldElement xFE_minus4 = xFE_4.inverse();
		const FieldElement xFE_minus8 = xFE_minus4*xFE_minus4;
		
		const FieldElement Transformator[8] = {
			
			mapIntegerToFieldElement(0, EXTDIM, 0x1),
			mapIntegerToFieldElement(0, EXTDIM, 0x33CE8BEDDC8A656), 
			mapIntegerToFieldElement(0, EXTDIM, 0x512620375ED2A108),
			mapIntegerToFieldElement(0, EXTDIM, 0xC9E636090AAFC01),
			
			
			mapIntegerToFieldElement(0, EXTDIM, 0xBA4F3CD82801769C),
			mapIntegerToFieldElement(0, EXTDIM, 0xBA26E7904ADB4A47), 
			mapIntegerToFieldElement(0, EXTDIM, 0x467698598926DC01),
			mapIntegerToFieldElement(0, EXTDIM, 0x4418AE808B28BDD0)
			
		};
		
		/*const FieldElement Transformator[8] = {
			mapIntegerToFieldElement(0, EXTDIM, 0x4418AE808B28BDD0),
			mapIntegerToFieldElement(0, EXTDIM, 0x467698598926DC01),
			mapIntegerToFieldElement(0, EXTDIM, 0xBA26E7904ADB4A47),
			mapIntegerToFieldElement(0, EXTDIM, 0xBA4F3CD82801769C),
			mapIntegerToFieldElement(0, EXTDIM, 0xC9E636090AAFC01),
			mapIntegerToFieldElement(0, EXTDIM, 0x512620375ED2A108),
			mapIntegerToFieldElement(0, EXTDIM, 0x33CE8BEDDC8A656),
			mapIntegerToFieldElement(0, EXTDIM, 0x1)
		};*/
		
		const FieldElement MixColMat[4][4] = {
			{ xFETransformed, xFEAndOneTransformed, oneTransformed, oneTransformed },
			{ oneTransformed, xFETransformed, xFEAndOneTransformed, oneTransformed },
			{ oneTransformed, oneTransformed, xFETransformed, xFEAndOneTransformed },
			{ xFEAndOneTransformed, oneTransformed, oneTransformed, xFETransformed }
		};

		const FieldElement invTransformator[14] = {
			mapIntegerToFieldElement(0, EXTDIM, 0x1),
			mapIntegerToFieldElement(0, EXTDIM, 0xFD),
			mapIntegerToFieldElement(0, EXTDIM, 0x7F),
			mapIntegerToFieldElement(0, EXTDIM, 0x4C),
			mapIntegerToFieldElement(0, EXTDIM, 0x2A),
			zFE,
			mapIntegerToFieldElement(0, EXTDIM, 0xA3),
			zFE, zFE, zFE,
			mapIntegerToFieldElement(0, EXTDIM, 0x41),
			zFE, zFE,
			mapIntegerToFieldElement(0, EXTDIM, 0x48)
		};

		const FieldElement polyFromMatrix[8] = {
			mapIntegerToFieldElement(0, EXTDIM, 0x512620375ED2A109),
			mapIntegerToFieldElement(0, EXTDIM, 0xC9E636090AAFC00),
			mapIntegerToFieldElement(0, EXTDIM, 0xE998EF1F07EA10A),
			mapIntegerToFieldElement(0, EXTDIM, 0xEB00C7A71409EB4E),
			mapIntegerToFieldElement(0, EXTDIM, 0x5321CDA63E06FC02),
			mapIntegerToFieldElement(0, EXTDIM, 0x1),
			mapIntegerToFieldElement(0, EXTDIM, 0x155755FFB7202002),
			mapIntegerToFieldElement(0, EXTDIM, 0x1A9C05699898468E)
		};
	}
}

namespace stark_dpm{
	namespace ACSP_FOR_AES160hashcLOCI{
		typedef struct{
			long long length;
			std::vector<Algebra::FieldElement> head;
		}AES160hashcLOCICommonParams;

        libstark::BairInstance buildBairInstance(const AES160hashcLOCICommonParams&);
        libstark::BairWitness buildBairWitness(const AES160hashcLOCICommonParams&, const AES160hashcLOCI::witnessType&, const AES160hashcLOCI::fingerprint_t&);
	}
}

#endif // __ACSP_FOR_AES160hashcLOCI_HPP__ 
