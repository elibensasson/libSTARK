#include "LOCI_AES160hashchain.hpp"

//#define DBGwitness

using namespace Algebra;

namespace AES160LOCIhashchain {

#ifndef DBGwitness
#define DBGGET(a,b,c) (a)[(b)][(c)]
#else
	std::set<std::pair<size_t, int>> dbgSet;
	inline FieldElement& DBGGET(witnessType a, size_t row, int column) {
		std::pair<size_t, int> tmp = std::pair<size_t, int>(row, column);
		dbgSet.insert(tmp);
		return a[row][column];
	}
#endif

	void isZeroDigitCompute(witnessType arr, size_t t, FieldElement NUM, reg::RegType BIT, reg::RegType W, int digit) {
		if ((size_t(1) << digit) & mapFieldElementToInteger(0, EXTDIM, NUM)) {
			DBGGET(arr, t, BIT) = one();
			DBGGET(arr, t, W) = extractBit(mapIntegerToFieldElement(0, EXTDIM, size_t(1) << digit)*arr[t][BIT] + NUM, digit);
		}
		else {
			DBGGET(arr, t, BIT) = zero();
			DBGGET(arr, t, W) = extractBit(NUM, digit);
		}
	}
	void SaveRegisters(witnessType arr, size_t t){
		for (int i = 0; i < NUMREGS; i++){
			DBGGET(arr, t + 1, i) = arr[t][i];
		}
	}


	void SubBytesAUX_REG2(witnessType arr, size_t t, int x, size_t resT, int y, int z, int u, int v, int w){
		DBGGET(arr, t, y) = (arr[t][x] != zero()) ? arr[t][x].inverse() : zero();
		FieldElement a = arr[t][y];
		FieldElement b = a*a;
		DBGGET(arr, t, u) = b*b;//inv^4

		a = arr[t][u];
		b = a*a;
		DBGGET(arr, t, v) = b*b;//inv^16

		a = arr[t][v];
		b = a*a;
		DBGGET(arr, t, w) = b*b;//inv^64

		DBGGET(arr, resT, z) = consts::polyFromMatrix[0] * arr[t][y] + consts::polyFromMatrix[1] * arr[t][y] * arr[t][y]
			+ consts::polyFromMatrix[2] * arr[t][u] + consts::polyFromMatrix[3] * arr[t][u] * arr[t][u]
			+ consts::polyFromMatrix[4] * arr[t][v] + consts::polyFromMatrix[5] * arr[t][v] * arr[t][v]
			+ consts::polyFromMatrix[6] * arr[t][w] + consts::polyFromMatrix[7] * arr[t][w] * arr[t][w] + consts::B_Transformed;

		//std::cout << "---------  check: ----------  x = " << x << arr[t + 1][z] << std::endl;

	}

	void SubBytesAUX_REGMethodA(witnessType arr, size_t t, reg::RegType regs[], size_t resT, reg::RegType res[], int size){
		reg::RegType W = reg::W11;
		for (int i = 0; i < size; i++){
			SubBytesAUX_REG2(arr, t, regs[i], resT, (reg::RegType((int)reg::inv1 + i)),
				res[i], W + i * 3, W + i * 3 + 1, W + i * 3 + 2);
		}
	}

	void MixColumns(witnessType arr, size_t t){
		DBGGET(arr, t + 1, reg::X00) = consts::MixColMat[0][0] * arr[t][reg::X00] + consts::MixColMat[0][1] * arr[t][reg::X10]
			+ consts::MixColMat[0][2] * arr[t][reg::X20] + consts::MixColMat[0][3] * arr[t][reg::X30];

		DBGGET(arr, t + 1, reg::X01) = consts::MixColMat[0][0] * arr[t][reg::X01] + consts::MixColMat[0][1] * arr[t][reg::X11]
			+ consts::MixColMat[0][2] * arr[t][reg::X21] + consts::MixColMat[0][3] * arr[t][reg::X31];

		DBGGET(arr, t + 1, reg::X02) = consts::MixColMat[0][0] * arr[t][reg::X02] + consts::MixColMat[0][1] * arr[t][reg::X12]
			+ consts::MixColMat[0][2] * arr[t][reg::X22] + consts::MixColMat[0][3] * arr[t][reg::X32];

		DBGGET(arr, t + 1, reg::X03) = consts::MixColMat[0][0] * arr[t][reg::X03] + consts::MixColMat[0][1] * arr[t][reg::X13]
			+ consts::MixColMat[0][2] * arr[t][reg::X23] + consts::MixColMat[0][3] * arr[t][reg::X33];

		DBGGET(arr, t + 1, reg::X04) = consts::MixColMat[0][0] * arr[t][reg::X04] + consts::MixColMat[0][1] * arr[t][reg::X14]
			+ consts::MixColMat[0][2] * arr[t][reg::X24] + consts::MixColMat[0][3] * arr[t][reg::X34];



		DBGGET(arr, t + 1, reg::X10) = consts::MixColMat[1][0] * arr[t][reg::X00] + consts::MixColMat[1][1] * arr[t][reg::X10]
			+ consts::MixColMat[1][2] * arr[t][reg::X20] + consts::MixColMat[1][3] * arr[t][reg::X30];

		DBGGET(arr, t + 1, reg::X11) = consts::MixColMat[1][0] * arr[t][reg::X01] + consts::MixColMat[1][1] * arr[t][reg::X11]
			+ consts::MixColMat[1][2] * arr[t][reg::X21] + consts::MixColMat[1][3] * arr[t][reg::X31];

		DBGGET(arr, t + 1, reg::X12) = consts::MixColMat[1][0] * arr[t][reg::X02] + consts::MixColMat[1][1] * arr[t][reg::X12]
			+ consts::MixColMat[1][2] * arr[t][reg::X22] + consts::MixColMat[1][3] * arr[t][reg::X32];

		DBGGET(arr, t + 1, reg::X13) = consts::MixColMat[1][0] * arr[t][reg::X03] + consts::MixColMat[1][1] * arr[t][reg::X13]
			+ consts::MixColMat[1][2] * arr[t][reg::X23] + consts::MixColMat[1][3] * arr[t][reg::X33];

		DBGGET(arr, t + 1, reg::X14) = consts::MixColMat[1][0] * arr[t][reg::X04] + consts::MixColMat[1][1] * arr[t][reg::X14]
			+ consts::MixColMat[1][2] * arr[t][reg::X24] + consts::MixColMat[1][3] * arr[t][reg::X34];


		DBGGET(arr, t + 1, reg::X20) = consts::MixColMat[2][0] * arr[t][reg::X00] + consts::MixColMat[2][1] * arr[t][reg::X10]
			+ consts::MixColMat[2][2] * arr[t][reg::X20] + consts::MixColMat[2][3] * arr[t][reg::X30];

		DBGGET(arr, t + 1, reg::X21) = consts::MixColMat[2][0] * arr[t][reg::X01] + consts::MixColMat[2][1] * arr[t][reg::X11]
			+ consts::MixColMat[2][2] * arr[t][reg::X21] + consts::MixColMat[2][3] * arr[t][reg::X31];

		DBGGET(arr, t + 1, reg::X22) = consts::MixColMat[2][0] * arr[t][reg::X02] + consts::MixColMat[2][1] * arr[t][reg::X12]
			+ consts::MixColMat[2][2] * arr[t][reg::X22] + consts::MixColMat[2][3] * arr[t][reg::X32];

		DBGGET(arr, t + 1, reg::X23) = consts::MixColMat[2][0] * arr[t][reg::X03] + consts::MixColMat[2][1] * arr[t][reg::X13]
			+ consts::MixColMat[2][2] * arr[t][reg::X23] + consts::MixColMat[2][3] * arr[t][reg::X33];

		DBGGET(arr, t + 1, reg::X24) = consts::MixColMat[2][0] * arr[t][reg::X04] + consts::MixColMat[2][1] * arr[t][reg::X14]
			+ consts::MixColMat[2][2] * arr[t][reg::X24] + consts::MixColMat[2][3] * arr[t][reg::X34];


		DBGGET(arr, t + 1, reg::X30) = consts::MixColMat[3][0] * arr[t][reg::X00] + consts::MixColMat[3][1] * arr[t][reg::X10]
			+ consts::MixColMat[3][2] * arr[t][reg::X20] + consts::MixColMat[3][3] * arr[t][reg::X30];

		DBGGET(arr, t + 1, reg::X31) = consts::MixColMat[3][0] * arr[t][reg::X01] + consts::MixColMat[3][1] * arr[t][reg::X11]
			+ consts::MixColMat[3][2] * arr[t][reg::X21] + consts::MixColMat[3][3] * arr[t][reg::X31];

		DBGGET(arr, t + 1, reg::X32) = consts::MixColMat[3][0] * arr[t][reg::X02] + consts::MixColMat[3][1] * arr[t][reg::X12]
			+ consts::MixColMat[3][2] * arr[t][reg::X22] + consts::MixColMat[3][3] * arr[t][reg::X32];

		DBGGET(arr, t + 1, reg::X33) = consts::MixColMat[3][0] * arr[t][reg::X03] + consts::MixColMat[3][1] * arr[t][reg::X13]
			+ consts::MixColMat[3][2] * arr[t][reg::X23] + consts::MixColMat[3][3] * arr[t][reg::X33];

		DBGGET(arr, t + 1, reg::X34) = consts::MixColMat[3][0] * arr[t][reg::X04] + consts::MixColMat[3][1] * arr[t][reg::X14]
			+ consts::MixColMat[3][2] * arr[t][reg::X24] + consts::MixColMat[3][3] * arr[t][reg::X34];

	}


	size_t loopBody(witnessType arr, size_t t, bool isLastRound = false){
		SaveRegisters(arr, t);
		if (arr[t][reg::FLAG1] == zero() && arr[t][reg::FLAG2] == zero()){
			if (arr[t][reg::RC] + consts::Rcon_round11 != zero()){
				DBGGET(arr, t, reg::invRC) = DBGGET(arr, t + 1, reg::invRC) = (arr[t][reg::RC] + consts::Rcon_round11).inverse();
			}
			reg::RegType regs[8] = { reg::X00, reg::X01, reg::X02, reg::X03, reg::X04 };
			reg::RegType res[8] = { reg::X00, reg::X01, reg::X02, reg::X03, reg::X04 };

			SubBytesAUX_REGMethodA(arr, t, regs, t + 1, res, 5);
			DBGGET(arr, t + 1, reg::FLAG2) = one();
		}
		else if (arr[t][reg::FLAG1] == zero() && arr[t][reg::FLAG2] == one()){

			reg::RegType regs[8] = { reg::X10, reg::X11, reg::X12, reg::X13, reg::X14 };
			reg::RegType res[8] = { reg::X14, reg::X10, reg::X11, reg::X12, reg::X13 };
			SubBytesAUX_REGMethodA(arr, t, regs, t + 1, res, 5);
			DBGGET(arr, t + 1, reg::FLAG1) = one();
			DBGGET(arr, t + 1, reg::FLAG2) = zero();
			DBGMSG("t = " << t << ":  -----inv4: " << arr[t][reg::inv4] << "\nX13: " << arr[t][reg::X13] << std::endl);

		}
		else if (arr[t][reg::FLAG1] == one() && arr[t][reg::FLAG2] == zero()){
			reg::RegType regs[8] = { reg::X20, reg::X21, reg::X22, reg::X23, reg::X24 };
			reg::RegType res[8] = { reg::X23, reg::X24, reg::X20, reg::X21, reg::X22 };
			SubBytesAUX_REGMethodA(arr, t, regs, t + 1, res, 5);
			DBGGET(arr, t + 1, reg::FLAG2) = one();

		}
		else if (arr[t][reg::FLAG1] == one() && arr[t][reg::FLAG2] == one()){
			reg::RegType regs[8] = { reg::X30, reg::X31, reg::X32, reg::X33, reg::X34 };
			reg::RegType res[8] = { reg::X32, reg::X33, reg::X34, reg::X30, reg::X31 };
			SubBytesAUX_REGMethodA(arr, t, regs, t + 1, res, 5);
			DBGGET(arr, t + 1, reg::FLAG1) = xFE();

			DBGGET(arr, t + 1, reg::FLAG2) = zero();
		}
		else if (arr[t][reg::FLAG1] == xFE()){
			DBGGET(arr, t + 1, reg::FLAG2) = zero();
			DBGGET(arr, t + 1, reg::invRC) = zero();

			if (!isLastRound)//do it with RC
			{
				DBGGET(arr, t + 1, reg::RC) = arr[t][reg::RC] * consts::xFETransformed;
				MixColumns(arr, t);
				//DBGGET(arr, t + 1, reg::RC) = arr[t][reg::RC] * consts::xFETransformed;
			}
			//arr[t][reg::K13] = zero();
			reg::RegType regs[] = { reg::K14, reg::K24, reg::K34, reg::K04 };
			reg::RegType res[] = { reg::W51, reg::W52, reg::W53, reg::inv5 };

			SubBytesAUX_REGMethodA(arr, t, regs, t, res, 4);

			DBGGET(arr, t + 1, reg::K00) = arr[t][reg::W51] + arr[t][reg::K00] + arr[t][reg::RC];
			DBGGET(arr, t + 1, reg::K10) = arr[t][reg::W52] + arr[t][reg::K10];
			DBGGET(arr, t + 1, reg::K20) = arr[t][reg::W53] + arr[t][reg::K20];
			DBGGET(arr, t + 1, reg::K30) = arr[t][reg::inv5] + arr[t][reg::K30];

			DBGGET(arr, t + 1, reg::K01) = arr[t + 1][reg::K00] + arr[t][reg::K01];
			DBGGET(arr, t + 1, reg::K11) = arr[t + 1][reg::K10] + arr[t][reg::K11];
			DBGGET(arr, t + 1, reg::K21) = arr[t + 1][reg::K20] + arr[t][reg::K21];
			DBGGET(arr, t + 1, reg::K31) = arr[t + 1][reg::K30] + arr[t][reg::K31];

			DBGGET(arr, t + 1, reg::K02) = arr[t + 1][reg::K01] + arr[t][reg::K02];
			DBGGET(arr, t + 1, reg::K12) = arr[t + 1][reg::K11] + arr[t][reg::K12];
			DBGGET(arr, t + 1, reg::K22) = arr[t + 1][reg::K21] + arr[t][reg::K22];
			DBGGET(arr, t + 1, reg::K32) = arr[t + 1][reg::K31] + arr[t][reg::K32];

			DBGGET(arr, t + 1, reg::K03) = arr[t + 1][reg::K02] + arr[t][reg::K03];
			DBGGET(arr, t + 1, reg::K13) = arr[t + 1][reg::K12] + arr[t][reg::K13];
			DBGGET(arr, t + 1, reg::K23) = arr[t + 1][reg::K22] + arr[t][reg::K23];
			DBGGET(arr, t + 1, reg::K33) = arr[t + 1][reg::K32] + arr[t][reg::K33];

			DBGGET(arr, t + 1, reg::K04) = arr[t + 1][reg::K03] + arr[t][reg::K04];
			DBGGET(arr, t + 1, reg::K14) = arr[t + 1][reg::K13] + arr[t][reg::K14];
			DBGGET(arr, t + 1, reg::K24) = arr[t + 1][reg::K23] + arr[t][reg::K24];
			DBGGET(arr, t + 1, reg::K34) = arr[t + 1][reg::K33] + arr[t][reg::K34];

			for (int w = 0; w < 20; w++){
				DBGGET(arr, t + 1, reg::X00 + w) = arr[t + 1][reg::X00 + w] + arr[t + 1][(reg::K00 + w)];

			}

			DBGGET(arr, t, reg::W51) = zero();
			DBGGET(arr, t, reg::W52) = zero();
			DBGGET(arr, t, reg::W53) = zero();
			DBGGET(arr, t, reg::inv5) = zero();

			if (isLastRound)
			{
				DBGGET(arr, t + 1, reg::FLAG2) = zero();
				DBGGET(arr, t + 1, reg::invRC) = one();
				//DBGGET(arr, t + 1, reg::RC) = zero();
				//DBGGET(arr, t + 1, reg::STATE) = xFE();
			}
			else{
				DBGGET(arr, t + 1, reg::FLAG1) = DBGGET(arr, t + 1, reg::FLAG2) = zero();

			}
		}
		else {
			DBGMSG("ERROR: need satisfy one condition");
		}

		return t + 1;
	}

	size_t genWitnessAES160(witnessType arr, size_t t){
		DBGMSG("tppppp = " << t << ": flag1 = " << arr[t][reg::FLAG1]);
		DBGMSG("tppppp = " << t << ": flag2 = " << arr[t][reg::FLAG2]);
		while (arr[t][reg::RC] != consts::Rcon_round11)
		{
			t = loopBody(arr, t);

		}


		t = loopBody(arr, t, true);
		t = loopBody(arr, t, true);
		t = loopBody(arr, t, true);
		t = loopBody(arr, t, true);

		t = loopBody(arr, t, true);
		DBGMSG("t......... = " << t << ": res1 = " << arr[t][reg::K20]);
		return t;

	}


	size_t computeAES160(witnessType arr){
		FieldElement RC = one();
		std::vector<FieldElement> arr2(20);
		for (int i = 0; i < 20; i++){
			arr2[i] = arr[0][reg::X00 + i];
		}
		size_t t = 0;
		for (RC = one(); RC != consts::Rcon_round11*consts::xFETransformed; RC *= consts::xFETransformed){

			reg::RegType regs[8] = { reg::X00, reg::X01, reg::X02, reg::X03, reg::X04 };
			reg::RegType res[8] = { reg::X00, reg::X01, reg::X02, reg::X03, reg::X04 };
			SubBytesAUX_REGMethodA(arr, t, regs, t + 1, res, 5);

			reg::RegType regs2[8] = { reg::X10, reg::X11, reg::X12, reg::X13, reg::X14 };
			reg::RegType res2[8] = { reg::X14, reg::X10, reg::X11, reg::X12, reg::X13 };
			SubBytesAUX_REGMethodA(arr, t, regs2, t + 1, res2, 5);

			reg::RegType regs3[8] = { reg::X20, reg::X21, reg::X22, reg::X23, reg::X24 };
			reg::RegType res3[8] = { reg::X23, reg::X24, reg::X20, reg::X21, reg::X22 };
			SubBytesAUX_REGMethodA(arr, t, regs3, t + 1, res3, 5);

			reg::RegType regs4[8] = { reg::X30, reg::X31, reg::X32, reg::X33, reg::X34 };
			reg::RegType res4[8] = { reg::X32, reg::X33, reg::X34, reg::X30, reg::X31 };
			SubBytesAUX_REGMethodA(arr, t, regs4, t + 1, res4, 5);

			for (int i = 0; i < 20; i++){
				arr[t][reg::X00 + i] = arr[t + 1][reg::X00 + i];
			}

			if (RC != consts::Rcon_round11)//do it with RC
			{
				MixColumns(arr, t);
				//DBGGET(arr, t + 1, reg::RC) = arr[t][reg::RC] * consts::xFETransformed;
			}
			//arr[t][reg::K13] = zero();
			reg::RegType regs5[] = { reg::K14, reg::K24, reg::K34, reg::K04 };
			reg::RegType res5[] = { reg::W51, reg::W52, reg::W53, reg::inv5 };

			SubBytesAUX_REGMethodA(arr, t, regs5, t, res5, 4);

			DBGGET(arr, t + 1, reg::K00) = arr[t][reg::W51] + arr[t][reg::K00] + RC;
			DBGGET(arr, t + 1, reg::K10) = arr[t][reg::W52] + arr[t][reg::K10];
			DBGGET(arr, t + 1, reg::K20) = arr[t][reg::W53] + arr[t][reg::K20];
			DBGGET(arr, t + 1, reg::K30) = arr[t][reg::inv5] + arr[t][reg::K30];

			DBGGET(arr, t + 1, reg::K01) = arr[t + 1][reg::K00] + arr[t][reg::K01];
			DBGGET(arr, t + 1, reg::K11) = arr[t + 1][reg::K10] + arr[t][reg::K11];
			DBGGET(arr, t + 1, reg::K21) = arr[t + 1][reg::K20] + arr[t][reg::K21];
			DBGGET(arr, t + 1, reg::K31) = arr[t + 1][reg::K30] + arr[t][reg::K31];

			DBGGET(arr, t + 1, reg::K02) = arr[t + 1][reg::K01] + arr[t][reg::K02];
			DBGGET(arr, t + 1, reg::K12) = arr[t + 1][reg::K11] + arr[t][reg::K12];
			DBGGET(arr, t + 1, reg::K22) = arr[t + 1][reg::K21] + arr[t][reg::K22];
			DBGGET(arr, t + 1, reg::K32) = arr[t + 1][reg::K31] + arr[t][reg::K32];

			DBGGET(arr, t + 1, reg::K03) = arr[t + 1][reg::K02] + arr[t][reg::K03];
			DBGGET(arr, t + 1, reg::K13) = arr[t + 1][reg::K12] + arr[t][reg::K13];
			DBGGET(arr, t + 1, reg::K23) = arr[t + 1][reg::K22] + arr[t][reg::K23];
			DBGGET(arr, t + 1, reg::K33) = arr[t + 1][reg::K32] + arr[t][reg::K33];

			DBGGET(arr, t + 1, reg::K04) = arr[t + 1][reg::K03] + arr[t][reg::K04];
			DBGGET(arr, t + 1, reg::K14) = arr[t + 1][reg::K13] + arr[t][reg::K14];
			DBGGET(arr, t + 1, reg::K24) = arr[t + 1][reg::K23] + arr[t][reg::K24];
			DBGGET(arr, t + 1, reg::K34) = arr[t + 1][reg::K33] + arr[t][reg::K34];

			for (int w = 0; w < 20; w++){
				DBGGET(arr, t + 1, reg::X00 + w) = arr[t + 1][reg::X00 + w] + arr[t + 1][(reg::K00 + w)];

			}
			t++;
		}
		for (int i = 0; i < 20; i++)
			arr[t][reg::X00 + i] += arr2[i];
		return t;
	}


	size_t genWitnessHashByAES160(witnessType arr, size_t t, size_t stepOfCollapsing){
		DBGGET(arr, t, reg::RC) = one();
		DBGGET(arr, t, reg::STATE) = one();
		DBGGET(arr, t, reg::FLAG1) = zero();
		DBGGET(arr, t, reg::FLAG2) = zero();
		stepOfCollapsing = t;
		DBGMSG("PLAESEPPPPPPPPPPPPPPPPp!\n\nt = " << t << ": A = " << arr[t][reg::A] << std::endl << std::endl);
		t = genWitnessAES160(arr, t);
		SaveRegisters(arr, t);
		DBGGET(arr, t, reg::STATE) = xFE();
		DBGGET(arr, t, reg::FLAG1) = zero();
		DBGGET(arr, t, reg::FLAG2) = zero();
		reg::RegType regs[10] = { reg::X00, reg::X10, reg::X20, reg::X30, reg::X01, reg::X11, reg::X21, reg::X31, reg::X04, reg::X14 };
		//DBGGET(arr, t, reg::invRC) = one();

		
		for (int i = 0; i < 10; i++){
			//TODO: it is problem because I use K_ij without justification. no need to fix it in the generate witness,
			//need to verify computing K-ij of last round of aes160 in the constraints polynomial without connection to K-ij registers.
			DBGGET(arr, t + 1, regs[i]) = arr[t][regs[i]] + arr[stepOfCollapsing][regs[i]];
			DBGGET(arr, t, reg::K00 + 4 * i) = arr[stepOfCollapsing][regs[i]];
			DBGGET(arr, t, reg::K00 + 4 * i + 1) = power(arr[t][reg::K00 + 4 * i], 4);
			DBGGET(arr, t, reg::K00 + 4 * i + 2) = power(arr[t][reg::K00 + 4 * i + 1], 4);
			DBGGET(arr, t, reg::K00 + 4 * i + 3) = power(arr[t][reg::K00 + 4 * i + 2], 4);
		}
		DBGGET(arr, t + 1, reg::A) = arr[stepOfCollapsing][reg::X04] + xFE()*arr[stepOfCollapsing][reg::X14];

		t++;

		SaveRegisters(arr, t);

		DBGGET(arr, t, reg::STATE) = xFE();
		DBGGET(arr, t, reg::FLAG1) = zero();
		DBGGET(arr, t, reg::FLAG2) = one();

		reg::RegType regs2[10] = { reg::X02, reg::X12, reg::X22, reg::X32, reg::X03, reg::X13, reg::X23, reg::X33, reg::X24, reg::X34 };
		for (int i = 0; i < 10; i++){
			//TODO: it is problem because I use K_ij without justification. no need to fix it in the generate witness,
			//need to verify computing K-ij of last round of aes160 in the constraints polynomial without connection to K-ij registers.
			DBGGET(arr, t + 1, regs2[i]) = arr[t][regs2[i]] + arr[stepOfCollapsing][regs2[i]];
			DBGGET(arr, t, reg::K00 + 4 * i) = arr[stepOfCollapsing][regs2[i]];
			DBGGET(arr, t, reg::K00 + 4 * i + 1) = power(arr[t][reg::K00 + 4 * i], 4);
			DBGGET(arr, t, reg::K00 + 4 * i + 2) = power(arr[t][reg::K00 + 4 * i + 1], 4);
			DBGGET(arr, t, reg::K00 + 4 * i + 3) = power(arr[t][reg::K00 + 4 * i + 2], 4);
		}
		return t + 1;
	}

	std::vector<FieldElement> genHashchain(witnessType HashChain, const database_t& db){
		const unsigned int len = db.size()*2;
        HashChain.resize(len);
		for (auto &i : HashChain) i.resize(20);

		for (unsigned int k = 0; k < db.size(); k++){
			for (int i = 0; i < 20; i++){
                for(int j=0; j<2; j++){
				int val = db[k][i][j];
				HashChain[2*k+j][i] = (255 == val) ? Algebra::zero() : power(consts::xFETransformed, val);
                }
            }
		}
		std::vector< std::vector<FieldElement> > myArr2(12, std::vector<FieldElement>(NUMREGS));
		size_t t = 0;
		for (int j = 0; j < 20; j++){
			myArr2[0][reg::X00 + j] = zero();
			myArr2[0][reg::K00 + j] = zero();
			}
		
		for (unsigned int k = 0; k < len; k++){

			for (int j = 0; j < 20; j++){
				myArr2[0][reg::K00 + j] = myArr2[t][reg::X00 + j];
				myArr2[0][reg::X00 + j] = HashChain[k][j];
			}
			t = computeAES160(myArr2);

			DBGMSG(k);
		}

		return myArr2[t];
	}
	inline FieldElement max(FieldElement a, FieldElement b){
		if (mapFieldElementToInteger(0, EXTDIM, a) >= mapFieldElementToInteger(0, EXTDIM, b)){
			return a;
		}
		return b;
	}

	size_t genWitness(witnessType arr, const witnessType HashChain, const int N, const FieldElement loci_constants[][2]){
		size_t t = 0;
		for (int i = 0; i < NUMREGS; i++){
			DBGGET(arr, t, i) = zero();
		}

		FieldElement lastStep = power(xFE(), N);
		arr[t][reg::K] = one();
		int counter = 0;
		while (1) {
			SaveRegisters(arr, t);
		
			for (int i = 0; i < 20; i++){
				DBGGET(arr, t + 1, reg::X00 + i) = HashChain[counter][i];
			}
			DBGGET(arr, t, reg::PHASE) = zero();

			int secondCompareLoci = 0;
			FieldElement lastFlag = xFE();

			if (arr[t][reg::isSecondPhaseComparingLOCI] == one()){
				secondCompareLoci = 1;
				lastFlag = arr[t][reg::PartialMATCH];
			}
			FieldElement SaveLastFlagBeforeLOCI = lastFlag;
			for (int i = 0; i < 10; i++)
			{
				int nextFlagIndex = i != 9 ? (reg::FLAG1 + i) : reg::PartialMATCH;
				int AUX_REGRegIndex = (i < 5 ? (reg::X00 + 4 * i) : (reg::inv1 + 4 * (i - 5)));
				DBGMSG("lastFlag = " << lastFlag);
				FieldElement tmp = arr[t + 1][reg::X00 + i] + loci_constants[i + 10 * secondCompareLoci][0];
				//DBGMSG("tmp = " << tmp);
				DBGGET(arr, t, AUX_REGRegIndex) = (tmp == zero()) ? zero() : tmp.inverse();
				tmp = arr[t + 1][reg::X00 + i] + loci_constants[i + 10 * secondCompareLoci][1];
				//DBGMSG("tmp = " << tmp);
				DBGGET(arr, t, AUX_REGRegIndex + 1) = (tmp == zero()) ? zero() : tmp.inverse();
				tmp = arr[t + 1][reg::X20 + i] + loci_constants[i + 10 * secondCompareLoci][0];
				//DBGMSG("tmp = " << tmp);
				DBGGET(arr, t, AUX_REGRegIndex + 2) = (tmp == zero()) ? zero() : tmp.inverse();
				tmp = arr[t + 1][reg::X20 + i] + loci_constants[i + 10 * secondCompareLoci][1];
				//DBGMSG("tmp = " << tmp);
				DBGGET(arr, t, AUX_REGRegIndex + 3) = (tmp == zero()) ? zero() : tmp.inverse();

				if (lastFlag == zero()){
					DBGGET(arr, t, nextFlagIndex) = zero();
				}
				else if (arr[t][AUX_REGRegIndex] == zero()){
					if (arr[t][AUX_REGRegIndex + 3] == zero()){
						DBGGET(arr, t, nextFlagIndex) = lastFlag;
						DBGMSG("t = " << t << ": reg enum = " << nextFlagIndex);

					}
					else{
						if (lastFlag != zero()){
							DBGGET(arr, t, nextFlagIndex) = one();
						}
					}
				}
				else if (arr[t][AUX_REGRegIndex + 1] == zero()){
					if (arr[t][AUX_REGRegIndex + 2] == zero()){
						DBGGET(arr, t, nextFlagIndex) = lastFlag;
						DBGMSG("t = " << t << ": reg enum = " << nextFlagIndex);

					}
					else{
						if (lastFlag != zero()){
							DBGGET(arr, t, nextFlagIndex) = one();
						}
					}
				}
				else {
					if (arr[t][AUX_REGRegIndex + 2] == zero() || arr[t][AUX_REGRegIndex + 3] == zero()){
						if (lastFlag != zero()){
							DBGGET(arr, t, nextFlagIndex) = one();
						}
					}
					else{
						DBGGET(arr, t, nextFlagIndex) = zero();
					}
				}
				DBGMSG("nextFlag" << arr[t][nextFlagIndex]);
				lastFlag = arr[t][reg::FLAG1 + i];
			}
			DBGGET(arr, t + 1,reg::isSecondPhaseComparingLOCI) = arr[t][reg::isSecondPhaseComparingLOCI] + one();
			DBGGET(arr, t + 1, reg::PartialMATCH) = arr[t][reg::PartialMATCH];
			DBGGET(arr, t, reg::PartialMATCH) = SaveLastFlagBeforeLOCI;
			if (secondCompareLoci){
				DBGGET(arr, t + 1, reg::MATCH) = max(arr[t][reg::MATCH], arr[t + 1][reg::PartialMATCH]);
			}
			DBGGET(arr, t + 1, reg::A) = (arr[t + 1][reg::X00] +
				xFE()*(arr[t + 1][reg::X10] +
				xFE()*(arr[t + 1][reg::X20] +
				xFE()*(arr[t + 1][reg::X30] +
				xFE()*(arr[t + 1][reg::X01] +
				xFE()*(arr[t + 1][reg::X11] +
				xFE()*(arr[t + 1][reg::X21] +
				xFE()*(arr[t + 1][reg::X31]))))))));

			DBGGET(arr, t + 1, reg::B) = (arr[t + 1][reg::X02] +
				xFE()*(arr[t + 1][reg::X12] +
				xFE()*(arr[t + 1][reg::X22] +
				xFE()*(arr[t + 1][reg::X32] +
				xFE()*(arr[t + 1][reg::X03] +
				xFE()*(arr[t + 1][reg::X13] +
				xFE()*(arr[t + 1][reg::X23] +
				xFE()*(arr[t + 1][reg::X33]))))))));
			
			DBGGET(arr, t + 1, reg::C) = (arr[t + 1][reg::X04] +
				xFE()*(arr[t + 1][reg::X14] +
				xFE()*(arr[t + 1][reg::X24] +
				xFE()*(arr[t + 1][reg::X34]))));
			
			t++;
			DBGGET(arr, t, reg::PHASE) = one();
			SaveRegisters(arr, t);
			t = genWitnessHashByAES160(arr, t, t);
			SaveRegisters(arr, t);
			for (int i = 0; i < 20; i++){
				arr[t + 1][reg::K00 + i] = arr[t][reg::X00 + i];
			}

			DBGGET(arr, t, reg::STATE) = xFE();
			DBGGET(arr, t, reg::FLAG1) = one();
			DBGGET(arr, t, reg::FLAG2) = one();

			if (lastStep + xFE()*arr[t][reg::K] != zero()){
				DBGGET(arr, t, reg::A) = (lastStep + xFE()*arr[t][reg::K]).inverse();
				DBGGET(arr, t + 1, reg::K) = xFE()*arr[t][reg::K];
			}
			else{
				break;
			}
			t++;
			counter++;
		}
		

		return t + 1;
	}

	void genWitnessLOCIHashcAES160WithPadding(witnessType arr, const witnessType HashChain, const int LEN, const fingerprint_t& fprint){
		FieldElement loci_pairs[20][2];
		for (int i = 0; i < 20; i++){
            for(int j=0; j<2; j++){
			unsigned int bits = fprint[i][j];
			loci_pairs[i][j] = (255 == bits) ? Algebra::zero() : power(consts::xFETransformed, bits);
            }
		}
		size_t t = genWitness(arr, HashChain, LEN, loci_pairs);
		DBGMSG("t = " << t << std::endl);
		DBGGET(arr, t, reg::PHASE) = one();
		DBGGET(arr, t, reg::STATE) = xFE();
		DBGGET(arr, t, reg::FLAG1) = one();
		DBGGET(arr, t, reg::FLAG2) = one();
		for (; t < arr.size() - 1; t++){
			SaveRegisters(arr, t);
			for (int i = 0; i < 20; i++){
				arr[t][reg::K00 + i] = zero();
				arr[t][reg::inv1 + i] = one();
				arr[t + 1][reg::X00 + i] = arr[t][reg::X00 + i];
			}
			DBGGET(arr, t + 1, reg::PHASE) = one();
			DBGGET(arr, t + 1, reg::STATE) = xFE();
			DBGGET(arr, t + 1, reg::FLAG1) = one();
			DBGGET(arr, t + 1, reg::FLAG2) = one();
			DBGGET(arr, t + 1, reg::K) = arr[t][reg::K];
		}
	}
	short getDim(long long len){
		return ceil(Infrastructure::Log2(59LL * len));
	}

} //namespace
