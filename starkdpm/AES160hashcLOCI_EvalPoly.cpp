#include "AES160hashcLOCI.hpp"

#define RI(n) n
//#define RI(n) 0
#define RN 643

using namespace Algebra;

namespace AES160hashcLOCI{

	FieldElement randCoeff[RN];
	//namespace { //anonymous namespace for polys and common vars


	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_00(const std::vector<FieldElement> & vars, FieldElement results[]){
		FieldElement tval = vars[reg::FLAG1 + NUMREGS];
		tval += randCoeff[RI(0)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(1)] * (vars[reg::B10 + NUMREGS] + vars[reg::B10]);
		tval += randCoeff[RI(2)] * (vars[reg::B11 + NUMREGS] + vars[reg::B11]);
		tval += randCoeff[RI(3)] * (vars[reg::B12 + NUMREGS] + vars[reg::B12]);
		tval += randCoeff[RI(4)] * (vars[reg::B13 + NUMREGS] + vars[reg::B13]);
		tval += randCoeff[RI(5)] * (vars[reg::B14 + NUMREGS] + vars[reg::B14]);
		tval += randCoeff[RI(6)] * (vars[reg::B20 + NUMREGS] + vars[reg::B20]);
		tval += randCoeff[RI(7)] * (vars[reg::B21 + NUMREGS] + vars[reg::B21]);
		tval += randCoeff[RI(8)] * (vars[reg::B22 + NUMREGS] + vars[reg::B22]);
		tval += randCoeff[RI(9)] * (vars[reg::B23 + NUMREGS] + vars[reg::B23]);
		tval += randCoeff[RI(10)] * (vars[reg::B24 + NUMREGS] + vars[reg::B24]);
		tval += randCoeff[RI(11)] * (vars[reg::B30 + NUMREGS] + vars[reg::B30]);
		tval += randCoeff[RI(12)] * (vars[reg::B31 + NUMREGS] + vars[reg::B31]);
		tval += randCoeff[RI(13)] * (vars[reg::B32 + NUMREGS] + vars[reg::B32]);
		tval += randCoeff[RI(14)] * (vars[reg::B33 + NUMREGS] + vars[reg::B33]);
		tval += randCoeff[RI(15)] * (vars[reg::B34 + NUMREGS] + vars[reg::B34]);

		tval += randCoeff[RI(16)] * (vars[reg::B00] * vars[reg::inv1] + one())*(vars[reg::B00] + randCoeff[RI(17)] * vars[reg::inv1]);
		tval += randCoeff[RI(18)] * (
			vars[reg::B00 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(19)] * (vars[reg::B01] * vars[reg::inv2] + one())*(vars[reg::B01] + randCoeff[RI(20)] * vars[reg::inv2]);
		tval += randCoeff[RI(21)] * (
			vars[reg::B01 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(22)] * (vars[reg::B02] * vars[reg::inv3] + one())*(vars[reg::B02] + randCoeff[RI(23)] * vars[reg::inv3]);
		tval += randCoeff[RI(24)] * (
			vars[reg::B02 + NUMREGS] +
			results[2]);

		tval += randCoeff[RI(25)] * (vars[reg::B03] * vars[reg::inv4] + one())*(vars[reg::B03] + randCoeff[RI(26)] * vars[reg::inv4]);
		tval += randCoeff[RI(27)] * (
			vars[reg::B03 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(28)] * (vars[reg::B04] * vars[reg::inv5] + one())*(vars[reg::B04] + randCoeff[RI(29)] * vars[reg::inv5]);
		tval += randCoeff[RI(30)] * (
			vars[reg::B04 + NUMREGS] +
			results[4]);

		return tval;
	}
	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_01(const std::vector<FieldElement> & vars, FieldElement results[]){
		FieldElement tval = zero();
		tval += vars[reg::FLAG1 + NUMREGS] + one();
		tval += randCoeff[RI(31)] * (vars[reg::FLAG2 + NUMREGS] + zero());
		tval += randCoeff[RI(32)] * (vars[reg::B00 + NUMREGS] + vars[reg::B00]);
		tval += randCoeff[RI(33)] * (vars[reg::B01 + NUMREGS] + vars[reg::B01]);
		tval += randCoeff[RI(34)] * (vars[reg::B02 + NUMREGS] + vars[reg::B02]);
		tval += randCoeff[RI(35)] * (vars[reg::B03 + NUMREGS] + vars[reg::B03]);
		tval += randCoeff[RI(36)] * (vars[reg::B04 + NUMREGS] + vars[reg::B04]);
		tval += randCoeff[RI(37)] * (vars[reg::B20 + NUMREGS] + vars[reg::B20]);
		tval += randCoeff[RI(38)] * (vars[reg::B21 + NUMREGS] + vars[reg::B21]);
		tval += randCoeff[RI(39)] * (vars[reg::B22 + NUMREGS] + vars[reg::B22]);
		tval += randCoeff[RI(40)] * (vars[reg::B23 + NUMREGS] + vars[reg::B23]);
		tval += randCoeff[RI(41)] * (vars[reg::B24 + NUMREGS] + vars[reg::B24]);
		tval += randCoeff[RI(42)] * (vars[reg::B30 + NUMREGS] + vars[reg::B30]);
		tval += randCoeff[RI(43)] * (vars[reg::B31 + NUMREGS] + vars[reg::B31]);
		tval += randCoeff[RI(44)] * (vars[reg::B32 + NUMREGS] + vars[reg::B32]);
		tval += randCoeff[RI(45)] * (vars[reg::B33 + NUMREGS] + vars[reg::B33]);
		tval += randCoeff[RI(46)] * (vars[reg::B34 + NUMREGS] + vars[reg::B34]);

		tval += randCoeff[RI(47)] * (vars[reg::B10] * vars[reg::inv1] + one())*(vars[reg::B10] + randCoeff[RI(48)] * vars[reg::inv1]);
		tval += randCoeff[RI(49)] * (
			vars[reg::B14 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(50)] * (vars[reg::B11] * vars[reg::inv2] + one())*(vars[reg::B11] + randCoeff[RI(51)] * vars[reg::inv2]);
		tval += randCoeff[RI(52)] * (
			vars[reg::B10 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(53)] * (vars[reg::B12] * vars[reg::inv3] + one())*(vars[reg::B12] + randCoeff[RI(54)] * vars[reg::inv3]);
		tval += randCoeff[RI(55)] * (
			vars[reg::B11 + NUMREGS] +
			results[2]);
		
		tval += randCoeff[RI(56)] * (vars[reg::B13] * vars[reg::inv4] + one())*(vars[reg::B13] + randCoeff[RI(57)] * vars[reg::inv4]);
		tval += randCoeff[RI(58)] * (
			vars[reg::B12 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(59)] * (vars[reg::B14] * vars[reg::inv5] + one())*(vars[reg::B14] + randCoeff[RI(60)] * vars[reg::inv5]);
		tval += randCoeff[RI(61)] * (
			vars[reg::B13 + NUMREGS] +
			results[4]);
		
		return tval;
	}

	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_10(const std::vector<FieldElement> & vars, FieldElement results[]){

		FieldElement tval = vars[reg::FLAG1 + NUMREGS] + one();
		tval += randCoeff[RI(62)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(63)] * (vars[reg::B00 + NUMREGS] + vars[reg::B00]);
		tval += randCoeff[RI(64)] * (vars[reg::B01 + NUMREGS] + vars[reg::B01]);
		tval += randCoeff[RI(65)] * (vars[reg::B02 + NUMREGS] + vars[reg::B02]);
		tval += randCoeff[RI(66)] * (vars[reg::B03 + NUMREGS] + vars[reg::B03]);
		tval += randCoeff[RI(67)] * (vars[reg::B04 + NUMREGS] + vars[reg::B04]);
		tval += randCoeff[RI(68)] * (vars[reg::B10 + NUMREGS] + vars[reg::B10]);
		tval += randCoeff[RI(69)] * (vars[reg::B11 + NUMREGS] + vars[reg::B11]);
		tval += randCoeff[RI(70)] * (vars[reg::B12 + NUMREGS] + vars[reg::B12]);
		tval += randCoeff[RI(71)] * (vars[reg::B13 + NUMREGS] + vars[reg::B13]);
		tval += randCoeff[RI(72)] * (vars[reg::B14 + NUMREGS] + vars[reg::B14]);
		tval += randCoeff[RI(73)] * (vars[reg::B30 + NUMREGS] + vars[reg::B30]);
		tval += randCoeff[RI(74)] * (vars[reg::B31 + NUMREGS] + vars[reg::B31]);
		tval += randCoeff[RI(75)] * (vars[reg::B32 + NUMREGS] + vars[reg::B32]);
		tval += randCoeff[RI(76)] * (vars[reg::B33 + NUMREGS] + vars[reg::B33]);
		tval += randCoeff[RI(77)] * (vars[reg::B34 + NUMREGS] + vars[reg::B34]);


		tval += randCoeff[RI(78)] * (vars[reg::B20] * vars[reg::inv1] + one())*(vars[reg::B20] + randCoeff[RI(79)] * vars[reg::inv1]);
		tval += randCoeff[RI(80)] * (
			vars[reg::B23 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(81)] * (vars[reg::B21] * vars[reg::inv2] + one())*(vars[reg::B21] + randCoeff[RI(82)] * vars[reg::inv2]);
		tval += randCoeff[RI(83)] * (
			vars[reg::B24 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(84)] * (vars[reg::B22] * vars[reg::inv3] + one())*(vars[reg::B22] + randCoeff[RI(85)] * vars[reg::inv3]);
		tval += randCoeff[RI(86)] * (
			vars[reg::B20 + NUMREGS] +
			results[2]);

		tval += randCoeff[RI(87)] * (vars[reg::B23] * vars[reg::inv4] + one())*(vars[reg::B23] + randCoeff[RI(88)] * vars[reg::inv4]);
		tval += randCoeff[RI(89)] * (
			vars[reg::B21 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(90)] * (vars[reg::B24] * vars[reg::inv5] + one())*(vars[reg::B24] + randCoeff[RI(91)] * vars[reg::inv5]);
		tval += randCoeff[RI(92)] * (
			vars[reg::B22 + NUMREGS] +
			results[4]);
		return tval;

	}


	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_11(const std::vector<FieldElement> & vars, FieldElement results[]){

		FieldElement tval = vars[reg::FLAG1 + NUMREGS] + xFE();
		tval += randCoeff[RI(93)] * (vars[reg::B00 + NUMREGS] + vars[reg::B00]);
		tval += randCoeff[RI(94)] * (vars[reg::B01 + NUMREGS] + vars[reg::B01]);
		tval += randCoeff[RI(95)] * (vars[reg::B02 + NUMREGS] + vars[reg::B02]);
		tval += randCoeff[RI(96)] * (vars[reg::B03 + NUMREGS] + vars[reg::B03]);
		tval += randCoeff[RI(97)] * (vars[reg::B04 + NUMREGS] + vars[reg::B04]);
		tval += randCoeff[RI(98)] * (vars[reg::B10 + NUMREGS] + vars[reg::B10]);
		tval += randCoeff[RI(99)] * (vars[reg::B11 + NUMREGS] + vars[reg::B11]);
		tval += randCoeff[RI(100)] * (vars[reg::B12 + NUMREGS] + vars[reg::B12]);
		tval += randCoeff[RI(101)] * (vars[reg::B13 + NUMREGS] + vars[reg::B13]);
		tval += randCoeff[RI(102)] * (vars[reg::B14 + NUMREGS] + vars[reg::B14]);
		tval += randCoeff[RI(103)] * (vars[reg::B20 + NUMREGS] + vars[reg::B20]);
		tval += randCoeff[RI(104)] * (vars[reg::B21 + NUMREGS] + vars[reg::B21]);
		tval += randCoeff[RI(105)] * (vars[reg::B22 + NUMREGS] + vars[reg::B22]);
		tval += randCoeff[RI(106)] * (vars[reg::B23 + NUMREGS] + vars[reg::B23]);
		tval += randCoeff[RI(107)] * (vars[reg::B24 + NUMREGS] + vars[reg::B24]);


		tval += randCoeff[RI(108)] * (vars[reg::B30] * vars[reg::inv1] + one())*(vars[reg::B30] + randCoeff[RI(109)] * vars[reg::inv1]);
		tval += randCoeff[RI(110)] * (
			vars[reg::B32 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(111)] * (vars[reg::B31] * vars[reg::inv2] + one())*(vars[reg::B31] + randCoeff[RI(112)] * vars[reg::inv2]);
		tval += randCoeff[RI(113)] * (
			vars[reg::B33 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(114)] * (vars[reg::B32] * vars[reg::inv3] + one())*(vars[reg::B32] + randCoeff[RI(115)] * vars[reg::inv3]);
		tval += randCoeff[RI(116)] * (
			vars[reg::B34 + NUMREGS] +
			results[2]);

		tval += randCoeff[RI(117)] * (vars[reg::B33] * vars[reg::inv4] + one())*(vars[reg::B33] + randCoeff[RI(118)] * vars[reg::inv4]);
		tval += randCoeff[RI(119)] * (
			vars[reg::B30 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(120)] * (vars[reg::B34] * vars[reg::inv5] + one())*(vars[reg::B34] + randCoeff[RI(121)] * vars[reg::inv5]);
		tval += randCoeff[RI(122)] * (
			vars[reg::B31 + NUMREGS] +
			results[4]);

		return tval;



	}
	inline FieldElement evalCPolyAES160_SB_And_SR(const std::vector<FieldElement> & vars, FieldElement results[], FieldElement PolyComputationInversePowers){
		FieldElement tval = vars[reg::STATE + NUMREGS] + one();
		tval += randCoeff[RI(123)] * (vars[reg::RC + NUMREGS] + vars[reg::RC]);
		tval += randCoeff[RI(124)] * (vars[reg::K00 + NUMREGS] + vars[reg::K00]);
		tval += randCoeff[RI(125)] * (vars[reg::K01 + NUMREGS] + vars[reg::K01]);
		tval += randCoeff[RI(126)] * (vars[reg::K02 + NUMREGS] + vars[reg::K02]);
		tval += randCoeff[RI(127)] * (vars[reg::K03 + NUMREGS] + vars[reg::K03]);
		tval += randCoeff[RI(128)] * (vars[reg::K04 + NUMREGS] + vars[reg::K04]);
		tval += randCoeff[RI(129)] * (vars[reg::K10 + NUMREGS] + vars[reg::K10]);
		tval += randCoeff[RI(130)] * (vars[reg::K11 + NUMREGS] + vars[reg::K11]);
		tval += randCoeff[RI(131)] * (vars[reg::K12 + NUMREGS] + vars[reg::K12]);
		tval += randCoeff[RI(132)] * (vars[reg::K13 + NUMREGS] + vars[reg::K13]);
		tval += randCoeff[RI(133)] * (vars[reg::K14 + NUMREGS] + vars[reg::K14]);
		tval += randCoeff[RI(134)] * (vars[reg::K20 + NUMREGS] + vars[reg::K20]);
		tval += randCoeff[RI(135)] * (vars[reg::K21 + NUMREGS] + vars[reg::K21]);
		tval += randCoeff[RI(136)] * (vars[reg::K22 + NUMREGS] + vars[reg::K22]);
		tval += randCoeff[RI(137)] * (vars[reg::K23 + NUMREGS] + vars[reg::K23]);
		tval += randCoeff[RI(138)] * (vars[reg::K24 + NUMREGS] + vars[reg::K24]);
		tval += randCoeff[RI(139)] * (vars[reg::K30 + NUMREGS] + vars[reg::K30]);
		tval += randCoeff[RI(140)] * (vars[reg::K31 + NUMREGS] + vars[reg::K31]);
		tval += randCoeff[RI(141)] * (vars[reg::K32 + NUMREGS] + vars[reg::K32]);
		tval += randCoeff[RI(142)] * (vars[reg::K33 + NUMREGS] + vars[reg::K33]);
		tval += randCoeff[RI(143)] * (vars[reg::K34 + NUMREGS] + vars[reg::K34]);
	
		


		tval += randCoeff[RI(144)] * (vars[reg::FLAG1] + one()) *(vars[reg::FLAG2] + one()) * evalCPolyAES160_SB_And_SR_FLAGS_ARE_00(vars, results);
		tval += randCoeff[RI(145)] * (vars[reg::FLAG1] + one()) *(vars[reg::FLAG2]) * evalCPolyAES160_SB_And_SR_FLAGS_ARE_01(vars, results);
		tval += randCoeff[RI(146)] * (vars[reg::FLAG1]) *(vars[reg::FLAG2] + one()) * evalCPolyAES160_SB_And_SR_FLAGS_ARE_10(vars, results);
		tval += randCoeff[RI(147)] * (vars[reg::FLAG1]) *(vars[reg::FLAG2]) * evalCPolyAES160_SB_And_SR_FLAGS_ARE_11(vars, results);
	
		return tval;


	}

	inline FieldElement evalCPolyLastRound(const std::vector<FieldElement> & vars, FieldElement K_next[][5], FieldElement lastStepConstraints){
		FieldElement tval = lastStepConstraints;

		tval += vars[reg::B00 + NUMREGS] + vars[reg::B00] + K_next[0][0];
		tval += randCoeff[RI(148)] * (vars[reg::B01 + NUMREGS] + vars[reg::B01] + K_next[0][1]);
		tval += randCoeff[RI(149)] * (vars[reg::B02 + NUMREGS] + vars[reg::B02] + K_next[0][2]);
		tval += randCoeff[RI(150)] * (vars[reg::B03 + NUMREGS] + vars[reg::B03] + K_next[0][3]);
		tval += randCoeff[RI(151)] * (vars[reg::B04 + NUMREGS] + vars[reg::B04] + K_next[0][4]);
																		 
		tval += randCoeff[RI(152)] * (vars[reg::B10 + NUMREGS] + vars[reg::B10] + K_next[1][0]);
		tval += randCoeff[RI(153)] * (vars[reg::B11 + NUMREGS] + vars[reg::B11] + K_next[1][1]);
		tval += randCoeff[RI(154)] * (vars[reg::B12 + NUMREGS] + vars[reg::B12] + K_next[1][2]);
		tval += randCoeff[RI(155)] * (vars[reg::B13 + NUMREGS] + vars[reg::B13] + K_next[1][3]);
		tval += randCoeff[RI(156)] * (vars[reg::B14 + NUMREGS] + vars[reg::B14] + K_next[1][4]);
																	
		tval += randCoeff[RI(157)] * (vars[reg::B20 + NUMREGS] + vars[reg::B20] + K_next[2][0]);
		tval += randCoeff[RI(158)] * (vars[reg::B21 + NUMREGS] + vars[reg::B21] + K_next[2][1]);
		tval += randCoeff[RI(159)] * (vars[reg::B22 + NUMREGS] + vars[reg::B22] + K_next[2][2]);
		tval += randCoeff[RI(160)] * (vars[reg::B23 + NUMREGS] + vars[reg::B23] + K_next[2][3]);
		tval += randCoeff[RI(161)] * (vars[reg::B24 + NUMREGS] + vars[reg::B24] + K_next[2][4]);
																		
		tval += randCoeff[RI(162)] * (vars[reg::B30 + NUMREGS] + vars[reg::B30] + K_next[3][0]);
		tval += randCoeff[RI(163)] * (vars[reg::B31 + NUMREGS] + vars[reg::B31] + K_next[3][1]);
		tval += randCoeff[RI(164)] * (vars[reg::B32 + NUMREGS] + vars[reg::B32] + K_next[3][2]);
		tval += randCoeff[RI(165)] * (vars[reg::B33 + NUMREGS] + vars[reg::B33] + K_next[3][3]);
		tval += randCoeff[RI(166)] * (vars[reg::B34 + NUMREGS] + vars[reg::B34] + K_next[3][4]);

		return tval;

	}
	inline FieldElement evalCPolyNonLastRound(const std::vector<FieldElement> & vars, FieldElement K_next[][5]){
		FieldElement tval = vars[reg::RC + NUMREGS] + consts::xFETransformed*vars[reg::RC];
		tval += randCoeff[RI(167)] * (vars[reg::STATE + NUMREGS] + one());


		tval += randCoeff[RI(168)] * (vars[reg::K00 + NUMREGS] + K_next[0][0]);
		tval += randCoeff[RI(169)] * (vars[reg::K01 + NUMREGS] + vars[reg::K00 + NUMREGS] + vars[reg::K01]);
		tval += randCoeff[RI(170)] * (vars[reg::K02 + NUMREGS] + vars[reg::K01 + NUMREGS] + vars[reg::K02]);
		tval += randCoeff[RI(171)] * (vars[reg::K03 + NUMREGS] + vars[reg::K02 + NUMREGS] + vars[reg::K03]);
		tval += randCoeff[RI(172)] * (vars[reg::K04 + NUMREGS] + vars[reg::K03 + NUMREGS] + vars[reg::K04]);
															   
		tval += randCoeff[RI(173)] * (vars[reg::K10 + NUMREGS] + K_next[1][0]);
		tval += randCoeff[RI(174)] * (vars[reg::K11 + NUMREGS] + vars[reg::K10 + NUMREGS] + vars[reg::K11]);
		tval += randCoeff[RI(175)] * (vars[reg::K12 + NUMREGS] + vars[reg::K11 + NUMREGS] + vars[reg::K12]);
		tval += randCoeff[RI(176)] * (vars[reg::K13 + NUMREGS] + vars[reg::K12 + NUMREGS] + vars[reg::K13]);
		tval += randCoeff[RI(177)] * (vars[reg::K14 + NUMREGS] + vars[reg::K13 + NUMREGS] + vars[reg::K14]);
																   
		tval += randCoeff[RI(178)] * (vars[reg::K20 + NUMREGS] + K_next[2][0]);
		tval += randCoeff[RI(179)] * (vars[reg::K21 + NUMREGS] + vars[reg::K20 + NUMREGS] + vars[reg::K21]);
		tval += randCoeff[RI(180)] * (vars[reg::K22 + NUMREGS] + vars[reg::K21 + NUMREGS] + vars[reg::K22]);
		tval += randCoeff[RI(181)] * (vars[reg::K23 + NUMREGS] + vars[reg::K22 + NUMREGS] + vars[reg::K23]);
		tval += randCoeff[RI(182)] * (vars[reg::K24 + NUMREGS] + vars[reg::K23 + NUMREGS] + vars[reg::K24]);
																   
		tval += randCoeff[RI(183)] * (vars[reg::K30 + NUMREGS] + K_next[3][0]);
		tval += randCoeff[RI(184)] * (vars[reg::K31 + NUMREGS] + vars[reg::K30 + NUMREGS] + vars[reg::K31]);
		tval += randCoeff[RI(185)] * (vars[reg::K32 + NUMREGS] + vars[reg::K31 + NUMREGS] + vars[reg::K32]);
		tval += randCoeff[RI(186)] * (vars[reg::K33 + NUMREGS] + vars[reg::K32 + NUMREGS] + vars[reg::K33]);
		tval += randCoeff[RI(187)] * (vars[reg::K34 + NUMREGS] + vars[reg::K33 + NUMREGS] + vars[reg::K34]);
		
		tval += randCoeff[RI(188)] * (vars[reg::B00 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::B00] + consts::MixColMat[0][1] * vars[reg::B10]
			+ consts::MixColMat[0][2] * vars[reg::B20] + consts::MixColMat[0][3] * vars[reg::B30] + vars[reg::K00 + NUMREGS]);
		
		
		tval += randCoeff[RI(189)] * (vars[reg::B01 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::B01] + consts::MixColMat[0][1] * vars[reg::B11]
			+ consts::MixColMat[0][2] * vars[reg::B21] + consts::MixColMat[0][3] * vars[reg::B31] + vars[reg::K01 + NUMREGS]);
		
		tval += randCoeff[RI(190)] * (vars[reg::B02 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::B02] + consts::MixColMat[0][1] * vars[reg::B12]
			+ consts::MixColMat[0][2] * vars[reg::B22] + consts::MixColMat[0][3] * vars[reg::B32] + vars[reg::K02 + NUMREGS]);
		
		tval += randCoeff[RI(191)] * (vars[reg::B03 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::B03] + consts::MixColMat[0][1] * vars[reg::B13]
			+ consts::MixColMat[0][2] * vars[reg::B23] + consts::MixColMat[0][3] * vars[reg::B33] + vars[reg::K03 + NUMREGS]);
		
		tval += randCoeff[RI(192)] * (vars[reg::B04 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::B04] + consts::MixColMat[0][1] * vars[reg::B14]
			+ consts::MixColMat[0][2] * vars[reg::B24] + consts::MixColMat[0][3] * vars[reg::B34] + vars[reg::K04 + NUMREGS]);
		


		tval += randCoeff[RI(193)] * (vars[reg::B10 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::B00] + consts::MixColMat[1][1] * vars[reg::B10]
			+ consts::MixColMat[1][2] * vars[reg::B20] + consts::MixColMat[1][3] * vars[reg::B30] + vars[reg::K10 + NUMREGS]);
		
		tval += randCoeff[RI(194)] * (vars[reg::B11 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::B01] + consts::MixColMat[1][1] * vars[reg::B11]
			+ consts::MixColMat[1][2] * vars[reg::B21] + consts::MixColMat[1][3] * vars[reg::B31] + vars[reg::K11 + NUMREGS]);
		
		tval += randCoeff[RI(195)] * (vars[reg::B12 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::B02] + consts::MixColMat[1][1] * vars[reg::B12]
			+ consts::MixColMat[1][2] * vars[reg::B22] + consts::MixColMat[1][3] * vars[reg::B32] + vars[reg::K12 + NUMREGS]);
		
		tval += randCoeff[RI(196)] * (vars[reg::B13 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::B03] + consts::MixColMat[1][1] * vars[reg::B13]
			+ consts::MixColMat[1][2] * vars[reg::B23] + consts::MixColMat[1][3] * vars[reg::B33] + vars[reg::K13 + NUMREGS]);
		
		tval += randCoeff[RI(197)] * (vars[reg::B14 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::B04] + consts::MixColMat[1][1] * vars[reg::B14]
			+ consts::MixColMat[1][2] * vars[reg::B24] + consts::MixColMat[1][3] * vars[reg::B34] + vars[reg::K14 + NUMREGS]);
		
		
		
		tval += randCoeff[RI(198)] * (vars[reg::B20 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::B00] + consts::MixColMat[2][1] * vars[reg::B10]
			+ consts::MixColMat[2][2] * vars[reg::B20] + consts::MixColMat[2][3] * vars[reg::B30] + vars[reg::K20 + NUMREGS]);
		
		tval += randCoeff[RI(199)] * (vars[reg::B21 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::B01] + consts::MixColMat[2][1] * vars[reg::B11]
			+ consts::MixColMat[2][2] * vars[reg::B21] + consts::MixColMat[2][3] * vars[reg::B31] + vars[reg::K21 + NUMREGS]);
		
		tval += randCoeff[RI(200)] * (vars[reg::B22 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::B02] + consts::MixColMat[2][1] * vars[reg::B12]
			+ consts::MixColMat[2][2] * vars[reg::B22] + consts::MixColMat[2][3] * vars[reg::B32] + vars[reg::K22 + NUMREGS]);
		
		tval += randCoeff[RI(201)] * (vars[reg::B23 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::B03] + consts::MixColMat[2][1] * vars[reg::B13]
			+ consts::MixColMat[2][2] * vars[reg::B23] + consts::MixColMat[2][3] * vars[reg::B33] + vars[reg::K23 + NUMREGS]);
		
		tval += randCoeff[RI(202)] * (vars[reg::B24 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::B04] + consts::MixColMat[2][1] * vars[reg::B14]
			+ consts::MixColMat[2][2] * vars[reg::B24] + consts::MixColMat[2][3] * vars[reg::B34] + vars[reg::K24 + NUMREGS]);
		
		
		
		tval += randCoeff[RI(203)] * (vars[reg::B30 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::B00] + consts::MixColMat[3][1] * vars[reg::B10]
			+ consts::MixColMat[3][2] * vars[reg::B20] + consts::MixColMat[3][3] * vars[reg::B30] + vars[reg::K30 + NUMREGS]);
		
		tval += randCoeff[RI(204)] * (vars[reg::B31 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::B01] + consts::MixColMat[3][1] * vars[reg::B11]
			+ consts::MixColMat[3][2] * vars[reg::B21] + consts::MixColMat[3][3] * vars[reg::B31] + vars[reg::K31 + NUMREGS]);
		
		tval += randCoeff[RI(205)] * (vars[reg::B32 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::B02] + consts::MixColMat[3][1] * vars[reg::B12]
			+ consts::MixColMat[3][2] * vars[reg::B22] + consts::MixColMat[3][3] * vars[reg::B32] + vars[reg::K32 + NUMREGS]);
		
		tval += randCoeff[RI(206)] * (vars[reg::B33 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::B03] + consts::MixColMat[3][1] * vars[reg::B13]
			+ consts::MixColMat[3][2] * vars[reg::B23] + consts::MixColMat[3][3] * vars[reg::B33] + vars[reg::K33 + NUMREGS]);
		
		tval += randCoeff[RI(207)] * (vars[reg::B34 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::B04] + consts::MixColMat[3][1] * vars[reg::B14]
			+ consts::MixColMat[3][2] * vars[reg::B24] + consts::MixColMat[3][3] * vars[reg::B34] + vars[reg::K34 + NUMREGS]);
		
		return tval;
	}

	inline FieldElement evalCPolyAES160_MC_And_ARK(const std::vector<FieldElement> & vars, FieldElement results[],
		FieldElement PolyComputationInversePowers, FieldElement lastStepConstraints){
		FieldElement tval = zero();

		tval += vars[reg::FLAG1 + NUMREGS];
		tval += randCoeff[RI(208)] * (vars[reg::FLAG2 + NUMREGS]);
		tval += randCoeff[RI(209)] * (vars[reg::K14] * vars[reg::inv1] + one())*(vars[reg::K14] + randCoeff[RI(210)] * vars[reg::inv1]);
		tval += randCoeff[RI(211)] * (vars[reg::K24] * vars[reg::inv2] + one())*(vars[reg::K24] + randCoeff[RI(212)] * vars[reg::inv2]);
		tval += randCoeff[RI(213)] * (vars[reg::K34] * vars[reg::inv3] + one())*(vars[reg::K34] + randCoeff[RI(214)] * vars[reg::inv3]);
		tval += randCoeff[RI(215)] * (vars[reg::K04] * vars[reg::inv4] + one())*(vars[reg::K04] + randCoeff[RI(216)] * vars[reg::inv4]);

		FieldElement K_next[4][5];

		K_next[0][0] = results[0] + vars[reg::K00] + vars[reg::RC];
		K_next[0][1] = K_next[0][0] + vars[reg::K01];
		K_next[0][2] = K_next[0][1] + vars[reg::K02];
		K_next[0][3] = K_next[0][2] + vars[reg::K03];
		K_next[0][4] = K_next[0][3] + vars[reg::K04];

		K_next[1][0] = results[1] + vars[reg::K10];
		K_next[1][1] = K_next[1][0] + vars[reg::K11];
		K_next[1][2] = K_next[1][1] + vars[reg::K12];
		K_next[1][3] = K_next[1][2] + vars[reg::K13];
		K_next[1][4] = K_next[1][3] + vars[reg::K14];

		K_next[2][0] = results[2] + vars[reg::K20];
		K_next[2][1] = K_next[2][0] + vars[reg::K21];
		K_next[2][2] = K_next[2][1] + vars[reg::K22];
		K_next[2][3] = K_next[2][2] + vars[reg::K23];
		K_next[2][4] = K_next[2][3] + vars[reg::K24];

		K_next[3][0] = results[3] + vars[reg::K30];
		K_next[3][1] = K_next[3][0] + vars[reg::K31];
		K_next[3][2] = K_next[3][1] + vars[reg::K32];
		K_next[3][3] = K_next[3][2] + vars[reg::K33];
		K_next[3][4] = K_next[3][3] + vars[reg::K34];

		tval += randCoeff[RI(217)] * (vars[reg::RC] + consts::Rcon_round11)*evalCPolyNonLastRound(vars, K_next);
		tval += randCoeff[RI(218)] * (one() + (vars[reg::RC] + consts::Rcon_round11)*vars[reg::invRC])*evalCPolyLastRound(vars, K_next, lastStepConstraints);
		return tval;

	}
	inline FieldElement evalCPolyAES160_LoopBody(const std::vector<FieldElement> & vars, FieldElement lastStepConstraints){

		FieldElement results[5];

		results[0] =
			vars[reg::inv1] * consts::polyFromMatrix[0] +
			vars[reg::inv1] * vars[reg::inv1] * consts::polyFromMatrix[1] +
			vars[reg::W11] * consts::polyFromMatrix[2] +
			vars[reg::W11] * vars[reg::W11] * consts::polyFromMatrix[3] +
			vars[reg::W12] * consts::polyFromMatrix[4] +
			vars[reg::W12] * vars[reg::W12] * consts::polyFromMatrix[5] +
			vars[reg::W13] * consts::polyFromMatrix[6] +
			vars[reg::W13] * vars[reg::W13] * consts::polyFromMatrix[7] +
			consts::B_Transformed;
		;
		results[1] =
			vars[reg::inv2] * consts::polyFromMatrix[0] +
			vars[reg::inv2] * vars[reg::inv2] * consts::polyFromMatrix[1] +
			vars[reg::W21] * consts::polyFromMatrix[2] +
			vars[reg::W21] * vars[reg::W21] * consts::polyFromMatrix[3] +
			vars[reg::W22] * consts::polyFromMatrix[4] +
			vars[reg::W22] * vars[reg::W22] * consts::polyFromMatrix[5] +
			vars[reg::W23] * consts::polyFromMatrix[6] +
			vars[reg::W23] * vars[reg::W23] * consts::polyFromMatrix[7] +
			consts::B_Transformed;
		;
		results[2] =
			vars[reg::inv3] * consts::polyFromMatrix[0] +
			vars[reg::inv3] * vars[reg::inv3] * consts::polyFromMatrix[1] +
			vars[reg::W31] * consts::polyFromMatrix[2] +
			vars[reg::W31] * vars[reg::W31] * consts::polyFromMatrix[3] +
			vars[reg::W32] * consts::polyFromMatrix[4] +
			vars[reg::W32] * vars[reg::W32] * consts::polyFromMatrix[5] +
			vars[reg::W33] * consts::polyFromMatrix[6] +
			vars[reg::W33] * vars[reg::W33] * consts::polyFromMatrix[7] +
			consts::B_Transformed;
		;
		results[3] =
			vars[reg::inv4] * consts::polyFromMatrix[0] +
			vars[reg::inv4] * vars[reg::inv4] * consts::polyFromMatrix[1] +
			vars[reg::W41] * consts::polyFromMatrix[2] +
			vars[reg::W41] * vars[reg::W41] * consts::polyFromMatrix[3] +
			vars[reg::W42] * consts::polyFromMatrix[4] +
			vars[reg::W42] * vars[reg::W42] * consts::polyFromMatrix[5] +
			vars[reg::W43] * consts::polyFromMatrix[6] +
			vars[reg::W43] * vars[reg::W43] * consts::polyFromMatrix[7] +
			consts::B_Transformed;
		;
		results[4] =
			vars[reg::inv5] * consts::polyFromMatrix[0] +
			vars[reg::inv5] * vars[reg::inv5] * consts::polyFromMatrix[1] +
			vars[reg::W51] * consts::polyFromMatrix[2] +
			vars[reg::W51] * vars[reg::W51] * consts::polyFromMatrix[3] +
			vars[reg::W52] * consts::polyFromMatrix[4] +
			vars[reg::W52] * vars[reg::W52] * consts::polyFromMatrix[5] +
			vars[reg::W53] * consts::polyFromMatrix[6] +
			vars[reg::W53] * vars[reg::W53] * consts::polyFromMatrix[7] +
			consts::B_Transformed;
		;

		FieldElement PolyComputationInversePowers = vars[reg::W11] + vars[reg::inv1] * vars[reg::inv1] * vars[reg::inv1] * vars[reg::inv1];
		PolyComputationInversePowers += randCoeff[RI(219)] * (vars[reg::W12] + vars[reg::W11] * vars[reg::W11] * vars[reg::W11] * vars[reg::W11]);
		PolyComputationInversePowers += randCoeff[RI(220)] * (vars[reg::W13] + vars[reg::W12] * vars[reg::W12] * vars[reg::W12] * vars[reg::W12]);

		PolyComputationInversePowers += randCoeff[RI(221)] * (vars[reg::W21] + vars[reg::inv2] * vars[reg::inv2] * vars[reg::inv2] * vars[reg::inv2]);
		PolyComputationInversePowers += randCoeff[RI(222)] * (vars[reg::W22] + vars[reg::W21] * vars[reg::W21] * vars[reg::W21] * vars[reg::W21]);
		PolyComputationInversePowers += randCoeff[RI(223)] * (vars[reg::W23] + vars[reg::W22] * vars[reg::W22] * vars[reg::W22] * vars[reg::W22]);

		PolyComputationInversePowers += randCoeff[RI(224)] * (vars[reg::W31] + vars[reg::inv3] * vars[reg::inv3] * vars[reg::inv3] * vars[reg::inv3]);
		PolyComputationInversePowers += randCoeff[RI(225)] * (vars[reg::W32] + vars[reg::W31] * vars[reg::W31] * vars[reg::W31] * vars[reg::W31]);
		PolyComputationInversePowers += randCoeff[RI(226)] * (vars[reg::W33] + vars[reg::W32] * vars[reg::W32] * vars[reg::W32] * vars[reg::W32]);

		PolyComputationInversePowers += randCoeff[RI(227)] * (vars[reg::W41] + vars[reg::inv4] * vars[reg::inv4] * vars[reg::inv4] * vars[reg::inv4]);
		PolyComputationInversePowers += randCoeff[RI(228)] * (vars[reg::W42] + vars[reg::W41] * vars[reg::W41] * vars[reg::W41] * vars[reg::W41]);
		PolyComputationInversePowers += randCoeff[RI(229)] * (vars[reg::W43] + vars[reg::W42] * vars[reg::W42] * vars[reg::W42] * vars[reg::W42]);

		PolyComputationInversePowers += randCoeff[RI(230)] * (vars[reg::W51] + vars[reg::inv5] * vars[reg::inv5] * vars[reg::inv5] * vars[reg::inv5]);
		PolyComputationInversePowers += randCoeff[RI(231)] * (vars[reg::W52] + vars[reg::W51] * vars[reg::W51] * vars[reg::W51] * vars[reg::W51]);
		PolyComputationInversePowers += randCoeff[RI(232)] * (vars[reg::W53] + vars[reg::W52] * vars[reg::W52] * vars[reg::W52] * vars[reg::W52]);


		FieldElement tval = randCoeff[RI(233)] * PolyComputationInversePowers;

		tval += randCoeff[RI(234)] * (vars[reg::FLAG1] + xFE())*evalCPolyAES160_SB_And_SR(vars, results, PolyComputationInversePowers);
		tval += randCoeff[RI(235)] * vars[reg::FLAG1] * (vars[reg::FLAG1] + one())*evalCPolyAES160_MC_And_ARK(vars, results, PolyComputationInversePowers, lastStepConstraints);
		return tval;
	}

	inline FieldElement evalCPolyST2isXX(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::PHASE + NUMREGS] + one();
		tval += randCoeff[RI(236)] * (vars[reg::STATE + NUMREGS] + one());
		tval += randCoeff[RI(237)] * (vars[reg::FLAG1 + NUMREGS]);
		tval += randCoeff[RI(238)] * (vars[reg::FLAG2 + NUMREGS]);

		FieldElement gval = vars[reg::isSecondPhaseComparingLOCI + NUMREGS];
		gval += randCoeff[RI(244)] * vars[reg::PartialMATCH + NUMREGS] * (vars[reg::PartialMATCH + NUMREGS] + one()) * (vars[reg::MATCH + NUMREGS] + xFE());
		gval += randCoeff[RI(245)] * vars[reg::MATCH] * (vars[reg::MATCH] + one()) * (vars[reg::MATCH + NUMREGS] + xFE());
		gval += randCoeff[RI(246)] * vars[reg::PartialMATCH + NUMREGS] * (vars[reg::PartialMATCH + NUMREGS] + xFE()) * (vars[reg::MATCH] + xFE()) * (vars[reg::MATCH + NUMREGS] + one());
		gval += randCoeff[RI(247)] * (vars[reg::PartialMATCH + NUMREGS] + one())* (vars[reg::PartialMATCH + NUMREGS] + xFE()) * (vars[reg::MATCH] + one()) * (vars[reg::MATCH] + xFE()) * vars[reg::MATCH + NUMREGS];

		gval *= vars[reg::isSecondPhaseComparingLOCI];

		gval += (vars[reg::isSecondPhaseComparingLOCI] + one()) *
			((vars[reg::isSecondPhaseComparingLOCI + NUMREGS] + one()) + randCoeff[RI(242)] * (vars[reg::MATCH + NUMREGS] + vars[reg::MATCH]) + randCoeff[RI(243)] * (vars[reg::PartialMATCH + NUMREGS] + vars[reg::PartialMATCH]));

		tval += randCoeff[RI(248)] * gval;

		for (int i = 0; i < 20; i++){
			tval += randCoeff[RI(249 + i)] * (vars[reg::B00 + i + NUMREGS] + vars[reg::B00 + i]);
		}

		tval += randCoeff[RI(269)] * (vars[reg::A + NUMREGS] + vars[reg::B00] +
			xFE()*(vars[reg::B10] +
			xFE()*(vars[reg::B20] +
			xFE()*(vars[reg::B30] +
			xFE()*(vars[reg::B01] +
			xFE()*(vars[reg::B11] +
			xFE()*(vars[reg::B21] +
			xFE()*(vars[reg::B31]))))))));
		
		tval += randCoeff[RI(270)] * (vars[reg::B + NUMREGS] + vars[reg::B02] +
			xFE()*(vars[reg::B12] +
			xFE()*(vars[reg::B22] +
			xFE()*(vars[reg::B32] +
			xFE()*(vars[reg::B03] +
			xFE()*(vars[reg::B13] +
			xFE()*(vars[reg::B23] +
			xFE()*(vars[reg::B33]))))))));
		
		tval += randCoeff[RI(271)] * (vars[reg::C + NUMREGS] + vars[reg::B04] +
			xFE()*(vars[reg::B14] +
			xFE()*(vars[reg::B24] +
			xFE()*(vars[reg::B34]))));

		gval = vars[reg::L1] + vars[reg::A + NUMREGS];
		gval += randCoeff[RI(272)] * (vars[reg::L2] + vars[reg::B + NUMREGS]);
		gval += randCoeff[RI(273)] * (vars[reg::L3] + vars[reg::C + NUMREGS]);
		gval += randCoeff[RI(274)] * (vars[reg::L4] + vars[reg::K00] +
			xFE()*(vars[reg::K10] +
			xFE()*(vars[reg::K20] +
			xFE()*(vars[reg::K30] +
			xFE()*(vars[reg::K01] +
			xFE()*(vars[reg::K11] +
			xFE()*(vars[reg::K21] +
			xFE()*(vars[reg::K31]))))))));
		gval += randCoeff[RI(275)] * (vars[reg::L5] + vars[reg::K02] +
			xFE()*(vars[reg::K12] +
			xFE()*(vars[reg::K22] +
			xFE()*(vars[reg::K32] +
			xFE()*(vars[reg::K03] +
			xFE()*(vars[reg::K13] +
			xFE()*(vars[reg::K23] +
			xFE()*(vars[reg::K33]))))))));
		gval += randCoeff[RI(276)] * (vars[reg::L6] + vars[reg::K04] +
			xFE()*(vars[reg::K14] +
			xFE()*(vars[reg::K24] +
			xFE()*(vars[reg::K34]))));

		gval *= ((vars[reg::K] + one())*vars[reg::STATE] + one());

		tval += randCoeff[RI(277)] * gval;
		return vars[reg::ST3] * (vars[reg::ST2] + one() + xFE()) * tval;
	}

	inline FieldElement evalCPolyST2is1(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::PHASE + NUMREGS];
		tval += randCoeff[RI(278)] * (vars[reg::isSecondPhaseComparingLOCI + NUMREGS] + vars[reg::isSecondPhaseComparingLOCI]);
		tval += randCoeff[RI(279)] * (vars[reg::ST2 + NUMREGS] + xFE());

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 5; ++j)
				tval += randCoeff[RI(280 + 5*i + j)] * (vars[reg::B00 + j + 5 * i] + power(vars[reg::B00 + j + 5 * i + NUMREGS], 4));
		tval += randCoeff[RI(300)] * (vars[reg::W11] + power(vars[reg::B00], 4)) + randCoeff[RI(301)] * (vars[reg::W12] + power(vars[reg::W11], 4));
		tval += randCoeff[RI(302)] * (vars[reg::B00 + NUMREGS] + power(vars[reg::W12], 4));
		tval += randCoeff[RI(303)] * (vars[reg::W13] + power(vars[reg::B01], 4)) + randCoeff[RI(304)] * (vars[reg::W21] + power(vars[reg::W13], 4));
		tval += randCoeff[RI(305)] * (vars[reg::B01 + NUMREGS] + power(vars[reg::W21], 4));
		tval += randCoeff[RI(306)] * (vars[reg::W22] + power(vars[reg::B10], 4)) + randCoeff[RI(307)] * (vars[reg::W23] + power(vars[reg::W22], 4));
		tval += randCoeff[RI(308)] * (vars[reg::B10 + NUMREGS] + power(vars[reg::W23], 4));
		tval += randCoeff[RI(309)] * (vars[reg::W31] + power(vars[reg::B11], 4)) + randCoeff[RI(310)] * (vars[reg::W32] + power(vars[reg::W31], 4));
		tval += randCoeff[RI(311)] * (vars[reg::B11 + NUMREGS] + power(vars[reg::W32], 4));
		tval += randCoeff[RI(312)] * (vars[reg::W33] + power(vars[reg::B20], 4)) + randCoeff[RI(313)] * (vars[reg::W41] + power(vars[reg::W33], 4));
		tval += randCoeff[RI(314)] * (vars[reg::B20 + NUMREGS] + power(vars[reg::W41], 4));
		tval += randCoeff[RI(315)] * (vars[reg::W42] + power(vars[reg::B21], 4)) + randCoeff[RI(316)] * (vars[reg::W43] + power(vars[reg::W42], 4));
		tval += randCoeff[RI(317)] * (vars[reg::B21 + NUMREGS] + power(vars[reg::W43], 4));
		tval += randCoeff[RI(318)] * (vars[reg::W51] + power(vars[reg::B30], 4)) + randCoeff[RI(319)] * (vars[reg::W52] + power(vars[reg::W51], 4));
		tval += randCoeff[RI(320)] * (vars[reg::B30 + NUMREGS] + power(vars[reg::W52], 4));
		tval += randCoeff[RI(321)] * (vars[reg::W53] + power(vars[reg::B31], 4)) + randCoeff[RI(322)] * (vars[reg::W11 + NUMREGS] + power(vars[reg::W53], 4));
		tval += randCoeff[RI(323)] * (vars[reg::B31 + NUMREGS] + power(vars[reg::W11 + NUMREGS], 4));
		tval += randCoeff[RI(324)] * (vars[reg::W12 + NUMREGS] + power(vars[reg::B02], 4)) + randCoeff[RI(325)] * (vars[reg::W13 + NUMREGS] + power(vars[reg::W12 + NUMREGS], 4));
		tval += randCoeff[RI(326)] * (vars[reg::B02 + NUMREGS] + power(vars[reg::W13 + NUMREGS], 4));
		tval += randCoeff[RI(327)] * (vars[reg::W21 + NUMREGS] + power(vars[reg::B03], 4)) + randCoeff[RI(328)] * (vars[reg::W22 + NUMREGS] + power(vars[reg::W21 + NUMREGS], 4));
		tval += randCoeff[RI(329)] * (vars[reg::B03 + NUMREGS] + power(vars[reg::W22 + NUMREGS], 4));
		tval += randCoeff[RI(330)] * (vars[reg::W23 + NUMREGS] + power(vars[reg::B12], 4)) + randCoeff[RI(331)] * (vars[reg::W31 + NUMREGS] + power(vars[reg::W23 + NUMREGS], 4));
		tval += randCoeff[RI(332)] * (vars[reg::B12 + NUMREGS] + power(vars[reg::W31 + NUMREGS], 4));
		tval += randCoeff[RI(333)] * (vars[reg::W32 + NUMREGS] + power(vars[reg::B13], 4)) + randCoeff[RI(334)] * (vars[reg::W33 + NUMREGS] + power(vars[reg::W32 + NUMREGS], 4));
		tval += randCoeff[RI(335)] * (vars[reg::B13 + NUMREGS] + power(vars[reg::W33 + NUMREGS], 4));
		tval += randCoeff[RI(336)] * (vars[reg::W41 + NUMREGS] + power(vars[reg::B22], 4)) + randCoeff[RI(337)] * (vars[reg::W42 + NUMREGS] + power(vars[reg::W41 + NUMREGS], 4));
		tval += randCoeff[RI(338)] * (vars[reg::B22 + NUMREGS] + power(vars[reg::W42 + NUMREGS], 4));
		tval += randCoeff[RI(339)] * (vars[reg::W43 + NUMREGS] + power(vars[reg::B23], 4)) + randCoeff[RI(340)] * (vars[reg::W51 + NUMREGS] + power(vars[reg::W43 + NUMREGS], 4));
		tval += randCoeff[RI(341)] * (vars[reg::B23 + NUMREGS] + power(vars[reg::W51 + NUMREGS], 4));
		tval += randCoeff[RI(342)] * (vars[reg::W52 + NUMREGS] + power(vars[reg::B32], 4)) + randCoeff[RI(343)] * (vars[reg::W53 + NUMREGS] + power(vars[reg::W52 + NUMREGS], 4));
		tval += randCoeff[RI(344)] * (vars[reg::B32 + NUMREGS] + power(vars[reg::W53 + NUMREGS], 4));
		tval += randCoeff[RI(345)] * (vars[reg::inv1] + power(vars[reg::B33], 4)) + randCoeff[RI(346)] * (vars[reg::inv2] + power(vars[reg::inv1], 4));
		tval += randCoeff[RI(347)] * (vars[reg::B33 + NUMREGS] + power(vars[reg::inv2], 4));
		tval += randCoeff[RI(348)] * (vars[reg::inv3] + power(vars[reg::B04], 4)) + randCoeff[RI(349)] * (vars[reg::inv4] + power(vars[reg::inv3], 4));
		tval += randCoeff[RI(350)] * (vars[reg::B04 + NUMREGS] + power(vars[reg::inv4], 4));
		tval += randCoeff[RI(351)] * (vars[reg::inv5] + power(vars[reg::B14], 4)) + randCoeff[RI(352)] * (vars[reg::inv1 + NUMREGS] + power(vars[reg::inv5], 4));
		tval += randCoeff[RI(353)] * (vars[reg::B14 + NUMREGS] + power(vars[reg::inv1 + NUMREGS], 4));
		tval += randCoeff[RI(354)] * (vars[reg::inv2 + NUMREGS] + power(vars[reg::B24], 4)) + randCoeff[RI(355)] * (vars[reg::inv3 + NUMREGS] + power(vars[reg::inv2 + NUMREGS], 4));
		tval += randCoeff[RI(356)] * (vars[reg::B24 + NUMREGS] + power(vars[reg::inv3 + NUMREGS], 4));
		tval += randCoeff[RI(357)] * (vars[reg::inv4 + NUMREGS] + power(vars[reg::B34], 4)) + randCoeff[RI(358)] * (vars[reg::inv5 + NUMREGS] + power(vars[reg::inv4 + NUMREGS], 4));
		tval += randCoeff[RI(359)] * (vars[reg::B34 + NUMREGS] + power(vars[reg::inv5 + NUMREGS], 4));

		FieldElement gval = (vars[reg::L4] + vars[reg::B00 + NUMREGS] +
			xFE()*(vars[reg::B10 + NUMREGS] +
			xFE()*(vars[reg::B20 + NUMREGS] +
			xFE()*(vars[reg::B30 + NUMREGS] +
			xFE()*(vars[reg::B01 + NUMREGS] +
			xFE()*(vars[reg::B11 + NUMREGS] +
			xFE()*(vars[reg::B21 + NUMREGS] +
			xFE()*(vars[reg::B31 + NUMREGS]))))))));
		gval += randCoeff[RI(360)] * (vars[reg::L5] + vars[reg::B02 + NUMREGS] +
			xFE()*(vars[reg::B12 + NUMREGS] +
			xFE()*(vars[reg::B22 + NUMREGS] +
			xFE()*(vars[reg::B32 + NUMREGS] +
			xFE()*(vars[reg::B03 + NUMREGS] +
			xFE()*(vars[reg::B13 + NUMREGS] +
			xFE()*(vars[reg::B23 + NUMREGS] +
			xFE()*(vars[reg::B33 + NUMREGS]))))))));
		gval += randCoeff[RI(361)] * (vars[reg::L6] + vars[reg::B04 + NUMREGS] +
			xFE()*(vars[reg::B14 + NUMREGS] +
			xFE()*(vars[reg::B24 + NUMREGS] +
			xFE()*(vars[reg::B34 + NUMREGS]))));
		gval *= vars[reg::isSecondPhaseComparingLOCI];
		FieldElement hval = (vars[reg::L1] + vars[reg::B00 + NUMREGS] +
			xFE()*(vars[reg::B10 + NUMREGS] +
			xFE()*(vars[reg::B20 + NUMREGS] +
			xFE()*(vars[reg::B30 + NUMREGS] +
			xFE()*(vars[reg::B01 + NUMREGS] +
			xFE()*(vars[reg::B11 + NUMREGS] +
			xFE()*(vars[reg::B21 + NUMREGS] +
			xFE()*(vars[reg::B31 + NUMREGS]))))))));
		hval += randCoeff[RI(362)] * (vars[reg::L2] + vars[reg::B02 + NUMREGS] +
			xFE()*(vars[reg::B12 + NUMREGS] +
			xFE()*(vars[reg::B22 + NUMREGS] +
			xFE()*(vars[reg::B32 + NUMREGS] +
			xFE()*(vars[reg::B03 + NUMREGS] +
			xFE()*(vars[reg::B13 + NUMREGS] +
			xFE()*(vars[reg::B23 + NUMREGS] +
			xFE()*(vars[reg::B33 + NUMREGS]))))))));
		hval += randCoeff[RI(363)] * (vars[reg::L3] + vars[reg::B04 + NUMREGS] +
			xFE()*(vars[reg::B14 + NUMREGS] +
			xFE()*(vars[reg::B24 + NUMREGS] +
			xFE()*(vars[reg::B34 + NUMREGS]))));
		hval *= (vars[reg::isSecondPhaseComparingLOCI] + one());
		tval += randCoeff[RI(364)] * (gval + hval);
		return (vars[reg::PHASE] + one()) * (vars[reg::ST2] + xFE()) * (vars[reg::ST2] + one() + xFE()) * (vars[reg::ST2] + xFE() * xFE()) * tval;
	}

	inline FieldElement evalCPolyST2isX(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::PHASE + NUMREGS];
		tval += randCoeff[RI(365)] * (vars[reg::isSecondPhaseComparingLOCI + NUMREGS] + vars[reg::isSecondPhaseComparingLOCI]);
		tval += randCoeff[RI(366)] * (vars[reg::ST2 + NUMREGS] + one() + xFE());
		for (int i = 0; i < 20; i++)
			tval += randCoeff[RI(367 + i)] * (vars[reg::B00 + i + NUMREGS] + vars[reg::B00 + i]);
		tval *= (vars[reg::ST2] + one()) * (vars[reg::ST2] + one() + xFE()) * (vars[reg::ST2] + xFE()*xFE());
		return tval;
	}
	
	inline FieldElement evalCPolyST2is1XCompareLOCI(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::PHASE + NUMREGS];
		tval += randCoeff[RI(387)] * (vars[reg::ST2 + NUMREGS] + xFE()*xFE());
		tval += randCoeff[RI(388)] * (vars[reg::isSecondPhaseComparingLOCI + NUMREGS] + vars[reg::isSecondPhaseComparingLOCI]);
		tval += randCoeff[RI(389)] * (vars[reg::isSecondPhaseComparingLOCI] + one()) * (vars[reg::PartialMATCH] + xFE());
		const int rounds = 10;
		for (int i = 0, nextFlagIndex = reg::PartialMATCH, AUX_REGRegIndex, lastFlagIndex; i < rounds; i++){
			int firstPartOfPairIndex = reg::B00 + NUMREGS + i;
			int secondPartOfPairIndex = reg::B20 + NUMREGS + i;
			lastFlagIndex = nextFlagIndex;
			if (i < 8) {
				nextFlagIndex = reg::FLAG1 + i;
			}
			else {
				nextFlagIndex = (8 == i ? (reg::RC + NUMREGS) : (reg::PartialMATCH + NUMREGS));
			}
			if (i < 5) {
				AUX_REGRegIndex = reg::inv1 + 4 * i + NUMREGS;
			}
			else {
				AUX_REGRegIndex = reg::inv1 + 4 * (i - 5);
			}
			//PRNMSG("regIndex = " << AUX_REGRegIndex);
			//PRNMSG("firstPartIndex = " << firstPartOfPairIndex);
			//PRNMSG("secondPartIndex = " << secondPartOfPairIndex);
			//PRNMSG("lastFlagIndex = " << lastFlagIndex);
			//PRNMSG("nextFlagIndex = " << nextFlagIndex);

			FieldElement tmp = (vars[firstPartOfPairIndex] + vars[firstPartOfPairIndex - NUMREGS]) * (vars[firstPartOfPairIndex] + vars[secondPartOfPairIndex - NUMREGS]);
			//FieldElement tmp = (vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][0]) * (vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][1]);
			tval += randCoeff[RI(390 + i)] * tmp*(vars[secondPartOfPairIndex] + vars[firstPartOfPairIndex - NUMREGS])*(vars[secondPartOfPairIndex] + vars[secondPartOfPairIndex - NUMREGS])*(vars[nextFlagIndex]);
			tval += randCoeff[RI(400 + i)] * tmp*((vars[secondPartOfPairIndex] + vars[firstPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 2] + one())*vars[lastFlagIndex] * (vars[nextFlagIndex] + one());
				tval += randCoeff[RI(410 + i)] * tmp*((vars[secondPartOfPairIndex] + vars[secondPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 3] + one())*vars[lastFlagIndex] * (vars[nextFlagIndex] + one());
				tmp = (vars[secondPartOfPairIndex] + vars[firstPartOfPairIndex - NUMREGS])*(vars[secondPartOfPairIndex] + vars[secondPartOfPairIndex - NUMREGS]);
				tval += randCoeff[RI(420 + i)] * ((vars[firstPartOfPairIndex] + vars[firstPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 0] + one())*tmp*vars[lastFlagIndex] * (vars[nextFlagIndex] + one());
				tval += randCoeff[RI(430 + i)] * ((vars[firstPartOfPairIndex] + vars[secondPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 1] + one())*tmp* vars[lastFlagIndex] * (vars[nextFlagIndex] + one());

				tval += randCoeff[RI(440 + i)] * ((vars[firstPartOfPairIndex] + vars[secondPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 1] + one())*((vars[secondPartOfPairIndex] + vars[firstPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 2] + one())* (vars[nextFlagIndex] + vars[lastFlagIndex]);
				tval += randCoeff[RI(450 + i)] * ((vars[firstPartOfPairIndex] + vars[firstPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 0] + one())*((vars[secondPartOfPairIndex] + vars[secondPartOfPairIndex - NUMREGS])*vars[AUX_REGRegIndex + 3] + one())*(vars[nextFlagIndex] + vars[lastFlagIndex]);
				tval += randCoeff[RI(460 + i)] * (vars[lastFlagIndex] + one())*(vars[lastFlagIndex] + xFE())*vars[nextFlagIndex];
		}
		tval *= vars[reg::ST3] * (vars[reg::ST2] + xFE()*xFE());
		return tval;
	}

	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_11_lastStep(const std::vector<FieldElement> & vars, const std::vector<FieldElement> & RootHash){
		FieldElement tval = zero();
		for (int i = 0; i < 20; i++){
			tval += randCoeff[RI(470 + i)] * (vars[reg::B00 + i] + RootHash[i] );
		}

		tval += randCoeff[RI(490)] * (vars[reg::PHASE + NUMREGS] + one());
		tval += randCoeff[RI(491)] * (vars[reg::STATE + NUMREGS] + xFE());
		tval += randCoeff[RI(492)] * (vars[reg::FLAG1 + NUMREGS] + one());
		tval += randCoeff[RI(493)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(494)] * (vars[reg::K + NUMREGS] + vars[reg::K]);

		return tval;
	}

	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_11(const std::vector<FieldElement> & vars, const std::vector<FieldElement> & RootHash, const FieldElement& gN){
		FieldElement tval = vars[reg::PHASE + NUMREGS];
		for (int i = 0; i < 20; i++){
			tval += randCoeff[RI(495)] * (vars[reg::K00 + i + NUMREGS] + vars[reg::B00 + i]);
		}
		tval *= (vars[reg::K] + one());
		tval += randCoeff[RI(515)] * (vars[reg::K + NUMREGS] + vars[reg::K] * xFE());

		tval = (xFE()*vars[reg::K] + gN) * tval;
		tval += ((xFE()*vars[reg::K] + gN) * vars[reg::A] + one()) * evalCPolyStateIsX_FLAGS_ARE_11_lastStep(vars, RootHash);
		return tval;
	}


	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_00(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::FLAG1 + NUMREGS];
		tval += randCoeff[RI(516)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(517)] * (vars[reg::B + NUMREGS] + vars[reg::B]);
		tval += randCoeff[RI(518)] * (vars[reg::C + NUMREGS] + vars[reg::C]);
		tval += randCoeff[RI(519)] * (vars[reg::A + NUMREGS] + vars[reg::K00 + 4 * 8] + xFE()*vars[reg::K00 + 4 * 9]);
		reg::RegType regs[8] = { reg::K00, reg::K04, reg::K13, reg::K22, reg::K31, reg::inv1, reg::inv5, reg::W21};
		
		FieldElement gval = zero();
		for (int i = 7; i >= 0; i--){
			gval = xFE()*gval + vars[regs[i]];
		}
		tval += randCoeff[RI(520)] * (vars[reg::A] + gval);
		
		tval += randCoeff[RI(521)] * (vars[reg::B00 + NUMREGS] + vars[reg::B00] + vars[reg::K00]);
		tval += randCoeff[RI(522)] * (vars[reg::B10 + NUMREGS] + vars[reg::B10] + vars[reg::K04]);
		tval += randCoeff[RI(523)] * (vars[reg::B20 + NUMREGS] + vars[reg::B20] + vars[reg::K13]);
		tval += randCoeff[RI(524)] * (vars[reg::B30 + NUMREGS] + vars[reg::B30] + vars[reg::K22]);
																			
		tval += randCoeff[RI(525)] * (vars[reg::B01 + NUMREGS] + vars[reg::B01] + vars[reg::K31]);
		tval += randCoeff[RI(526)] * (vars[reg::B11 + NUMREGS] + vars[reg::B11] + vars[reg::inv1]);
		tval += randCoeff[RI(527)] * (vars[reg::B21 + NUMREGS] + vars[reg::B21] + vars[reg::inv5]);
		tval += randCoeff[RI(528)] * (vars[reg::B31 + NUMREGS] + vars[reg::B31] + vars[reg::W21]);
																			
		tval += randCoeff[RI(529)] * (vars[reg::B04 + NUMREGS] + vars[reg::B04] + vars[reg::W32]);
		tval += randCoeff[RI(530)] * (vars[reg::B14 + NUMREGS] + vars[reg::B14] + vars[reg::W43]);
		//
		tval += randCoeff[RI(531)] * (vars[reg::B02 + NUMREGS] + vars[reg::B02]);
		tval += randCoeff[RI(532)] * (vars[reg::B12 + NUMREGS] + vars[reg::B12]);
		tval += randCoeff[RI(533)] * (vars[reg::B22 + NUMREGS] + vars[reg::B22]);
		tval += randCoeff[RI(534)] * (vars[reg::B32 + NUMREGS] + vars[reg::B32]);
		
		tval += randCoeff[RI(535)] * (vars[reg::B03 + NUMREGS] + vars[reg::B03]);
		tval += randCoeff[RI(536)] * (vars[reg::B13 + NUMREGS] + vars[reg::B13]);
		tval += randCoeff[RI(537)] * (vars[reg::B23 + NUMREGS] + vars[reg::B23]);
		tval += randCoeff[RI(538)] * (vars[reg::B33 + NUMREGS] + vars[reg::B33]);
		
		tval += randCoeff[RI(539)] * (vars[reg::B24 + NUMREGS] + vars[reg::B24]);
		tval += randCoeff[RI(540)] * (vars[reg::B34 + NUMREGS] + vars[reg::B34]);
		

		return tval;

	}

	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_01(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::FLAG1 + NUMREGS] + one();
		tval += randCoeff[RI(541)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(542)] * (vars[reg::A] + vars[reg::C] + xFE()*xFE()*(vars[reg::K00 + 4 * 8] + xFE()*vars[reg::K00 + 4 * 9]));
		
		reg::RegType regs[8] = { reg::K00, reg::K04, reg::K13, reg::K22, reg::K31, reg::inv1, reg::inv5, reg::W21 };

		FieldElement gval = zero();
		for (int i = 7; i >= 0; i--){
			gval = xFE()*gval + vars[regs[i]];
		}
		tval += randCoeff[RI(543)] * (vars[reg::B] + gval);
		
		tval += randCoeff[RI(544)] * (vars[reg::B02 + NUMREGS] + vars[reg::B02] + vars[reg::K00]);
		tval += randCoeff[RI(545)] * (vars[reg::B12 + NUMREGS] + vars[reg::B12] + vars[reg::K04]);
		tval += randCoeff[RI(546)] * (vars[reg::B22 + NUMREGS] + vars[reg::B22] + vars[reg::K13]);
		tval += randCoeff[RI(547)] * (vars[reg::B32 + NUMREGS] + vars[reg::B32] + vars[reg::K22]);

		tval += randCoeff[RI(548)] * (vars[reg::B03 + NUMREGS] + vars[reg::B03] + vars[reg::K31]);
		tval += randCoeff[RI(549)] * (vars[reg::B13 + NUMREGS] + vars[reg::B13] + vars[reg::inv1]);
		tval += randCoeff[RI(550)] * (vars[reg::B23 + NUMREGS] + vars[reg::B23] + vars[reg::inv5]);
		tval += randCoeff[RI(551)] * (vars[reg::B33 + NUMREGS] + vars[reg::B33] + vars[reg::W21]);

		tval += randCoeff[RI(552)] * (vars[reg::B24 + NUMREGS] + vars[reg::B24] + vars[reg::W32]);
		tval += randCoeff[RI(553)] * (vars[reg::B34 + NUMREGS] + vars[reg::B34] + vars[reg::W43]);

		tval += randCoeff[RI(554)] * (vars[reg::B00 + NUMREGS] + vars[reg::B00]);
		tval += randCoeff[RI(555)] * (vars[reg::B10 + NUMREGS] + vars[reg::B10]);
		tval += randCoeff[RI(556)] * (vars[reg::B20 + NUMREGS] + vars[reg::B20]);
		tval += randCoeff[RI(557)] * (vars[reg::B30 + NUMREGS] + vars[reg::B30]);

		tval += randCoeff[RI(558)] * (vars[reg::B01 + NUMREGS] + vars[reg::B01]);
		tval += randCoeff[RI(559)] * (vars[reg::B11 + NUMREGS] + vars[reg::B11]);
		tval += randCoeff[RI(560)] * (vars[reg::B21 + NUMREGS] + vars[reg::B21]);
		tval += randCoeff[RI(561)] * (vars[reg::B31 + NUMREGS] + vars[reg::B31]);

		tval += randCoeff[RI(562)] * (vars[reg::B04 + NUMREGS] + vars[reg::B04]);
		tval += randCoeff[RI(563)] * (vars[reg::B14 + NUMREGS] + vars[reg::B14]);

		return tval;

	}
	
	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_0_(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::STATE + NUMREGS] + xFE();
		tval += randCoeff[RI(564)] * (vars[reg::K + NUMREGS] + vars[reg::K]);

		for (int i = 0; i < 10; i++){
			tval += randCoeff[RI(565 + i)] * (vars[reg::K00 + 4 * i + 1] + power(vars[reg::K00 + 4 * i], 4));
			tval += randCoeff[RI(575 + i)] * (vars[reg::K00 + 4 * i + 2] + power(vars[reg::K00 + 4 * i + 1], 4));
			tval += randCoeff[RI(585 + i)] * (vars[reg::K00 + 4 * i + 3] + power(vars[reg::K00 + 4 * i + 2], 4));
			tval += randCoeff[RI(595 + i)] * (vars[reg::K00 + 4 * i] + power(vars[reg::K00 + 4 * i + 3], 4)); // for field x elment in group which size 2^n, pow(x, n) = x
		}


		return  tval;
	}
	inline FieldElement evalCPolyStateIsX(const std::vector<FieldElement> & vars, FieldElement lastStepConstraints, const std::vector<FieldElement> & RootHash, FieldElement gN){
		FieldElement tval = (vars[reg::FLAG1] + one()) * evalCPolyStateIsX_FLAGS_ARE_0_(vars);
		tval += randCoeff[RI(605)] * (vars[reg::FLAG1] + one()) * (vars[reg::FLAG2] + one()) * evalCPolyStateIsX_FLAGS_ARE_00(vars);
		tval += randCoeff[RI(606)] * (vars[reg::FLAG1] + one()) * vars[reg::FLAG2] * evalCPolyStateIsX_FLAGS_ARE_01(vars);
		tval += randCoeff[RI(607)] * vars[reg::FLAG1]  * vars[reg::FLAG2] * evalCPolyStateIsX_FLAGS_ARE_11(vars, RootHash, gN);
		
		return tval; //TODO: remove some rand coeffs
	}

	FieldElement evalCPoly(const std::vector<FieldElement> & vars,
		const std::vector<FieldElement> & RootHash, const FieldElement& gN){
#if 0
		for (int i = 0; i < RN; i++){
			randCoeff[i] = one();
		}
#endif
		FieldElement lastStepConstraintsOfAes160 = (vars[reg::STATE + NUMREGS] + xFE());

		FieldElement tval = vars[reg::A + NUMREGS] + vars[reg::A];
		tval += randCoeff[RI(608)]*(vars[reg::B + NUMREGS] + vars[reg::B]);
		tval += randCoeff[RI(609)]*(vars[reg::C + NUMREGS] + vars[reg::C]);
		tval += randCoeff[RI(610)] * (vars[reg::K + NUMREGS] + vars[reg::K]);
		tval += randCoeff[RI(611)] * (vars[reg::PHASE + NUMREGS] + vars[reg::PHASE]);

		tval = (vars[reg::STATE] + xFE()) *  (tval + randCoeff[RI(612)] * evalCPolyAES160_LoopBody(vars, lastStepConstraintsOfAes160));
		tval += (vars[reg::STATE] + one()) * evalCPolyStateIsX(vars, zero(), RootHash, gN);
		tval += randCoeff[RI(613)] * (vars[reg::ST2 + NUMREGS] + one());
		tval += randCoeff[RI(614)] * (vars[reg::isSecondPhaseComparingLOCI + NUMREGS] + vars[reg::isSecondPhaseComparingLOCI]);
		tval *= vars[reg::PHASE];

		FieldElement gval = zero();
		for (int i = 0; i < 20; i++){
			gval += randCoeff[RI(615 + i)] * (vars[reg::K00 + i + NUMREGS] + vars[reg::K00 + i]);
		}
		tval += (vars[reg::PHASE] + one()) * gval;

		gval = evalCPolyST2is1(vars);
		gval += evalCPolyST2isX(vars);
		gval += evalCPolyST2is1XCompareLOCI(vars);
		gval += evalCPolyST2isXX(vars);
		
		tval =  randCoeff[RI(635)] * tval + randCoeff[RI(636)] * gval;
		tval += randCoeff[RI(637)] * (vars[reg::L1 + NUMREGS] + vars[reg::L1]);
		tval += randCoeff[RI(638)] * (vars[reg::L2 + NUMREGS] + vars[reg::L2]);
		tval += randCoeff[RI(639)] * (vars[reg::L3 + NUMREGS] + vars[reg::L3]);
		tval += randCoeff[RI(640)] * (vars[reg::L4 + NUMREGS] + vars[reg::L4]);
		tval += randCoeff[RI(641)] * (vars[reg::L5 + NUMREGS] + vars[reg::L5]);
		tval += randCoeff[RI(642)] * (vars[reg::L6 + NUMREGS] + vars[reg::L6]);
		tval += vars[reg::ST3] + (vars[reg::ST2] + one()) * (vars[reg::ST2] + xFE());
		return tval;
	}

	
	std::vector<FieldElement> evalp::rHash = std::vector<FieldElement>(20);
	FieldElement evalp::last_leaf_index;
	void evalp::setParams(const std::vector<FieldElement>& rootHash, const FieldElement& last_leaf_index, const int prngseed) {
		Algebra::rng.seed(prngseed);
		evalp::last_leaf_index = last_leaf_index;
		for (int i = 0; i < 20; i++)
			rHash[i] = rootHash[reg::B00 + i];
		for (int i = 0; i < RN; ++i)
			randCoeff[i] = Algebra::generateRandom();
	}
	FieldElement evalp::ep(const std::vector<FieldElement>& vars) {
		return evalCPoly(vars, evalp::rHash, evalp::last_leaf_index);
	}
	
#if 0
#include "pyeval_AES160LOCIhashc.cpp"
#endif
} //namespace 
