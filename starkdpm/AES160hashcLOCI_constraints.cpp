
    #include "AES160hashcLOCI_instance.hpp"
    #include <algebraLib/BitExtract.hpp>
    #include "languages/Bair/BairInstance.hpp"
    #include "languages/Bair/BairWitness.hpp"
    
    
    #define ttgenRand (Algebra::one())
    //#define ttgenRand (generateRandom())
    
    using Algebra::FieldElement;
    using Algebra::PolynomialInterface;
    using Algebra::PolynomialDegree;
    using Algebra::degreeOfProduct;
    using Algebra::one;
    using Algebra::generateRandom;
    
    namespace stark_dpm{
    namespace ACSP_FOR_AES160hashcLOCI{

    namespace { //anonymous namespace for polys and common vars
        using Algebra::mapIntegerToFieldElement;

    class polyAES160hashcLOCI_class : public PolynomialInterface{
        public:
          polyAES160hashcLOCI_class() {};

		  Algebra::FieldElement eval(const std::vector<FieldElement>& x)const{
			  return AES160hashcLOCI::evalp::ep(x);
		  }

        bool isEffectiveInput(const size_t varId)const{
        switch(varId)
        {
		  case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::ST3: return false;
		  case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::invRC: return false;
            default: return true;
        }
    }
        
    Algebra::PolynomialDegree getDegreeBound(const std::vector<PolynomialDegree>& inputDegrees)const{
        
        //std::vector<PolynomialDegree> inputDegrees(inputDegrees_.size(), PolynomialDegree(1));

        struct ttdeg{
            long long deg_;
            ttdeg(const PolynomialDegree& d):deg_(PolynomialDegree::integral_t(d)){};
            ttdeg(const long long& d):deg_(d){};

            ttdeg operator*(const ttdeg& d)const{
                return degreeOfProduct(PolynomialDegree(deg_),PolynomialDegree(d.deg_));
            }
            
            ttdeg operator+(const ttdeg& d)const{
                return std::max(deg_,d.deg_);
            }
        };

		  const ttdeg K00 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K00]);
		  const ttdeg K01 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K01]);
		  const ttdeg K02 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K02]);
		  const ttdeg K03 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K03]);
		  const ttdeg K04 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K04]);
		  const ttdeg K10 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K10]);
		  const ttdeg K11 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K11]);
		  const ttdeg K12 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K12]);
		  const ttdeg K13 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K13]);
		  const ttdeg K14 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K14]);
		  const ttdeg K20 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K20]);
		  const ttdeg K21 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K21]);
		  const ttdeg K22 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K22]);
		  const ttdeg K23 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K23]);
		  const ttdeg K24 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K24]);
		  const ttdeg K30 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K30]);
		  const ttdeg K31 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K31]);
		  const ttdeg K32 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K32]);
		  const ttdeg K33 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K33]);
		  const ttdeg K34 = ttdeg(inputDegrees[AES160hashcLOCI::reg::K34]);

		  const ttdeg B00 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B00]);
		  const ttdeg B01 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B01]);
		  const ttdeg B02 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B02]);
		  const ttdeg B03 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B03]);
		  const ttdeg B04 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B04]);
		  const ttdeg B10 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B10]);
		  const ttdeg B11 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B11]);
		  const ttdeg B12 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B12]);
		  const ttdeg B13 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B13]);
		  const ttdeg B14 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B14]);
		  const ttdeg B20 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B20]);
		  const ttdeg B21 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B21]);
		  const ttdeg B22 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B22]);
		  const ttdeg B23 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B23]);
		  const ttdeg B24 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B24]);
		  const ttdeg B30 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B30]);
		  const ttdeg B31 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B31]);
		  const ttdeg B32 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B32]);
		  const ttdeg B33 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B33]);
		  const ttdeg B34 = ttdeg(inputDegrees[AES160hashcLOCI::reg::B34]);

		  const ttdeg W11 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W11]);
		  const ttdeg W12 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W12]);
		  const ttdeg W13 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W13]);
		  const ttdeg W21 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W21]);
		  const ttdeg W22 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W22]);
		  const ttdeg W23 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W23]);
		  const ttdeg W31 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W31]);
		  const ttdeg W32 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W32]);
		  const ttdeg W33 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W33]);
		  const ttdeg W41 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W41]);
		  const ttdeg W42 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W42]);
		  const ttdeg W43 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W43]);
		  const ttdeg W51 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W51]);
		  const ttdeg W52 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W52]);
		  const ttdeg W53 = ttdeg(inputDegrees[AES160hashcLOCI::reg::W53]);

		  const ttdeg inv1 = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv1]);
		  const ttdeg inv2 = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv2]);
		  const ttdeg inv3 = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv3]);
		  const ttdeg inv4 = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv4]);
		  const ttdeg inv5 = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv5]);

		  const ttdeg STATE = ttdeg(inputDegrees[AES160hashcLOCI::reg::STATE]);
		  const ttdeg PHASE = ttdeg(inputDegrees[AES160hashcLOCI::reg::PHASE]);
		  const ttdeg MATCH = ttdeg(inputDegrees[AES160hashcLOCI::reg::MATCH]);
		  const ttdeg ST2 = ttdeg(inputDegrees[AES160hashcLOCI::reg::ST2]);
		  const ttdeg ST3 = ttdeg(inputDegrees[AES160hashcLOCI::reg::ST3]);
		  const ttdeg L1 = ttdeg(inputDegrees[AES160hashcLOCI::reg::L1]);
		  const ttdeg L2 = ttdeg(inputDegrees[AES160hashcLOCI::reg::L2]);
		  const ttdeg L3 = ttdeg(inputDegrees[AES160hashcLOCI::reg::L3]);
		  const ttdeg L4 = ttdeg(inputDegrees[AES160hashcLOCI::reg::L4]);
		  const ttdeg L5 = ttdeg(inputDegrees[AES160hashcLOCI::reg::L5]);
		  const ttdeg L6 = ttdeg(inputDegrees[AES160hashcLOCI::reg::L6]);
		  const ttdeg PartialMATCH = ttdeg(inputDegrees[AES160hashcLOCI::reg::PartialMATCH]);
		  const ttdeg isSecondPhaseComparingLOCI = ttdeg(inputDegrees[AES160hashcLOCI::reg::isSecondPhaseComparingLOCI]);
		  const ttdeg FLAG1 = ttdeg(inputDegrees[AES160hashcLOCI::reg::FLAG1]);
		  const ttdeg FLAG2 = ttdeg(inputDegrees[AES160hashcLOCI::reg::FLAG2]);
		  const ttdeg RC = ttdeg(inputDegrees[AES160hashcLOCI::reg::RC]);
		  const ttdeg A = ttdeg(inputDegrees[AES160hashcLOCI::reg::A]);
		  const ttdeg B = ttdeg(inputDegrees[AES160hashcLOCI::reg::B]);
		  const ttdeg C = ttdeg(inputDegrees[AES160hashcLOCI::reg::C]);
		  const ttdeg K = ttdeg(inputDegrees[AES160hashcLOCI::reg::K]);
		  const ttdeg invRC = ttdeg(inputDegrees[AES160hashcLOCI::reg::invRC]);

		  const ttdeg K00_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K00 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K01_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K01 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K02_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K02 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K03_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K03 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K04_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K04 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K10_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K10 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K11_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K11 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K12_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K12 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K13_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K13 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K14_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K14 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K20_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K20 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K21_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K21 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K22_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K22 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K23_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K23 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K24_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K24 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K30_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K30 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K31_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K31 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K32_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K32 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K33_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K33 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K34_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K34 + AES160hashcLOCI::NUMREGS]);

		  const ttdeg B00_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B00 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B01_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B01 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B02_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B02 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B03_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B03 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B04_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B04 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B10_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B10 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B11_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B11 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B12_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B12 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B13_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B13 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B14_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B14 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B20_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B20 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B21_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B21 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B22_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B22 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B23_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B23 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B24_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B24 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B30_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B30 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B31_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B31 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B32_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B32 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B33_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B33 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B34_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B34 + AES160hashcLOCI::NUMREGS]);

		  const ttdeg W11_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W11 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W12_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W12 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W13_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W13 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W21_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W21 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W22_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W22 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W23_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W23 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W31_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W31 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W32_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W32 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W33_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W33 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W41_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W41 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W42_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W42 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W43_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W43 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W51_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W51 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W52_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W52 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg W53_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::W53 + AES160hashcLOCI::NUMREGS]);

		  const ttdeg inv1_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv1 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg inv2_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv2 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg inv3_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv3 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg inv4_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv4 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg inv5_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::inv5 + AES160hashcLOCI::NUMREGS]);

		  const ttdeg STATE_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::STATE + AES160hashcLOCI::NUMREGS]);
		  const ttdeg PHASE_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::PHASE + AES160hashcLOCI::NUMREGS]);
		  const ttdeg MATCH_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::MATCH + AES160hashcLOCI::NUMREGS]);
		  const ttdeg ST2_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::ST2 + AES160hashcLOCI::NUMREGS]);
		  //const ttdeg ST3_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::ST3 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg L1_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::L1 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg L2_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::L2 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg L3_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::L3 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg L4_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::L4 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg L5_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::L5 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg L6_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::L6 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg PartialMATCH_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::PartialMATCH + AES160hashcLOCI::NUMREGS]);
		  const ttdeg isSecondPhaseComparingLOCI_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::isSecondPhaseComparingLOCI + AES160hashcLOCI::NUMREGS]);
		  const ttdeg FLAG1_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::FLAG1 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg FLAG2_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::FLAG2 + AES160hashcLOCI::NUMREGS]);
		  const ttdeg RC_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::RC + AES160hashcLOCI::NUMREGS]);
		  const ttdeg A_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::A + AES160hashcLOCI::NUMREGS]);
		  const ttdeg B_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::B + AES160hashcLOCI::NUMREGS]);
		  const ttdeg C_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::C + AES160hashcLOCI::NUMREGS]);
		  const ttdeg K_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::K + AES160hashcLOCI::NUMREGS]);
		  //const ttdeg invRC_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::invRC + AES160hashcLOCI::NUMREGS]);

		  const ttdeg resTmp = ((ST3+(ST2)*(ST2))+(L1_next+L1)+(L2_next+L2)+(L3_next+L3)+(L4_next+L4)+(L5_next+L5)+(L6_next+L6)+((PHASE)*((K00_next+K00)+(K01_next+K01)+(K02_next+K02)+(K03_next+K03)+(K04_next+K04)+(K10_next+K10)+(K11_next+K11)+(K12_next+K12)+(K13_next+K13)+(K14_next+K14)+(K20_next+K20)+(K21_next+K21)+(K22_next+K22)+(K23_next+K23)+(K24_next+K24)+(K30_next+K30)+(K31_next+K31)+(K32_next+K32)+(K33_next+K33)+(K34_next+K34))+PHASE*((ST2_next)+(isSecondPhaseComparingLOCI_next+isSecondPhaseComparingLOCI)+((STATE)*((A_next+A)+(B_next+B)+(C_next+C)+(K_next+K)+(PHASE_next+PHASE)+(W11+inv1*inv1*inv1*inv1)+(W12+W11*W11*W11*W11)+(W13+W12*W12*W12*W12)+(W21+inv2*inv2*inv2*inv2)+(W22+W21*W21*W21*W21)+(W23+W22*W22*W22*W22)+(W31+inv3*inv3*inv3*inv3)+(W32+W31*W31*W31*W31)+(W33+W32*W32*W32*W32)+(W41+inv4*inv4*inv4*inv4)+(W42+W41*W41*W41*W41)+(W43+W42*W42*W42*W42)+(W51+inv5*inv5*inv5*inv5)+(W52+W51*W51*W51*W51)+(W53+W52*W52*W52*W52)+((FLAG1)*((STATE_next)+(RC_next+RC)+(K00_next+K00)+(K01_next+K01)+(K02_next+K02)+(K03_next+K03)+(K04_next+K04)+(K10_next+K10)+(K11_next+K11)+(K12_next+K12)+(K13_next+K13)+(K14_next+K14)+(K20_next+K20)+(K21_next+K21)+(K22_next+K22)+(K23_next+K23)+(K24_next+K24)+(K30_next+K30)+(K31_next+K31)+(K32_next+K32)+(K33_next+K33)+(K34_next+K34)+(FLAG1)*(FLAG2)*(FLAG1_next+(FLAG2_next)+(B10_next+B10)+(B11_next+B11)+(B12_next+B12)+(B13_next+B13)+(B14_next+B14)+(B20_next+B20)+(B21_next+B21)+(B22_next+B22)+(B23_next+B23)+(B24_next+B24)+(B30_next+B30)+(B31_next+B31)+(B32_next+B32)+(B33_next+B33)+(B34_next+B34)+(inv1*B00)*(B00+inv1)+(B00_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*B01)*(B01+inv2)+(B01_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*B02)*(B02+inv3)+(B02_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*B03)*(B03+inv4)+(B03_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*B04)*(B04+inv5)+(B04_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53))+(FLAG1)*FLAG2*((FLAG1_next)+FLAG2_next+(B00_next+B00)+(B01_next+B01)+(B02_next+B02)+(B03_next+B03)+(B04_next+B04)+(B20_next+B20)+(B21_next+B21)+(B22_next+B22)+(B23_next+B23)+(B24_next+B24)+(B30_next+B30)+(B31_next+B31)+(B32_next+B32)+(B33_next+B33)+(B34_next+B34)+(inv1*B10)*(B10+inv1)+(B14_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*B11)*(B11+inv2)+(B10_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*B12)*(B12+inv3)+(B11_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*B13)*(B13+inv4)+(B12_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*B14)*(B14+inv5)+(B13_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53))+FLAG1*(FLAG2)*((FLAG1_next)+(FLAG2_next)+(B00_next+B00)+(B01_next+B01)+(B02_next+B02)+(B03_next+B03)+(B04_next+B04)+(B10_next+B10)+(B11_next+B11)+(B12_next+B12)+(B13_next+B13)+(B14_next+B14)+(B30_next+B30)+(B31_next+B31)+(B32_next+B32)+(B33_next+B33)+(B34_next+B34)+(inv1*B20)*(B20+inv1)+(B23_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*B21)*(B21+inv2)+(B24_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*B22)*(B22+inv3)+(B20_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*B23)*(B23+inv4)+(B21_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*B24)*(B24+inv5)+(B22_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53))+FLAG1*FLAG2*((FLAG1_next)+(B00_next+B00)+(B01_next+B01)+(B02_next+B02)+(B03_next+B03)+(B04_next+B04)+(B10_next+B10)+(B11_next+B11)+(B12_next+B12)+(B13_next+B13)+(B14_next+B14)+(B20_next+B20)+(B21_next+B21)+(B22_next+B22)+(B23_next+B23)+(B24_next+B24)+(inv1*B30)*(B30+inv1)+(B32_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*B31)*(B31+inv2)+(B33_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*B32)*(B32+inv3)+(B34_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*B33)*(B33+inv4)+(B30_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*B34)*(B34+inv5)+(B31_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53)))+FLAG1*(FLAG1)*(FLAG1_next+(FLAG2_next)+(inv1*K14)*(K14+inv1)+(inv2*K24)*(K24+inv2)+(inv3*K34)*(K34+inv3)+(inv4*K04)*(K04+inv4)+(RC)*((RC_next+RC)+(STATE_next)+(K00_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC)+(K10_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10)+(K20_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20)+(K30_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30)+(K01_next+K00_next+K01)+(K11_next+K10_next+K11)+(K21_next+K20_next+K21)+(K31_next+K30_next+K31)+(K02_next+K01_next+K02)+(K12_next+K11_next+K12)+(K22_next+K21_next+K22)+(K32_next+K31_next+K32)+(K03_next+K02_next+K03)+(K13_next+K12_next+K13)+(K23_next+K22_next+K23)+(K33_next+K32_next+K33)+(K04_next+K03_next+K04)+(K14_next+K13_next+K14)+(K24_next+K23_next+K24)+(K34_next+K33_next+K34)+(B00_next+B00+B10+B20+B30+K00_next)+(B01_next+B01+B11+B21+B31+K01_next)+(B02_next+B02+B12+B22+B32+K02_next)+(B03_next+B03+B13+B23+B33+K03_next)+(B04_next+B04+B14+B24+B34+K04_next)+(B10_next+B00+B10+B20+B30+K10_next)+(B11_next+B01+B11+B21+B31+K11_next)+(B12_next+B02+B12+B22+B32+K12_next)+(B13_next+B03+B13+B23+B33+K13_next)+(B14_next+B04+B14+B24+B34+K14_next)+(B20_next+B00+B10+B20+B30+K20_next)+(B21_next+B01+B11+B21+B31+K21_next)+(B22_next+B02+B12+B22+B32+K22_next)+(B23_next+B03+B13+B23+B33+K23_next)+(B24_next+B04+B14+B24+B34+K24_next)+(B30_next+B00+B10+B20+B30+K30_next)+(B31_next+B01+B11+B21+B31+K31_next)+(B32_next+B02+B12+B22+B32+K32_next)+(B33_next+B03+B13+B23+B33+K33_next)+(B34_next+B04+B14+B24+B34+K34_next))+((RC)*invRC)*((STATE_next)+(B00_next+B00+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC)+(B10_next+B10+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10)+(B20_next+B20+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20)+(B30_next+B30+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30)+(B01_next+B01+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01)+(B11_next+B11+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11)+(B21_next+B21+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21)+(B31_next+B31+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31)+(B02_next+B02+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01+K02)+(B12_next+B12+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11+K12)+(B22_next+B22+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21+K22)+(B32_next+B32+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31+K32)+(B03_next+B03+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01+K02+K03)+(B13_next+B13+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11+K12+K13)+(B23_next+B23+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21+K22+K23)+(B33_next+B33+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31+K32+K33)+(B04_next+B04+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01+K02+K03+K04)+(B14_next+B14+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11+K12+K13+K14)+(B24_next+B24+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21+K22+K23+K24)+(B34_next+B34+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31+K32+K33+K34)))))+(STATE)*((FLAG1)*((K_next+K)+(STATE_next)+(K01+K00*K00*K00*K00)+(K02+K01*K01*K01*K01)+(K03+K02*K02*K02*K02)+(K00+K03*K03*K03*K03)+(K10+K04*K04*K04*K04)+(K11+K10*K10*K10*K10)+(K12+K11*K11*K11*K11)+(K04+K12*K12*K12*K12)+(K14+K13*K13*K13*K13)+(K20+K14*K14*K14*K14)+(K21+K20*K20*K20*K20)+(K13+K21*K21*K21*K21)+(K23+K22*K22*K22*K22)+(K24+K23*K23*K23*K23)+(K30+K24*K24*K24*K24)+(K22+K30*K30*K30*K30)+(K32+K31*K31*K31*K31)+(K33+K32*K32*K32*K32)+(K34+K33*K33*K33*K33)+(K31+K34*K34*K34*K34)+(inv2+inv1*inv1*inv1*inv1)+(inv3+inv2*inv2*inv2*inv2)+(inv4+inv3*inv3*inv3*inv3)+(inv1+inv4*inv4*inv4*inv4)+(W11+inv5*inv5*inv5*inv5)+(W12+W11*W11*W11*W11)+(W13+W12*W12*W12*W12)+(inv5+W13*W13*W13*W13)+(W22+W21*W21*W21*W21)+(W23+W22*W22*W22*W22)+(W31+W23*W23*W23*W23)+(W21+W31*W31*W31*W31)+(W33+W32*W32*W32*W32)+(W41+W33*W33*W33*W33)+(W42+W41*W41*W41*W41)+(W32+W42*W42*W42*W42)+(W51+W43*W43*W43*W43)+(W52+W51*W51*W51*W51)+(W53+W52*W52*W52*W52)+(W43+W53*W53*W53*W53)+((FLAG2)*((B_next+B)+(C_next+C)+FLAG1_next+(FLAG2_next)+(A_next+W32+W43)+(A+K00+(K04+(K13+(K22+(K31+(inv1+(inv5+(W21))))))))+(B00_next+B00+K00)+(B10_next+B10+K04)+(B20_next+B20+K13)+(B30_next+B30+K22)+(B01_next+B01+K31)+(B11_next+B11+inv1)+(B21_next+B21+inv5)+(B31_next+B31+W21)+(B04_next+B04+W32)+(B14_next+B14+W43)+(B02_next+B02)+(B12_next+B12)+(B22_next+B22)+(B32_next+B32)+(B03_next+B03)+(B13_next+B13)+(B23_next+B23)+(B33_next+B33)+(B24_next+B24)+(B34_next+B34))+FLAG2*((FLAG1_next)+(FLAG2_next)+(C+A+(W32+W43))+(B+K00+(K04+(K13+(K22+(K31+(inv1+(inv5+(W21))))))))+(B02_next+B02+K00)+(B12_next+B12+K04)+(B22_next+B22+K13)+(B32_next+B32+K22)+(B03_next+B03+K31)+(B13_next+B13+inv1)+(B23_next+B23+inv5)+(B33_next+B33+W21)+(B24_next+B24+W32)+(B34_next+B34+W43)+(B00_next+B00)+(B10_next+B10)+(B20_next+B20)+(B30_next+B30)+(B01_next+B01)+(B11_next+B11)+(B21_next+B21)+(B31_next+B31)+(B04_next+B04)+(B14_next+B14))))+FLAG1*(FLAG2*((K)*((K_next+K)+((K)*(PHASE_next+(K00_next+B00)+(K01_next+B01)+(K02_next+B02)+(K03_next+B03)+(K04_next+B04)+(K10_next+B10)+(K11_next+B11)+(K12_next+B12)+(K13_next+B13)+(K14_next+B14)+(K20_next+B20)+(K21_next+B21)+(K22_next+B22)+(K23_next+B23)+(K24_next+B24)+(K30_next+B30)+(K31_next+B31)+(K32_next+B32)+(K33_next+B33)+(K34_next+B34))))+((K)*A)*((PHASE_next)+(K_next+K)+(STATE_next)+(FLAG1_next)+(FLAG2_next)+B00+B01+B02+B03+B04+B10+B11+B12+B13+B14+B20+B21+B22+B23+B24+B30+B31+B32+B33+B34)))))))+((PHASE)*(ST2)*(ST2)*(ST2)*(PHASE_next+(ST2_next)+(isSecondPhaseComparingLOCI_next+isSecondPhaseComparingLOCI)+(B00+B00_next*B00_next*B00_next*B00_next)+(W11+B00*B00*B00*B00)+(W12+W11*W11*W11*W11)+(B00_next+W12*W12*W12*W12)+(B01+B01_next*B01_next*B01_next*B01_next)+(W13+B01*B01*B01*B01)+(W21+W13*W13*W13*W13)+(B01_next+W21*W21*W21*W21)+(B10+B10_next*B10_next*B10_next*B10_next)+(W22+B10*B10*B10*B10)+(W23+W22*W22*W22*W22)+(B10_next+W23*W23*W23*W23)+(B11+B11_next*B11_next*B11_next*B11_next)+(W31+B11*B11*B11*B11)+(W32+W31*W31*W31*W31)+(B11_next+W32*W32*W32*W32)+(B20+B20_next*B20_next*B20_next*B20_next)+(W33+B20*B20*B20*B20)+(W41+W33*W33*W33*W33)+(B20_next+W41*W41*W41*W41)+(B21+B21_next*B21_next*B21_next*B21_next)+(W42+B21*B21*B21*B21)+(W43+W42*W42*W42*W42)+(B21_next+W43*W43*W43*W43)+(B30+B30_next*B30_next*B30_next*B30_next)+(W51+B30*B30*B30*B30)+(W52+W51*W51*W51*W51)+(B30_next+W52*W52*W52*W52)+(B31+B31_next*B31_next*B31_next*B31_next)+(W53+B31*B31*B31*B31)+(W11_next+W53*W53*W53*W53)+(B31_next+W11_next*W11_next*W11_next*W11_next)+(B02+B02_next*B02_next*B02_next*B02_next)+(W12_next+B02*B02*B02*B02)+(W13_next+W12_next*W12_next*W12_next*W12_next)+(B02_next+W13_next*W13_next*W13_next*W13_next)+(B03+B03_next*B03_next*B03_next*B03_next)+(W21_next+B03*B03*B03*B03)+(W22_next+W21_next*W21_next*W21_next*W21_next)+(B03_next+W22_next*W22_next*W22_next*W22_next)+(B12+B12_next*B12_next*B12_next*B12_next)+(W23_next+B12*B12*B12*B12)+(W31_next+W23_next*W23_next*W23_next*W23_next)+(B12_next+W31_next*W31_next*W31_next*W31_next)+(B13+B13_next*B13_next*B13_next*B13_next)+(W32_next+B13*B13*B13*B13)+(W33_next+W32_next*W32_next*W32_next*W32_next)+(B13_next+W33_next*W33_next*W33_next*W33_next)+(B22+B22_next*B22_next*B22_next*B22_next)+(W41_next+B22*B22*B22*B22)+(W42_next+W41_next*W41_next*W41_next*W41_next)+(B22_next+W42_next*W42_next*W42_next*W42_next)+(B23+B23_next*B23_next*B23_next*B23_next)+(W43_next+B23*B23*B23*B23)+(W51_next+W43_next*W43_next*W43_next*W43_next)+(B23_next+W51_next*W51_next*W51_next*W51_next)+(B32+B32_next*B32_next*B32_next*B32_next)+(W52_next+B32*B32*B32*B32)+(W53_next+W52_next*W52_next*W52_next*W52_next)+(B32_next+W53_next*W53_next*W53_next*W53_next)+(B33+B33_next*B33_next*B33_next*B33_next)+(inv1+B33*B33*B33*B33)+(inv2+inv1*inv1*inv1*inv1)+(B33_next+inv2*inv2*inv2*inv2)+(B04+B04_next*B04_next*B04_next*B04_next)+(inv3+B04*B04*B04*B04)+(inv4+inv3*inv3*inv3*inv3)+(B04_next+inv4*inv4*inv4*inv4)+(B14+B14_next*B14_next*B14_next*B14_next)+(inv5+B14*B14*B14*B14)+(inv1_next+inv5*inv5*inv5*inv5)+(B14_next+inv1_next*inv1_next*inv1_next*inv1_next)+(B24+B24_next*B24_next*B24_next*B24_next)+(inv2_next+B24*B24*B24*B24)+(inv3_next+inv2_next*inv2_next*inv2_next*inv2_next)+(B24_next+inv3_next*inv3_next*inv3_next*inv3_next)+(B34+B34_next*B34_next*B34_next*B34_next)+(inv4_next+B34*B34*B34*B34)+(inv5_next+inv4_next*inv4_next*inv4_next*inv4_next)+(B34_next+inv5_next*inv5_next*inv5_next*inv5_next)+(isSecondPhaseComparingLOCI*((L4+B00_next+(B10_next+(B20_next+(B30_next+(B01_next+(B11_next+(B21_next+(B31_next))))))))+(L5+B02_next+(B12_next+(B22_next+(B32_next+(B03_next+(B13_next+(B23_next+(B33_next))))))))+(L6+B04_next+(B14_next+(B24_next+(B34_next)))))+(isSecondPhaseComparingLOCI)*((L1+B00_next+(B10_next+(B20_next+(B30_next+(B01_next+(B11_next+(B21_next+(B31_next))))))))+(L2+B02_next+(B12_next+(B22_next+(B32_next+(B03_next+(B13_next+(B23_next+(B33_next))))))))+(L3+B04_next+(B14_next+(B24_next+(B34_next)))))))+(ST2)*(ST2)*(ST2)*(PHASE_next+(isSecondPhaseComparingLOCI_next+isSecondPhaseComparingLOCI)+(ST2_next)+(B00_next+B00)+(B01_next+B01)+(B02_next+B02)+(B03_next+B03)+(B04_next+B04)+(B10_next+B10)+(B11_next+B11)+(B12_next+B12)+(B13_next+B13)+(B14_next+B14)+(B20_next+B20)+(B21_next+B21)+(B22_next+B22)+(B23_next+B23)+(B24_next+B24)+(B30_next+B30)+(B31_next+B31)+(B32_next+B32)+(B33_next+B33)+(B34_next+B34))+ST3*(ST2)*(PHASE_next+(ST2_next)+(isSecondPhaseComparingLOCI_next+isSecondPhaseComparingLOCI)+(isSecondPhaseComparingLOCI)*(PartialMATCH)+(B00_next+B00)*(B00_next+B20)*((B20_next+B00)*(B20_next+B20)*FLAG1+PartialMATCH*(FLAG1)*(((B20_next+B00)*inv3_next)+((B20_next+B20)*inv4_next)))+(B20_next+B00)*(B20_next+B20)*(PartialMATCH*(FLAG1)*(((B00_next+B00)*inv1_next)+((B00_next+B20)*inv2_next)))+((B00_next+B20)*inv2_next)*((B20_next+B00)*inv3_next)*(FLAG1+PartialMATCH)+((B00_next+B00)*inv1_next)*((B20_next+B20)*inv4_next)*(FLAG1+PartialMATCH)+(PartialMATCH)*(PartialMATCH)*FLAG1+(B01_next+B01)*(B01_next+B21)*((B21_next+B01)*(B21_next+B21)*FLAG2+FLAG1*(FLAG2)*(((B21_next+B01)*W12_next)+((B21_next+B21)*W13_next)))+(B21_next+B01)*(B21_next+B21)*(FLAG1*(FLAG2)*(((B01_next+B01)*inv5_next)+((B01_next+B21)*W11_next)))+((B01_next+B21)*W11_next)*((B21_next+B01)*W12_next)*(FLAG2+FLAG1)+((B01_next+B01)*inv5_next)*((B21_next+B21)*W13_next)*(FLAG2+FLAG1)+(FLAG1)*(FLAG1)*FLAG2+(B02_next+B02)*(B02_next+B22)*((B22_next+B02)*(B22_next+B22)*RC+FLAG2*(RC)*(((B22_next+B02)*W23_next)+((B22_next+B22)*W31_next)))+(B22_next+B02)*(B22_next+B22)*(FLAG2*(RC)*(((B02_next+B02)*W21_next)+((B02_next+B22)*W22_next)))+((B02_next+B22)*W22_next)*((B22_next+B02)*W23_next)*(RC+FLAG2)+((B02_next+B02)*W21_next)*((B22_next+B22)*W31_next)*(RC+FLAG2)+(FLAG2)*(FLAG2)*RC+(B03_next+B03)*(B03_next+B23)*((B23_next+B03)*(B23_next+B23)*invRC+RC*(invRC)*(((B23_next+B03)*W41_next)+((B23_next+B23)*W42_next)))+(B23_next+B03)*(B23_next+B23)*(RC*(invRC)*(((B03_next+B03)*W32_next)+((B03_next+B23)*W33_next)))+((B03_next+B23)*W33_next)*((B23_next+B03)*W41_next)*(invRC+RC)+((B03_next+B03)*W32_next)*((B23_next+B23)*W42_next)*(invRC+RC)+(RC)*(RC)*invRC+(B04_next+B04)*(B04_next+B24)*((B24_next+B04)*(B24_next+B24)*A+invRC*(A)*(((B24_next+B04)*W52_next)+((B24_next+B24)*W53_next)))+(B24_next+B04)*(B24_next+B24)*(invRC*(A)*(((B04_next+B04)*W43_next)+((B04_next+B24)*W51_next)))+((B04_next+B24)*W51_next)*((B24_next+B04)*W52_next)*(A+invRC)+((B04_next+B04)*W43_next)*((B24_next+B24)*W53_next)*(A+invRC)+(invRC)*(invRC)*A+(B10_next+B10)*(B10_next+B30)*((B30_next+B10)*(B30_next+B30)*B+A*(B)*(((B30_next+B10)*inv3)+((B30_next+B30)*inv4)))+(B30_next+B10)*(B30_next+B30)*(A*(B)*(((B10_next+B10)*inv1)+((B10_next+B30)*inv2)))+((B10_next+B30)*inv2)*((B30_next+B10)*inv3)*(B+A)+((B10_next+B10)*inv1)*((B30_next+B30)*inv4)*(B+A)+(A)*(A)*B+(B11_next+B11)*(B11_next+B31)*((B31_next+B11)*(B31_next+B31)*C+B*(C)*(((B31_next+B11)*W12)+((B31_next+B31)*W13)))+(B31_next+B11)*(B31_next+B31)*(B*(C)*(((B11_next+B11)*inv5)+((B11_next+B31)*W11)))+((B11_next+B31)*W11)*((B31_next+B11)*W12)*(C+B)+((B11_next+B11)*inv5)*((B31_next+B31)*W13)*(C+B)+(B)*(B)*C+(B12_next+B12)*(B12_next+B32)*((B32_next+B12)*(B32_next+B32)*STATE+C*(STATE)*(((B32_next+B12)*W23)+((B32_next+B32)*W31)))+(B32_next+B12)*(B32_next+B32)*(C*(STATE)*(((B12_next+B12)*W21)+((B12_next+B32)*W22)))+((B12_next+B32)*W22)*((B32_next+B12)*W23)*(STATE+C)+((B12_next+B12)*W21)*((B32_next+B32)*W31)*(STATE+C)+(C)*(C)*STATE+(B13_next+B13)*(B13_next+B33)*((B33_next+B13)*(B33_next+B33)*RC_next+STATE*(RC_next)*(((B33_next+B13)*W41)+((B33_next+B33)*W42)))+(B33_next+B13)*(B33_next+B33)*(STATE*(RC_next)*(((B13_next+B13)*W32)+((B13_next+B33)*W33)))+((B13_next+B33)*W33)*((B33_next+B13)*W41)*(RC_next+STATE)+((B13_next+B13)*W32)*((B33_next+B33)*W42)*(RC_next+STATE)+(STATE)*(STATE)*RC_next+(B14_next+B14)*(B14_next+B34)*((B34_next+B14)*(B34_next+B34)*PartialMATCH_next+RC_next*(PartialMATCH_next)*(((B34_next+B14)*W52)+((B34_next+B34)*W53)))+(B34_next+B14)*(B34_next+B34)*(RC_next*(PartialMATCH_next)*(((B14_next+B14)*W43)+((B14_next+B34)*W51)))+((B14_next+B34)*W51)*((B34_next+B14)*W52)*(PartialMATCH_next+RC_next)+((B14_next+B14)*W43)*((B34_next+B34)*W53)*(PartialMATCH_next+RC_next)+(RC_next)*(RC_next)*PartialMATCH_next)+ST3*(ST2)*((PHASE_next)+(STATE_next)+FLAG1_next+FLAG2_next+(isSecondPhaseComparingLOCI*(isSecondPhaseComparingLOCI_next+PartialMATCH_next*(PartialMATCH_next)*(MATCH_next)+MATCH*(MATCH)*(MATCH_next)+PartialMATCH_next*(PartialMATCH_next)*(MATCH)*(MATCH_next)+(PartialMATCH_next)*(PartialMATCH_next)*(MATCH)*(MATCH)*MATCH_next)+(isSecondPhaseComparingLOCI)*((isSecondPhaseComparingLOCI_next)+(MATCH_next+MATCH)+(PartialMATCH_next+PartialMATCH)))+(B00_next+B00)+(B01_next+B01)+(B02_next+B02)+(B03_next+B03)+(B04_next+B04)+(B10_next+B10)+(B11_next+B11)+(B12_next+B12)+(B13_next+B13)+(B14_next+B14)+(B20_next+B20)+(B21_next+B21)+(B22_next+B22)+(B23_next+B23)+(B24_next+B24)+(B30_next+B30)+(B31_next+B31)+(B32_next+B32)+(B33_next+B33)+(B34_next+B34)+(A_next+B00+(B10+(B20+(B30+(B01+(B11+(B21+(B31))))))))+(B_next+B02+(B12+(B22+(B32+(B03+(B13+(B23+(B33))))))))+(C_next+B04+(B14+(B24+(B34))))+((K)*STATE)*((L1+A_next)+(L2+B_next)+(L3+C_next)+(L4+K00+(K10+(K20+(K30+(K01+(K11+(K21+(K31))))))))+(L5+K02+(K12+(K22+(K32+(K03+(K13+(K23+(K33))))))))+(L6+K04+(K14+(K24+(K34))))))));

		  //std::cout << "TMP!!degreeBound()=" << resTmp.deg_ << std::endl;
		  return PolynomialDegree(resTmp.deg_);
    }
    

        std::unique_ptr<PolynomialInterface> clone()const{
            return std::unique_ptr<PolynomialInterface>(new polyAES160hashcLOCI_class);
        }

        size_t numVars()const{
            return AES160hashcLOCI::NUMREGS;
        }

    };
    
 } //anonymous namespace for polys

    AES160hashcLOCI_CS::AES160hashcLOCI_CS(){
    polys_.push_back(polyPtr_t(new polyAES160hashcLOCI_class()));

    }
    
    AES160hashcLOCI_CS* AES160hashcLOCI_CS::clone() const{
        return new AES160hashcLOCI_CS();
    }
    
    using std::vector;
    vector<FieldElement> AES160hashcLOCI_CS::eval(const vector<FieldElement>& assignment) const{
        vector<FieldElement> res;
        for(const auto& p: polys_){
            res.push_back(p->eval(assignment));
        }

        return res;
    }
    
    } // ACSP_FOR_AES160hashcLOCI namespace
    } // stark_dpm namespace
    
