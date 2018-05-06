#include "LOCI_AES160hashchain.hpp"

#define RI(n) n
//#define RI(n) 0
#define RN 517

using namespace Algebra;

namespace AES160LOCIhashchain{

	FieldElement randCoeff[RN];
	//namespace { //anonymous namespace for polys and common vars


	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_00(const std::vector<FieldElement> & vars, FieldElement results[]){
		FieldElement tval = vars[reg::FLAG1 + NUMREGS];
		tval += randCoeff[RI(0)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(1)] * (vars[reg::X10 + NUMREGS] + vars[reg::X10]);
		tval += randCoeff[RI(2)] * (vars[reg::X11 + NUMREGS] + vars[reg::X11]);
		tval += randCoeff[RI(3)] * (vars[reg::X12 + NUMREGS] + vars[reg::X12]);
		tval += randCoeff[RI(4)] * (vars[reg::X13 + NUMREGS] + vars[reg::X13]);
		tval += randCoeff[RI(5)] * (vars[reg::X14 + NUMREGS] + vars[reg::X14]);
		tval += randCoeff[RI(6)] * (vars[reg::X20 + NUMREGS] + vars[reg::X20]);
		tval += randCoeff[RI(7)] * (vars[reg::X21 + NUMREGS] + vars[reg::X21]);
		tval += randCoeff[RI(8)] * (vars[reg::X22 + NUMREGS] + vars[reg::X22]);
		tval += randCoeff[RI(9)] * (vars[reg::X23 + NUMREGS] + vars[reg::X23]);
		tval += randCoeff[RI(10)] * (vars[reg::X24 + NUMREGS] + vars[reg::X24]);
		tval += randCoeff[RI(11)] * (vars[reg::X30 + NUMREGS] + vars[reg::X30]);
		tval += randCoeff[RI(12)] * (vars[reg::X31 + NUMREGS] + vars[reg::X31]);
		tval += randCoeff[RI(13)] * (vars[reg::X32 + NUMREGS] + vars[reg::X32]);
		tval += randCoeff[RI(14)] * (vars[reg::X33 + NUMREGS] + vars[reg::X33]);
		tval += randCoeff[RI(15)] * (vars[reg::X34 + NUMREGS] + vars[reg::X34]);

		tval += randCoeff[RI(16)] * (vars[reg::X00] * vars[reg::inv1] + one())*(vars[reg::X00] + randCoeff[RI(17)] * vars[reg::inv1]);
		tval += randCoeff[RI(18)] * (
			vars[reg::X00 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(19)] * (vars[reg::X01] * vars[reg::inv2] + one())*(vars[reg::X01] + randCoeff[RI(20)] * vars[reg::inv2]);
		tval += randCoeff[RI(21)] * (
			vars[reg::X01 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(22)] * (vars[reg::X02] * vars[reg::inv3] + one())*(vars[reg::X02] + randCoeff[RI(23)] * vars[reg::inv3]);
		tval += randCoeff[RI(24)] * (
			vars[reg::X02 + NUMREGS] +
			results[2]);

		tval += randCoeff[RI(25)] * (vars[reg::X03] * vars[reg::inv4] + one())*(vars[reg::X03] + randCoeff[RI(26)] * vars[reg::inv4]);
		tval += randCoeff[RI(27)] * (
			vars[reg::X03 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(28)] * (vars[reg::X04] * vars[reg::inv5] + one())*(vars[reg::X04] + randCoeff[RI(29)] * vars[reg::inv5]);
		tval += randCoeff[RI(30)] * (
			vars[reg::X04 + NUMREGS] +
			results[4]);

		return tval;
	}
	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_01(const std::vector<FieldElement> & vars, FieldElement results[]){
		FieldElement tval = zero();
		tval += vars[reg::FLAG1 + NUMREGS] + one();
		tval += randCoeff[RI(31)] * (vars[reg::FLAG2 + NUMREGS] + zero());
		tval += randCoeff[RI(32)] * (vars[reg::X00 + NUMREGS] + vars[reg::X00]);
		tval += randCoeff[RI(33)] * (vars[reg::X01 + NUMREGS] + vars[reg::X01]);
		tval += randCoeff[RI(34)] * (vars[reg::X02 + NUMREGS] + vars[reg::X02]);
		tval += randCoeff[RI(35)] * (vars[reg::X03 + NUMREGS] + vars[reg::X03]);
		tval += randCoeff[RI(36)] * (vars[reg::X04 + NUMREGS] + vars[reg::X04]);
		tval += randCoeff[RI(37)] * (vars[reg::X20 + NUMREGS] + vars[reg::X20]);
		tval += randCoeff[RI(38)] * (vars[reg::X21 + NUMREGS] + vars[reg::X21]);
		tval += randCoeff[RI(39)] * (vars[reg::X22 + NUMREGS] + vars[reg::X22]);
		tval += randCoeff[RI(40)] * (vars[reg::X23 + NUMREGS] + vars[reg::X23]);
		tval += randCoeff[RI(41)] * (vars[reg::X24 + NUMREGS] + vars[reg::X24]);
		tval += randCoeff[RI(42)] * (vars[reg::X30 + NUMREGS] + vars[reg::X30]);
		tval += randCoeff[RI(43)] * (vars[reg::X31 + NUMREGS] + vars[reg::X31]);
		tval += randCoeff[RI(44)] * (vars[reg::X32 + NUMREGS] + vars[reg::X32]);
		tval += randCoeff[RI(45)] * (vars[reg::X33 + NUMREGS] + vars[reg::X33]);
		tval += randCoeff[RI(46)] * (vars[reg::X34 + NUMREGS] + vars[reg::X34]);

		tval += randCoeff[RI(47)] * (vars[reg::X10] * vars[reg::inv1] + one())*(vars[reg::X10] + randCoeff[RI(48)] * vars[reg::inv1]);
		tval += randCoeff[RI(49)] * (
			vars[reg::X14 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(50)] * (vars[reg::X11] * vars[reg::inv2] + one())*(vars[reg::X11] + randCoeff[RI(51)] * vars[reg::inv2]);
		tval += randCoeff[RI(52)] * (
			vars[reg::X10 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(53)] * (vars[reg::X12] * vars[reg::inv3] + one())*(vars[reg::X12] + randCoeff[RI(54)] * vars[reg::inv3]);
		tval += randCoeff[RI(55)] * (
			vars[reg::X11 + NUMREGS] +
			results[2]);
		
		tval += randCoeff[RI(56)] * (vars[reg::X13] * vars[reg::inv4] + one())*(vars[reg::X13] + randCoeff[RI(57)] * vars[reg::inv4]);
		tval += randCoeff[RI(58)] * (
			vars[reg::X12 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(59)] * (vars[reg::X14] * vars[reg::inv5] + one())*(vars[reg::X14] + randCoeff[RI(60)] * vars[reg::inv5]);
		tval += randCoeff[RI(61)] * (
			vars[reg::X13 + NUMREGS] +
			results[4]);
		
		return tval;
	}

	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_10(const std::vector<FieldElement> & vars, FieldElement results[]){

		FieldElement tval = vars[reg::FLAG1 + NUMREGS] + one();
		tval += randCoeff[RI(62)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(63)] * (vars[reg::X00 + NUMREGS] + vars[reg::X00]);
		tval += randCoeff[RI(64)] * (vars[reg::X01 + NUMREGS] + vars[reg::X01]);
		tval += randCoeff[RI(65)] * (vars[reg::X02 + NUMREGS] + vars[reg::X02]);
		tval += randCoeff[RI(66)] * (vars[reg::X03 + NUMREGS] + vars[reg::X03]);
		tval += randCoeff[RI(67)] * (vars[reg::X04 + NUMREGS] + vars[reg::X04]);
		tval += randCoeff[RI(68)] * (vars[reg::X10 + NUMREGS] + vars[reg::X10]);
		tval += randCoeff[RI(69)] * (vars[reg::X11 + NUMREGS] + vars[reg::X11]);
		tval += randCoeff[RI(70)] * (vars[reg::X12 + NUMREGS] + vars[reg::X12]);
		tval += randCoeff[RI(71)] * (vars[reg::X13 + NUMREGS] + vars[reg::X13]);
		tval += randCoeff[RI(72)] * (vars[reg::X14 + NUMREGS] + vars[reg::X14]);
		tval += randCoeff[RI(73)] * (vars[reg::X30 + NUMREGS] + vars[reg::X30]);
		tval += randCoeff[RI(74)] * (vars[reg::X31 + NUMREGS] + vars[reg::X31]);
		tval += randCoeff[RI(75)] * (vars[reg::X32 + NUMREGS] + vars[reg::X32]);
		tval += randCoeff[RI(76)] * (vars[reg::X33 + NUMREGS] + vars[reg::X33]);
		tval += randCoeff[RI(77)] * (vars[reg::X34 + NUMREGS] + vars[reg::X34]);


		tval += randCoeff[RI(78)] * (vars[reg::X20] * vars[reg::inv1] + one())*(vars[reg::X20] + randCoeff[RI(79)] * vars[reg::inv1]);
		tval += randCoeff[RI(80)] * (
			vars[reg::X23 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(81)] * (vars[reg::X21] * vars[reg::inv2] + one())*(vars[reg::X21] + randCoeff[RI(82)] * vars[reg::inv2]);
		tval += randCoeff[RI(83)] * (
			vars[reg::X24 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(84)] * (vars[reg::X22] * vars[reg::inv3] + one())*(vars[reg::X22] + randCoeff[RI(85)] * vars[reg::inv3]);
		tval += randCoeff[RI(86)] * (
			vars[reg::X20 + NUMREGS] +
			results[2]);

		tval += randCoeff[RI(87)] * (vars[reg::X23] * vars[reg::inv4] + one())*(vars[reg::X23] + randCoeff[RI(88)] * vars[reg::inv4]);
		tval += randCoeff[RI(89)] * (
			vars[reg::X21 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(90)] * (vars[reg::X24] * vars[reg::inv5] + one())*(vars[reg::X24] + randCoeff[RI(91)] * vars[reg::inv5]);
		tval += randCoeff[RI(92)] * (
			vars[reg::X22 + NUMREGS] +
			results[4]);
		return tval;

	}


	inline FieldElement evalCPolyAES160_SB_And_SR_FLAGS_ARE_11(const std::vector<FieldElement> & vars, FieldElement results[]){

		FieldElement tval = vars[reg::FLAG1 + NUMREGS] + xFE();
		tval += randCoeff[RI(93)] * (vars[reg::X00 + NUMREGS] + vars[reg::X00]);
		tval += randCoeff[RI(94)] * (vars[reg::X01 + NUMREGS] + vars[reg::X01]);
		tval += randCoeff[RI(95)] * (vars[reg::X02 + NUMREGS] + vars[reg::X02]);
		tval += randCoeff[RI(96)] * (vars[reg::X03 + NUMREGS] + vars[reg::X03]);
		tval += randCoeff[RI(97)] * (vars[reg::X04 + NUMREGS] + vars[reg::X04]);
		tval += randCoeff[RI(98)] * (vars[reg::X10 + NUMREGS] + vars[reg::X10]);
		tval += randCoeff[RI(99)] * (vars[reg::X11 + NUMREGS] + vars[reg::X11]);
		tval += randCoeff[RI(100)] * (vars[reg::X12 + NUMREGS] + vars[reg::X12]);
		tval += randCoeff[RI(101)] * (vars[reg::X13 + NUMREGS] + vars[reg::X13]);
		tval += randCoeff[RI(102)] * (vars[reg::X14 + NUMREGS] + vars[reg::X14]);
		tval += randCoeff[RI(103)] * (vars[reg::X20 + NUMREGS] + vars[reg::X20]);
		tval += randCoeff[RI(104)] * (vars[reg::X21 + NUMREGS] + vars[reg::X21]);
		tval += randCoeff[RI(105)] * (vars[reg::X22 + NUMREGS] + vars[reg::X22]);
		tval += randCoeff[RI(106)] * (vars[reg::X23 + NUMREGS] + vars[reg::X23]);
		tval += randCoeff[RI(107)] * (vars[reg::X24 + NUMREGS] + vars[reg::X24]);


		tval += randCoeff[RI(108)] * (vars[reg::X30] * vars[reg::inv1] + one())*(vars[reg::X30] + randCoeff[RI(109)] * vars[reg::inv1]);
		tval += randCoeff[RI(110)] * (
			vars[reg::X32 + NUMREGS] +
			results[0]);

		tval += randCoeff[RI(111)] * (vars[reg::X31] * vars[reg::inv2] + one())*(vars[reg::X31] + randCoeff[RI(112)] * vars[reg::inv2]);
		tval += randCoeff[RI(113)] * (
			vars[reg::X33 + NUMREGS] +
			results[1]);

		tval += randCoeff[RI(114)] * (vars[reg::X32] * vars[reg::inv3] + one())*(vars[reg::X32] + randCoeff[RI(115)] * vars[reg::inv3]);
		tval += randCoeff[RI(116)] * (
			vars[reg::X34 + NUMREGS] +
			results[2]);

		tval += randCoeff[RI(117)] * (vars[reg::X33] * vars[reg::inv4] + one())*(vars[reg::X33] + randCoeff[RI(118)] * vars[reg::inv4]);
		tval += randCoeff[RI(119)] * (
			vars[reg::X30 + NUMREGS] +
			results[3]);

		tval += randCoeff[RI(120)] * (vars[reg::X34] * vars[reg::inv5] + one())*(vars[reg::X34] + randCoeff[RI(121)] * vars[reg::inv5]);
		tval += randCoeff[RI(122)] * (
			vars[reg::X31 + NUMREGS] +
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

		tval += vars[reg::X00 + NUMREGS] + vars[reg::X00] + K_next[0][0];
		tval += randCoeff[RI(148)] * (vars[reg::X01 + NUMREGS] + vars[reg::X01] + K_next[0][1]);
		tval += randCoeff[RI(149)] * (vars[reg::X02 + NUMREGS] + vars[reg::X02] + K_next[0][2]);
		tval += randCoeff[RI(150)] * (vars[reg::X03 + NUMREGS] + vars[reg::X03] + K_next[0][3]);
		tval += randCoeff[RI(151)] * (vars[reg::X04 + NUMREGS] + vars[reg::X04] + K_next[0][4]);
																		 
		tval += randCoeff[RI(152)] * (vars[reg::X10 + NUMREGS] + vars[reg::X10] + K_next[1][0]);
		tval += randCoeff[RI(153)] * (vars[reg::X11 + NUMREGS] + vars[reg::X11] + K_next[1][1]);
		tval += randCoeff[RI(154)] * (vars[reg::X12 + NUMREGS] + vars[reg::X12] + K_next[1][2]);
		tval += randCoeff[RI(155)] * (vars[reg::X13 + NUMREGS] + vars[reg::X13] + K_next[1][3]);
		tval += randCoeff[RI(156)] * (vars[reg::X14 + NUMREGS] + vars[reg::X14] + K_next[1][4]);
																	
		tval += randCoeff[RI(157)] * (vars[reg::X20 + NUMREGS] + vars[reg::X20] + K_next[2][0]);
		tval += randCoeff[RI(158)] * (vars[reg::X21 + NUMREGS] + vars[reg::X21] + K_next[2][1]);
		tval += randCoeff[RI(159)] * (vars[reg::X22 + NUMREGS] + vars[reg::X22] + K_next[2][2]);
		tval += randCoeff[RI(160)] * (vars[reg::X23 + NUMREGS] + vars[reg::X23] + K_next[2][3]);
		tval += randCoeff[RI(161)] * (vars[reg::X24 + NUMREGS] + vars[reg::X24] + K_next[2][4]);
																		
		tval += randCoeff[RI(162)] * (vars[reg::X30 + NUMREGS] + vars[reg::X30] + K_next[3][0]);
		tval += randCoeff[RI(163)] * (vars[reg::X31 + NUMREGS] + vars[reg::X31] + K_next[3][1]);
		tval += randCoeff[RI(164)] * (vars[reg::X32 + NUMREGS] + vars[reg::X32] + K_next[3][2]);
		tval += randCoeff[RI(165)] * (vars[reg::X33 + NUMREGS] + vars[reg::X33] + K_next[3][3]);
		tval += randCoeff[RI(166)] * (vars[reg::X34 + NUMREGS] + vars[reg::X34] + K_next[3][4]);

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
		
		tval += randCoeff[RI(188)] * (vars[reg::X00 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::X00] + consts::MixColMat[0][1] * vars[reg::X10]
			+ consts::MixColMat[0][2] * vars[reg::X20] + consts::MixColMat[0][3] * vars[reg::X30] + vars[reg::K00 + NUMREGS]);
		
		
		tval += randCoeff[RI(189)] * (vars[reg::X01 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::X01] + consts::MixColMat[0][1] * vars[reg::X11]
			+ consts::MixColMat[0][2] * vars[reg::X21] + consts::MixColMat[0][3] * vars[reg::X31] + vars[reg::K01 + NUMREGS]);
		
		tval += randCoeff[RI(190)] * (vars[reg::X02 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::X02] + consts::MixColMat[0][1] * vars[reg::X12]
			+ consts::MixColMat[0][2] * vars[reg::X22] + consts::MixColMat[0][3] * vars[reg::X32] + vars[reg::K02 + NUMREGS]);
		
		tval += randCoeff[RI(191)] * (vars[reg::X03 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::X03] + consts::MixColMat[0][1] * vars[reg::X13]
			+ consts::MixColMat[0][2] * vars[reg::X23] + consts::MixColMat[0][3] * vars[reg::X33] + vars[reg::K03 + NUMREGS]);
		
		tval += randCoeff[RI(192)] * (vars[reg::X04 + NUMREGS] +
			consts::MixColMat[0][0] * vars[reg::X04] + consts::MixColMat[0][1] * vars[reg::X14]
			+ consts::MixColMat[0][2] * vars[reg::X24] + consts::MixColMat[0][3] * vars[reg::X34] + vars[reg::K04 + NUMREGS]);
		


		tval += randCoeff[RI(193)] * (vars[reg::X10 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::X00] + consts::MixColMat[1][1] * vars[reg::X10]
			+ consts::MixColMat[1][2] * vars[reg::X20] + consts::MixColMat[1][3] * vars[reg::X30] + vars[reg::K10 + NUMREGS]);
		
		tval += randCoeff[RI(194)] * (vars[reg::X11 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::X01] + consts::MixColMat[1][1] * vars[reg::X11]
			+ consts::MixColMat[1][2] * vars[reg::X21] + consts::MixColMat[1][3] * vars[reg::X31] + vars[reg::K11 + NUMREGS]);
		
		tval += randCoeff[RI(195)] * (vars[reg::X12 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::X02] + consts::MixColMat[1][1] * vars[reg::X12]
			+ consts::MixColMat[1][2] * vars[reg::X22] + consts::MixColMat[1][3] * vars[reg::X32] + vars[reg::K12 + NUMREGS]);
		
		tval += randCoeff[RI(196)] * (vars[reg::X13 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::X03] + consts::MixColMat[1][1] * vars[reg::X13]
			+ consts::MixColMat[1][2] * vars[reg::X23] + consts::MixColMat[1][3] * vars[reg::X33] + vars[reg::K13 + NUMREGS]);
		
		tval += randCoeff[RI(197)] * (vars[reg::X14 + NUMREGS] +
			consts::MixColMat[1][0] * vars[reg::X04] + consts::MixColMat[1][1] * vars[reg::X14]
			+ consts::MixColMat[1][2] * vars[reg::X24] + consts::MixColMat[1][3] * vars[reg::X34] + vars[reg::K14 + NUMREGS]);
		
		
		
		tval += randCoeff[RI(198)] * (vars[reg::X20 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::X00] + consts::MixColMat[2][1] * vars[reg::X10]
			+ consts::MixColMat[2][2] * vars[reg::X20] + consts::MixColMat[2][3] * vars[reg::X30] + vars[reg::K20 + NUMREGS]);
		
		tval += randCoeff[RI(199)] * (vars[reg::X21 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::X01] + consts::MixColMat[2][1] * vars[reg::X11]
			+ consts::MixColMat[2][2] * vars[reg::X21] + consts::MixColMat[2][3] * vars[reg::X31] + vars[reg::K21 + NUMREGS]);
		
		tval += randCoeff[RI(200)] * (vars[reg::X22 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::X02] + consts::MixColMat[2][1] * vars[reg::X12]
			+ consts::MixColMat[2][2] * vars[reg::X22] + consts::MixColMat[2][3] * vars[reg::X32] + vars[reg::K22 + NUMREGS]);
		
		tval += randCoeff[RI(201)] * (vars[reg::X23 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::X03] + consts::MixColMat[2][1] * vars[reg::X13]
			+ consts::MixColMat[2][2] * vars[reg::X23] + consts::MixColMat[2][3] * vars[reg::X33] + vars[reg::K23 + NUMREGS]);
		
		tval += randCoeff[RI(202)] * (vars[reg::X24 + NUMREGS] +
			consts::MixColMat[2][0] * vars[reg::X04] + consts::MixColMat[2][1] * vars[reg::X14]
			+ consts::MixColMat[2][2] * vars[reg::X24] + consts::MixColMat[2][3] * vars[reg::X34] + vars[reg::K24 + NUMREGS]);
		
		
		
		tval += randCoeff[RI(203)] * (vars[reg::X30 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::X00] + consts::MixColMat[3][1] * vars[reg::X10]
			+ consts::MixColMat[3][2] * vars[reg::X20] + consts::MixColMat[3][3] * vars[reg::X30] + vars[reg::K30 + NUMREGS]);
		
		tval += randCoeff[RI(204)] * (vars[reg::X31 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::X01] + consts::MixColMat[3][1] * vars[reg::X11]
			+ consts::MixColMat[3][2] * vars[reg::X21] + consts::MixColMat[3][3] * vars[reg::X31] + vars[reg::K31 + NUMREGS]);
		
		tval += randCoeff[RI(205)] * (vars[reg::X32 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::X02] + consts::MixColMat[3][1] * vars[reg::X12]
			+ consts::MixColMat[3][2] * vars[reg::X22] + consts::MixColMat[3][3] * vars[reg::X32] + vars[reg::K32 + NUMREGS]);
		
		tval += randCoeff[RI(206)] * (vars[reg::X33 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::X03] + consts::MixColMat[3][1] * vars[reg::X13]
			+ consts::MixColMat[3][2] * vars[reg::X23] + consts::MixColMat[3][3] * vars[reg::X33] + vars[reg::K33 + NUMREGS]);
		
		tval += randCoeff[RI(207)] * (vars[reg::X34 + NUMREGS] +
			consts::MixColMat[3][0] * vars[reg::X04] + consts::MixColMat[3][1] * vars[reg::X14]
			+ consts::MixColMat[3][2] * vars[reg::X24] + consts::MixColMat[3][3] * vars[reg::X34] + vars[reg::K34 + NUMREGS]);
		
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
	
	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_10CompareLOCI(const std::vector<FieldElement> & vars, const FieldElement values[][2], int secondCompareLoci){
		FieldElement tval = zero();
		int i;
		int rounds = 10;
		for (i = 0; i < rounds; i++){
			int firstPartOfPairIndex = reg::X00 + NUMREGS + i;
			int secondPartOfPairIndex = reg::X20 + NUMREGS + i;
			int lastFlagIndex = i == 0 ? reg::PartialMATCH : (reg::FLAG1 + i - 1);
			int nextFlagIndex = i != rounds - 1 ? (reg::FLAG1 + i) : (reg::PartialMATCH + NUMREGS);
			int AUX_REGRegIndex = (i < 5 ? (reg::X00 + 4 * i) : (reg::inv1 + 4 * (i - 5)));
			//PRNMSG("regIndex = " << AUX_REGRegIndex);
			//PRNMSG("firstPartIndex = " << firstPartOfPairIndex);
			//PRNMSG("secondPartIndex = " << secondPartOfPairIndex);
			//PRNMSG("lastFlagIndex = " << lastFlagIndex);
			//PRNMSG("nextFlagIndex = " << nextFlagIndex);

			FieldElement tmp = (vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][0]) * (vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][1]);
			tval += randCoeff[413 + i + secondCompareLoci ? 48 : 0] * tmp*(vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][0])*(vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][1])*(vars[nextFlagIndex]);
				tval += randCoeff[419 + i + secondCompareLoci ? 49 : 0] * tmp*((vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][0])*vars[AUX_REGRegIndex + 2] + one())*vars[lastFlagIndex] * (vars[nextFlagIndex] + one());
				tval += randCoeff[425 + i + secondCompareLoci ? 50 : 0] * tmp*((vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][1])*vars[AUX_REGRegIndex + 3] + one())*vars[lastFlagIndex] * (vars[nextFlagIndex] + one());
				tmp = (vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][0])*(vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][1]);
				tval += randCoeff[431 + i + secondCompareLoci ? 51 : 0] * ((vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][0])*vars[AUX_REGRegIndex + 0] + one())*tmp*vars[lastFlagIndex] * (vars[nextFlagIndex] + one());
				tval += randCoeff[437 + i + secondCompareLoci ? 52 : 0] * ((vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][1])*vars[AUX_REGRegIndex + 1] + one())*tmp* vars[lastFlagIndex] * (vars[nextFlagIndex] + one());

				tval += randCoeff[443 + i + secondCompareLoci ? 53 : 0] * ((vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][1])*vars[AUX_REGRegIndex + 1] + one())*((vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][0])*vars[AUX_REGRegIndex + 2] + one())* (vars[nextFlagIndex] + vars[lastFlagIndex]);
				tval += randCoeff[449 + i + secondCompareLoci ? 54 : 0] * ((vars[firstPartOfPairIndex] + values[i + secondCompareLoci * rounds][0])*vars[AUX_REGRegIndex + 0] + one())*((vars[secondPartOfPairIndex] + values[i + secondCompareLoci * rounds][1])*vars[AUX_REGRegIndex + 3] + one())*(vars[nextFlagIndex] + vars[lastFlagIndex]);
				tval += randCoeff[455 + i + secondCompareLoci ? 55 : 0] * (vars[lastFlagIndex] + one())*(vars[lastFlagIndex] + xFE())*vars[nextFlagIndex];
		}
		return tval;
	}


	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_10(const std::vector<FieldElement> & vars, const FieldElement values[][2]){
		FieldElement tval = zero();
		tval += vars[reg::PHASE + NUMREGS] + one();
		tval += randCoeff[RI(236)] * (vars[reg::STATE + NUMREGS] + one());
		tval += randCoeff[RI(237)] * (vars[reg::FLAG1 + NUMREGS]);
		tval += randCoeff[RI(238)] * (vars[reg::FLAG2 + NUMREGS]);

		tval += randCoeff[RI(239)] * (vars[reg::A + NUMREGS] + vars[reg::X00 + NUMREGS] +
			xFE()*(vars[reg::X10 + NUMREGS] +
			xFE()*(vars[reg::X20 + NUMREGS] +
			xFE()*(vars[reg::X30 + NUMREGS] +
			xFE()*(vars[reg::X01 + NUMREGS] +
			xFE()*(vars[reg::X11 + NUMREGS] +
			xFE()*(vars[reg::X21 + NUMREGS] +
			xFE()*(vars[reg::X31 + NUMREGS]))))))));
		
		tval += randCoeff[RI(240)] * (vars[reg::B + NUMREGS] + vars[reg::X02 + NUMREGS] +
			xFE()*(vars[reg::X12 + NUMREGS] +
			xFE()*(vars[reg::X22 + NUMREGS] +
			xFE()*(vars[reg::X32 + NUMREGS] +
			xFE()*(vars[reg::X03 + NUMREGS] +
			xFE()*(vars[reg::X13 + NUMREGS] +
			xFE()*(vars[reg::X23 + NUMREGS] +
			xFE()*(vars[reg::X33 + NUMREGS]))))))));
		
		tval += randCoeff[RI(241)] * (vars[reg::C + NUMREGS] + vars[reg::X04 + NUMREGS] +
			xFE()*(vars[reg::X14 + NUMREGS] +
			xFE()*(vars[reg::X24 + NUMREGS] +
			xFE()*(vars[reg::X34 + NUMREGS]))));
		
		FieldElement gval = evalCPolyStateIsX_FLAGS_ARE_10CompareLOCI(vars, values, 0);
		gval += randCoeff[RI(242)] * (vars[reg::PartialMATCH] + xFE());

		tval += randCoeff[RI(243)] * (vars[reg::isSecondPhaseComparingLOCI] + one()) * gval;

		gval = evalCPolyStateIsX_FLAGS_ARE_10CompareLOCI(vars, values, 1);
		gval += randCoeff[RI(244)] * vars[reg::PartialMATCH + NUMREGS] * (vars[reg::PartialMATCH + NUMREGS] + one()) * (vars[reg::MATCH + NUMREGS] + xFE());
		gval += randCoeff[RI(245)] * vars[reg::MATCH] * (vars[reg::MATCH] + one()) * (vars[reg::MATCH + NUMREGS] + xFE());
		gval += randCoeff[RI(246)] * vars[reg::PartialMATCH + NUMREGS] * (vars[reg::PartialMATCH + NUMREGS] + xFE()) * (vars[reg::MATCH] + xFE()) * (vars[reg::MATCH + NUMREGS] + one());
		gval += randCoeff[RI(247)] * (vars[reg::PartialMATCH + NUMREGS] + one())* (vars[reg::PartialMATCH + NUMREGS] + xFE()) * (vars[reg::MATCH] + one()) * (vars[reg::MATCH] + xFE()) * vars[reg::MATCH + NUMREGS];

		tval += randCoeff[RI(248)] * (vars[reg::isSecondPhaseComparingLOCI]) * gval;


		for (int i = 0; i < 20; i++){
			tval += randCoeff[RI(249 + i)] * (vars[reg::K00 + i + NUMREGS] + vars[reg::K00 + i]);
		}
		return tval;

	}

	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_11_lastStep(const std::vector<FieldElement> & vars, const std::vector<FieldElement> & RootHash){
		FieldElement tval = zero();
		for (int i = 0; i < 20; i++){
			tval += randCoeff[RI(269)] * (vars[reg::X00 + i] + RootHash[i] );
		}

		tval += randCoeff[RI(289)] * (vars[reg::PHASE + NUMREGS] + one());
		tval += randCoeff[RI(290)] * (vars[reg::STATE + NUMREGS] + xFE());
		tval += randCoeff[RI(291)] * (vars[reg::FLAG1 + NUMREGS] + one());
		tval += randCoeff[RI(292)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(293)] * (vars[reg::K + NUMREGS] + vars[reg::K]);

		return tval;
	}

	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_11(const std::vector<FieldElement> & vars, const std::vector<FieldElement> & RootHash, const FieldElement& gN){
		FieldElement tval = zero();
		FieldElement gval = zero();
		
		gval += vars[reg::PHASE + NUMREGS];
		gval += randCoeff[RI(294)] * (vars[reg::K + NUMREGS] + vars[reg::K] * xFE());

		for (int i = 0; i < 20; i++){
			gval += randCoeff[RI(295)] * (vars[reg::K00 + i + NUMREGS] + vars[reg::X00 + i]);
		}
		
		tval += (xFE()*vars[reg::K] + gN) * gval;
		tval += randCoeff[RI(315)] * ((vars[reg::K + NUMREGS] + gN) * vars[reg::A] + one()) * evalCPolyStateIsX_FLAGS_ARE_11_lastStep(vars, RootHash);
		return tval;
	}


	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_00(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::FLAG1 + NUMREGS];
		tval += randCoeff[RI(316)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(317)] * (vars[reg::B + NUMREGS] + vars[reg::B]);
		tval += randCoeff[RI(318)] * (vars[reg::C + NUMREGS] + vars[reg::C]);
		tval += randCoeff[RI(319)] * (vars[reg::A + NUMREGS] + vars[reg::K00 + 4 * 8] + xFE()*vars[reg::K00 + 4 * 9]);
		reg::RegType regs[8] = { reg::K00, reg::K04, reg::K13, reg::K22, reg::K31, reg::inv1, reg::inv5, reg::W21};
		
		FieldElement gval = zero();
		for (int i = 7; i >= 0; i--){
			gval = xFE()*gval + vars[regs[i]];
		}
		tval += randCoeff[RI(320)] * (vars[reg::A] + gval);
		
		tval += randCoeff[RI(321)] * (vars[reg::X00 + NUMREGS] + vars[reg::X00] + vars[reg::K00]);
		tval += randCoeff[RI(322)] * (vars[reg::X10 + NUMREGS] + vars[reg::X10] + vars[reg::K04]);
		tval += randCoeff[RI(323)] * (vars[reg::X20 + NUMREGS] + vars[reg::X20] + vars[reg::K13]);
		tval += randCoeff[RI(324)] * (vars[reg::X30 + NUMREGS] + vars[reg::X30] + vars[reg::K22]);
																			
		tval += randCoeff[RI(325)] * (vars[reg::X01 + NUMREGS] + vars[reg::X01] + vars[reg::K31]);
		tval += randCoeff[RI(326)] * (vars[reg::X11 + NUMREGS] + vars[reg::X11] + vars[reg::inv1]);
		tval += randCoeff[RI(327)] * (vars[reg::X21 + NUMREGS] + vars[reg::X21] + vars[reg::inv5]);
		tval += randCoeff[RI(328)] * (vars[reg::X31 + NUMREGS] + vars[reg::X31] + vars[reg::W21]);
																			
		tval += randCoeff[RI(329)] * (vars[reg::X04 + NUMREGS] + vars[reg::X04] + vars[reg::W32]);
		tval += randCoeff[RI(330)] * (vars[reg::X14 + NUMREGS] + vars[reg::X14] + vars[reg::W43]);
		//
		tval += randCoeff[RI(331)] * (vars[reg::X02 + NUMREGS] + vars[reg::X02]);
		tval += randCoeff[RI(332)] * (vars[reg::X12 + NUMREGS] + vars[reg::X12]);
		tval += randCoeff[RI(333)] * (vars[reg::X22 + NUMREGS] + vars[reg::X22]);
		tval += randCoeff[RI(334)] * (vars[reg::X32 + NUMREGS] + vars[reg::X32]);
		
		tval += randCoeff[RI(335)] * (vars[reg::X03 + NUMREGS] + vars[reg::X03]);
		tval += randCoeff[RI(336)] * (vars[reg::X13 + NUMREGS] + vars[reg::X13]);
		tval += randCoeff[RI(337)] * (vars[reg::X23 + NUMREGS] + vars[reg::X23]);
		tval += randCoeff[RI(338)] * (vars[reg::X33 + NUMREGS] + vars[reg::X33]);
		
		tval += randCoeff[RI(339)] * (vars[reg::X24 + NUMREGS] + vars[reg::X24]);
		tval += randCoeff[RI(340)] * (vars[reg::X34 + NUMREGS] + vars[reg::X34]);
		

		return tval;

	}

	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_01(const std::vector<FieldElement> & vars){
		FieldElement tval = vars[reg::FLAG1 + NUMREGS] + one();
		tval += randCoeff[RI(341)] * (vars[reg::FLAG2 + NUMREGS] + one());
		tval += randCoeff[RI(342)] * (vars[reg::A] + vars[reg::C] + xFE()*xFE()*(vars[reg::K00 + 4 * 8] + xFE()*vars[reg::K00 + 4 * 9]));
		
		reg::RegType regs[8] = { reg::K00, reg::K04, reg::K13, reg::K22, reg::K31, reg::inv1, reg::inv5, reg::W21 };

		FieldElement gval = zero();
		for (int i = 7; i >= 0; i--){
			gval = xFE()*gval + vars[regs[i]];
		}
		tval += randCoeff[RI(343)] * (vars[reg::B] + gval);
		
		tval += randCoeff[RI(344)] * (vars[reg::X02 + NUMREGS] + vars[reg::X02] + vars[reg::K00]);
		tval += randCoeff[RI(345)] * (vars[reg::X12 + NUMREGS] + vars[reg::X12] + vars[reg::K04]);
		tval += randCoeff[RI(346)] * (vars[reg::X22 + NUMREGS] + vars[reg::X22] + vars[reg::K13]);
		tval += randCoeff[RI(347)] * (vars[reg::X32 + NUMREGS] + vars[reg::X32] + vars[reg::K22]);

		tval += randCoeff[RI(348)] * (vars[reg::X03 + NUMREGS] + vars[reg::X03] + vars[reg::K31]);
		tval += randCoeff[RI(349)] * (vars[reg::X13 + NUMREGS] + vars[reg::X13] + vars[reg::inv1]);
		tval += randCoeff[RI(350)] * (vars[reg::X23 + NUMREGS] + vars[reg::X23] + vars[reg::inv5]);
		tval += randCoeff[RI(351)] * (vars[reg::X33 + NUMREGS] + vars[reg::X33] + vars[reg::W21]);

		tval += randCoeff[RI(352)] * (vars[reg::X24 + NUMREGS] + vars[reg::X24] + vars[reg::W32]);
		tval += randCoeff[RI(353)] * (vars[reg::X34 + NUMREGS] + vars[reg::X34] + vars[reg::W43]);

		tval += randCoeff[RI(354)] * (vars[reg::X00 + NUMREGS] + vars[reg::X00]);
		tval += randCoeff[RI(355)] * (vars[reg::X10 + NUMREGS] + vars[reg::X10]);
		tval += randCoeff[RI(356)] * (vars[reg::X20 + NUMREGS] + vars[reg::X20]);
		tval += randCoeff[RI(357)] * (vars[reg::X30 + NUMREGS] + vars[reg::X30]);

		tval += randCoeff[RI(358)] * (vars[reg::X01 + NUMREGS] + vars[reg::X01]);
		tval += randCoeff[RI(359)] * (vars[reg::X11 + NUMREGS] + vars[reg::X11]);
		tval += randCoeff[RI(360)] * (vars[reg::X21 + NUMREGS] + vars[reg::X21]);
		tval += randCoeff[RI(361)] * (vars[reg::X31 + NUMREGS] + vars[reg::X31]);

		tval += randCoeff[RI(362)] * (vars[reg::X04 + NUMREGS] + vars[reg::X04]);
		tval += randCoeff[RI(363)] * (vars[reg::X14 + NUMREGS] + vars[reg::X14]);

		return tval;

	}
	
	inline FieldElement evalCPolyStateIsX_FLAGS_ARE_0_(const std::vector<FieldElement> & vars){
		FieldElement tval = zero();
		tval += vars[reg::STATE + NUMREGS] + xFE();
		tval += randCoeff[RI(364)] * (vars[reg::K + NUMREGS] + vars[reg::K]);

		for (int i = 0; i < 10; i++){
			tval += randCoeff[RI(365 + i)] * (vars[reg::K00 + 4 * i + 1] + power(vars[reg::K00 + 4 * i], 4));
			tval += randCoeff[RI(375 + i)] * (vars[reg::K00 + 4 * i + 2] + power(vars[reg::K00 + 4 * i + 1], 4));
			tval += randCoeff[RI(385 + i)] * (vars[reg::K00 + 4 * i + 3] + power(vars[reg::K00 + 4 * i + 2], 4));
			tval += randCoeff[RI(395 + i)] * (vars[reg::K00 + 4 * i] + power(vars[reg::K00 + 4 * i + 3], 4)); // for field x elment in group which size 2^n, pow(x, n) = x
		}


		return  tval;
	}
	inline FieldElement evalCPolyStateIsX(const std::vector<FieldElement> & vars, FieldElement lastStepConstraints, const std::vector<FieldElement> & RootHash, FieldElement gN){
		FieldElement tval = zero();
		tval += (vars[reg::FLAG1] + one()) * evalCPolyStateIsX_FLAGS_ARE_0_(vars);
		tval += randCoeff[RI(405)] * (vars[reg::FLAG1] + one()) * (vars[reg::FLAG2] + one()) * evalCPolyStateIsX_FLAGS_ARE_00(vars);
		tval += randCoeff[RI(406)] * (vars[reg::FLAG1] + one()) * vars[reg::FLAG2] * evalCPolyStateIsX_FLAGS_ARE_01(vars);
		tval += randCoeff[RI(407)] * vars[reg::FLAG1]  * vars[reg::FLAG2] * evalCPolyStateIsX_FLAGS_ARE_11(vars, RootHash, gN);
		
		return tval;
	}

	FieldElement evalCPoly(const std::vector<FieldElement> & vars,
		const std::vector<FieldElement> & RootHash, const FieldElement& gN, const FieldElement values[][2]){
		FieldElement tval = zero();
		FieldElement lastStepConstraintsOfAes160 = (vars[reg::STATE + NUMREGS] + xFE());

		FieldElement saveRegisters = vars[reg::A + NUMREGS] + vars[reg::A];
		saveRegisters += randCoeff[RI(408)]*(vars[reg::B + NUMREGS] + vars[reg::B]);
		saveRegisters += randCoeff[RI(409)]*(vars[reg::C + NUMREGS] + vars[reg::C]);
		saveRegisters += randCoeff[RI(410)] * (vars[reg::K + NUMREGS] + vars[reg::K]);
		saveRegisters += randCoeff[RI(411)] * (vars[reg::PHASE + NUMREGS] + vars[reg::PHASE]);
		tval += vars[reg::PHASE]*(vars[reg::STATE] + xFE()) *  (saveRegisters + randCoeff[RI(412)] * evalCPolyAES160_LoopBody(vars, lastStepConstraintsOfAes160));
		
		tval += (vars[reg::PHASE] + one()) * evalCPolyStateIsX_FLAGS_ARE_10(vars, values);
		tval += vars[reg::PHASE] * (vars[reg::STATE] + one()) * evalCPolyStateIsX(vars, zero(), RootHash, gN);
				
		return tval;
	}

	
	std::vector<FieldElement> evalp::rHash = std::vector<FieldElement>(20);
	FieldElement evalp::last_leaf_index;
	FieldElement evalp::values[20][2];
	void evalp::setParams(const std::vector<FieldElement>& rootHash, const FieldElement& last_leaf_index, const fingerprint_t& fprint) {
		for (int i = 0; i < 20; i++){
            for(int j=0; j<2; j++){
			unsigned int bits = fprint[i][j];
			evalp::values[i][j] = (255 == bits) ? Algebra::zero() : power(consts::xFETransformed, bits);
            }
        }
		evalp::last_leaf_index = last_leaf_index;
		for (int i = 0; i < 20; i++)
			rHash[i] = rootHash[reg::X00 + i];
		for (int i = 0; i < RN; ++i)
			randCoeff[i] = Algebra::generateRandom();
	}
	FieldElement evalp::ep(const std::vector<FieldElement>& vars) {
		return evalCPoly(vars, evalp::rHash, evalp::last_leaf_index, evalp::values);
	}
	
} //namespace 
