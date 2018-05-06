
    #include "LOCI_AES160hashchain_instance.hpp"
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
    namespace ACSP_FOR_AES160LOCIhashchain{

    namespace { //anonymous namespace for polys and common vars
        using Algebra::mapIntegerToFieldElement;

    class polyAES160LOCIhashchain_class : public PolynomialInterface{
        public:
          polyAES160LOCIhashchain_class() {};

		  Algebra::FieldElement eval(const std::vector<FieldElement>& x)const{
			  return AES160LOCIhashchain::evalp::ep(x);
		  }

        bool isEffectiveInput(const size_t varId)const{
        switch(varId)
        {
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W11 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W12 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W13 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W21 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W22 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W23 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W31 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W32 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W33 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W41 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W42 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W43 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W51 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W52 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W53 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv1 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv2 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv3 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv4 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv5 : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::AUX_REG : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::isSecondPhaseComparingLOCI : return false;
case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::invRC : return false;

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
        
        const ttdeg K00 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K00]);
        const ttdeg K01 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K01]);
        const ttdeg K02 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K02]);
        const ttdeg K03 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K03]);
        const ttdeg K04 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K04]);
        const ttdeg K10 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K10]);
        const ttdeg K11 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K11]);
        const ttdeg K12 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K12]);
        const ttdeg K13 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K13]);
        const ttdeg K14 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K14]);
        const ttdeg K20 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K20]);
        const ttdeg K21 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K21]);
        const ttdeg K22 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K22]);
        const ttdeg K23 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K23]);
        const ttdeg K24 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K24]);
        const ttdeg K30 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K30]);
        const ttdeg K31 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K31]);
        const ttdeg K32 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K32]);
        const ttdeg K33 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K33]);
        const ttdeg K34 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K34]);

        const ttdeg X00 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X00]);
        const ttdeg X01 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X01]);
        const ttdeg X02 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X02]);
        const ttdeg X03 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X03]);
        const ttdeg X04 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X04]);
        const ttdeg X10 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X10]);
        const ttdeg X11 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X11]);
        const ttdeg X12 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X12]);
        const ttdeg X13 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X13]);
        const ttdeg X14 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X14]);
        const ttdeg X20 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X20]);
        const ttdeg X21 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X21]);
        const ttdeg X22 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X22]);
        const ttdeg X23 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X23]);
        const ttdeg X24 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X24]);
        const ttdeg X30 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X30]);
        const ttdeg X31 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X31]);
        const ttdeg X32 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X32]);
        const ttdeg X33 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X33]);
        const ttdeg X34 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X34]);

        const ttdeg W11 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W11]);
        const ttdeg W12 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W12]);
        const ttdeg W13 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W13]);
        const ttdeg W21 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W21]);
        const ttdeg W22 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W22]);
        const ttdeg W23 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W23]);
        const ttdeg W31 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W31]);
        const ttdeg W32 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W32]);
        const ttdeg W33 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W33]);
        const ttdeg W41 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W41]);
        const ttdeg W42 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W42]);
        const ttdeg W43 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W43]);
        const ttdeg W51 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W51]);
        const ttdeg W52 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W52]);
        const ttdeg W53 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W53]);

        const ttdeg inv1 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv1]);
        const ttdeg inv2 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv2]);
        const ttdeg inv3 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv3]);
        const ttdeg inv4 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv4]);
        const ttdeg inv5 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv5]);

        const ttdeg STATE = ttdeg(inputDegrees[AES160LOCIhashchain::reg::STATE]);
        const ttdeg PHASE = ttdeg(inputDegrees[AES160LOCIhashchain::reg::PHASE]);
        const ttdeg MATCH = ttdeg(inputDegrees[AES160LOCIhashchain::reg::MATCH]);
        const ttdeg AUX_REG = ttdeg(inputDegrees[AES160LOCIhashchain::reg::AUX_REG]);
        const ttdeg PartialMATCH = ttdeg(inputDegrees[AES160LOCIhashchain::reg::PartialMATCH]);
        const ttdeg isSecondPhaseComparingLOCI = ttdeg(inputDegrees[AES160LOCIhashchain::reg::isSecondPhaseComparingLOCI]);
        const ttdeg FLAG1 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::FLAG1]);
        const ttdeg FLAG2 = ttdeg(inputDegrees[AES160LOCIhashchain::reg::FLAG2]);
        const ttdeg RC = ttdeg(inputDegrees[AES160LOCIhashchain::reg::RC]);
        const ttdeg A = ttdeg(inputDegrees[AES160LOCIhashchain::reg::A]);
        const ttdeg B = ttdeg(inputDegrees[AES160LOCIhashchain::reg::B]);
        const ttdeg C = ttdeg(inputDegrees[AES160LOCIhashchain::reg::C]);
        const ttdeg K = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K]);
        const ttdeg invRC = ttdeg(inputDegrees[AES160LOCIhashchain::reg::invRC]);

        const ttdeg K00_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K00 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K01_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K01 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K02_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K02 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K03_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K03 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K04_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K04 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K10_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K10 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K11_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K11 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K12_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K12 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K13_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K13 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K14_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K14 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K20_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K20 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K21_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K21 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K22_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K22 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K23_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K23 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K24_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K24 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K30_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K30 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K31_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K31 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K32_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K32 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K33_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K33 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K34_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K34 + AES160LOCIhashchain::NUMREGS]);

        const ttdeg X00_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X00 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X01_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X01 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X02_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X02 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X03_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X03 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X04_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X04 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X10_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X10 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X11_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X11 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X12_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X12 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X13_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X13 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X14_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X14 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X20_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X20 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X21_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X21 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X22_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X22 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X23_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X23 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X24_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X24 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X30_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X30 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X31_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X31 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X32_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X32 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X33_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X33 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg X34_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::X34 + AES160LOCIhashchain::NUMREGS]);

        const ttdeg STATE_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::STATE + AES160LOCIhashchain::NUMREGS]);
        const ttdeg PHASE_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::PHASE + AES160LOCIhashchain::NUMREGS]);
        const ttdeg MATCH_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::MATCH + AES160LOCIhashchain::NUMREGS]);
        const ttdeg PartialMATCH_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::PartialMATCH + AES160LOCIhashchain::NUMREGS]);
        const ttdeg FLAG1_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::FLAG1 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg FLAG2_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::FLAG2 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg RC_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::RC + AES160LOCIhashchain::NUMREGS]);
        const ttdeg A_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::A + AES160LOCIhashchain::NUMREGS]);
        const ttdeg B_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::B + AES160LOCIhashchain::NUMREGS]);
        const ttdeg C_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::C + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K + AES160LOCIhashchain::NUMREGS]);

        const ttdeg resTmp = ((PHASE)*((PHASE_next)+(STATE_next)+FLAG1_next+FLAG2_next+(A_next+X00_next+(X10_next+(X20_next+(X30_next+(X01_next+(X11_next+(X21_next+(X31_next))))))))+(B_next+X02_next+(X12_next+(X22_next+(X32_next+(X03_next+(X13_next+(X23_next+(X33_next))))))))+(C_next+X04_next+(X14_next+(X24_next+(X34_next))))+(isSecondPhaseComparingLOCI)*((PartialMATCH)+(X00_next)*(X00_next)*((X20_next)*(X20_next)*FLAG1+PartialMATCH*(FLAG1)*(((X20_next)*X02)+((X20_next)*X03)))+(X20_next)*(X20_next)*(PartialMATCH*(FLAG1)*(((X00_next)*X00)+((X00_next)*X01)))+((X00_next)*X01)*((X20_next)*X02)*(FLAG1+PartialMATCH)+((X00_next)*X00)*((X20_next)*X03)*(FLAG1+PartialMATCH)+(PartialMATCH)*(PartialMATCH)*FLAG1+(X01_next)*(X01_next)*((X21_next)*(X21_next)*FLAG2+FLAG1*(FLAG2)*(((X21_next)*X11)+((X21_next)*X12)))+(X21_next)*(X21_next)*(FLAG1*(FLAG2)*(((X01_next)*X04)+((X01_next)*X10)))+((X01_next)*X10)*((X21_next)*X11)*(FLAG2+FLAG1)+((X01_next)*X04)*((X21_next)*X12)*(FLAG2+FLAG1)+(FLAG1)*(FLAG1)*FLAG2+(X02_next)*(X02_next)*((X22_next)*(X22_next)*RC+FLAG2*(RC)*(((X22_next)*X20)+((X22_next)*X21)))+(X22_next)*(X22_next)*(FLAG2*(RC)*(((X02_next)*X13)+((X02_next)*X14)))+((X02_next)*X14)*((X22_next)*X20)*(RC+FLAG2)+((X02_next)*X13)*((X22_next)*X21)*(RC+FLAG2)+(FLAG2)*(FLAG2)*RC+(X03_next)*(X03_next)*((X23_next)*(X23_next)*invRC+RC*(invRC)*(((X23_next)*X24)+((X23_next)*X30)))+(X23_next)*(X23_next)*(RC*(invRC)*(((X03_next)*X22)+((X03_next)*X23)))+((X03_next)*X23)*((X23_next)*X24)*(invRC+RC)+((X03_next)*X22)*((X23_next)*X30)*(invRC+RC)+(RC)*(RC)*invRC+(X04_next)*(X04_next)*((X24_next)*(X24_next)*A+invRC*(A)*(((X24_next)*X33)+((X24_next)*X34)))+(X24_next)*(X24_next)*(invRC*(A)*(((X04_next)*X31)+((X04_next)*X32)))+((X04_next)*X32)*((X24_next)*X33)*(A+invRC)+((X04_next)*X31)*((X24_next)*X34)*(A+invRC)+(invRC)*(invRC)*A+(X10_next)*(X10_next)*((X30_next)*(X30_next)*B+A*(B)*(((X30_next)*inv3)+((X30_next)*inv4)))+(X30_next)*(X30_next)*(A*(B)*(((X10_next)*inv1)+((X10_next)*inv2)))+((X10_next)*inv2)*((X30_next)*inv3)*(B+A)+((X10_next)*inv1)*((X30_next)*inv4)*(B+A)+(A)*(A)*B+(X11_next)*(X11_next)*((X31_next)*(X31_next)*C+B*(C)*(((X31_next)*W12)+((X31_next)*W13)))+(X31_next)*(X31_next)*(B*(C)*(((X11_next)*inv5)+((X11_next)*W11)))+((X11_next)*W11)*((X31_next)*W12)*(C+B)+((X11_next)*inv5)*((X31_next)*W13)*(C+B)+(B)*(B)*C+(X12_next)*(X12_next)*((X32_next)*(X32_next)*STATE+C*(STATE)*(((X32_next)*W23)+((X32_next)*W31)))+(X32_next)*(X32_next)*(C*(STATE)*(((X12_next)*W21)+((X12_next)*W22)))+((X12_next)*W22)*((X32_next)*W23)*(STATE+C)+((X12_next)*W21)*((X32_next)*W31)*(STATE+C)+(C)*(C)*STATE+(X13_next)*(X13_next)*((X33_next)*(X33_next)*AUX_REG+STATE*(AUX_REG)*(((X33_next)*W41)+((X33_next)*W42)))+(X33_next)*(X33_next)*(STATE*(AUX_REG)*(((X13_next)*W32)+((X13_next)*W33)))+((X13_next)*W33)*((X33_next)*W41)*(AUX_REG+STATE)+((X13_next)*W32)*((X33_next)*W42)*(AUX_REG+STATE)+(STATE)*(STATE)*AUX_REG+(X14_next)*(X14_next)*((X34_next)*(X34_next)*PartialMATCH_next+AUX_REG*(PartialMATCH_next)*(((X34_next)*W52)+((X34_next)*W53)))+(X34_next)*(X34_next)*(AUX_REG*(PartialMATCH_next)*(((X14_next)*W43)+((X14_next)*W51)))+((X14_next)*W51)*((X34_next)*W52)*(PartialMATCH_next+AUX_REG)+((X14_next)*W43)*((X34_next)*W53)*(PartialMATCH_next+AUX_REG)+(AUX_REG)*(AUX_REG)*PartialMATCH_next)+isSecondPhaseComparingLOCI*((X00_next)*(X00_next)*((X20_next)*(X20_next)*FLAG1+PartialMATCH*(FLAG1)*(((X20_next)*X02)+((X20_next)*X03)))+(X20_next)*(X20_next)*(PartialMATCH*(FLAG1)*(((X00_next)*X00)+((X00_next)*X01)))+((X00_next)*X01)*((X20_next)*X02)*(FLAG1+PartialMATCH)+((X00_next)*X00)*((X20_next)*X03)*(FLAG1+PartialMATCH)+(PartialMATCH)*(PartialMATCH)*FLAG1+(X01_next)*(X01_next)*((X21_next)*(X21_next)*FLAG2+FLAG1*(FLAG2)*(((X21_next)*X11)+((X21_next)*X12)))+(X21_next)*(X21_next)*(FLAG1*(FLAG2)*(((X01_next)*X04)+((X01_next)*X10)))+((X01_next)*X10)*((X21_next)*X11)*(FLAG2+FLAG1)+((X01_next)*X04)*((X21_next)*X12)*(FLAG2+FLAG1)+(FLAG1)*(FLAG1)*FLAG2+(X02_next)*(X02_next)*((X22_next)*(X22_next)*RC+FLAG2*(RC)*(((X22_next)*X20)+((X22_next)*X21)))+(X22_next)*(X22_next)*(FLAG2*(RC)*(((X02_next)*X13)+((X02_next)*X14)))+((X02_next)*X14)*((X22_next)*X20)*(RC+FLAG2)+((X02_next)*X13)*((X22_next)*X21)*(RC+FLAG2)+(FLAG2)*(FLAG2)*RC+(X03_next)*(X03_next)*((X23_next)*(X23_next)*invRC+RC*(invRC)*(((X23_next)*X24)+((X23_next)*X30)))+(X23_next)*(X23_next)*(RC*(invRC)*(((X03_next)*X22)+((X03_next)*X23)))+((X03_next)*X23)*((X23_next)*X24)*(invRC+RC)+((X03_next)*X22)*((X23_next)*X30)*(invRC+RC)+(RC)*(RC)*invRC+(X04_next)*(X04_next)*((X24_next)*(X24_next)*A+invRC*(A)*(((X24_next)*X33)+((X24_next)*X34)))+(X24_next)*(X24_next)*(invRC*(A)*(((X04_next)*X31)+((X04_next)*X32)))+((X04_next)*X32)*((X24_next)*X33)*(A+invRC)+((X04_next)*X31)*((X24_next)*X34)*(A+invRC)+(invRC)*(invRC)*A+(X10_next)*(X10_next)*((X30_next)*(X30_next)*B+A*(B)*(((X30_next)*inv3)+((X30_next)*inv4)))+(X30_next)*(X30_next)*(A*(B)*(((X10_next)*inv1)+((X10_next)*inv2)))+((X10_next)*inv2)*((X30_next)*inv3)*(B+A)+((X10_next)*inv1)*((X30_next)*inv4)*(B+A)+(A)*(A)*B+(X11_next)*(X11_next)*((X31_next)*(X31_next)*C+B*(C)*(((X31_next)*W12)+((X31_next)*W13)))+(X31_next)*(X31_next)*(B*(C)*(((X11_next)*inv5)+((X11_next)*W11)))+((X11_next)*W11)*((X31_next)*W12)*(C+B)+((X11_next)*inv5)*((X31_next)*W13)*(C+B)+(B)*(B)*C+(X12_next)*(X12_next)*((X32_next)*(X32_next)*STATE+C*(STATE)*(((X32_next)*W23)+((X32_next)*W31)))+(X32_next)*(X32_next)*(C*(STATE)*(((X12_next)*W21)+((X12_next)*W22)))+((X12_next)*W22)*((X32_next)*W23)*(STATE+C)+((X12_next)*W21)*((X32_next)*W31)*(STATE+C)+(C)*(C)*STATE+(X13_next)*(X13_next)*((X33_next)*(X33_next)*AUX_REG+STATE*(AUX_REG)*(((X33_next)*W41)+((X33_next)*W42)))+(X33_next)*(X33_next)*(STATE*(AUX_REG)*(((X13_next)*W32)+((X13_next)*W33)))+((X13_next)*W33)*((X33_next)*W41)*(AUX_REG+STATE)+((X13_next)*W32)*((X33_next)*W42)*(AUX_REG+STATE)+(STATE)*(STATE)*AUX_REG+(X14_next)*(X14_next)*((X34_next)*(X34_next)*PartialMATCH_next+AUX_REG*(PartialMATCH_next)*(((X34_next)*W52)+((X34_next)*W53)))+(X34_next)*(X34_next)*(AUX_REG*(PartialMATCH_next)*(((X14_next)*W43)+((X14_next)*W51)))+((X14_next)*W51)*((X34_next)*W52)*(PartialMATCH_next+AUX_REG)+((X14_next)*W43)*((X34_next)*W53)*(PartialMATCH_next+AUX_REG)+(AUX_REG)*(AUX_REG)*PartialMATCH_next+PartialMATCH_next*(PartialMATCH_next)*(MATCH_next)+MATCH*(MATCH)*(MATCH_next)+PartialMATCH_next*(PartialMATCH_next)*(MATCH)*(MATCH_next)+(PartialMATCH_next)*(PartialMATCH_next)*(MATCH)*(MATCH)*MATCH_next)+(K00_next+K00)+(K01_next+K01)+(K02_next+K02)+(K03_next+K03)+(K04_next+K04)+(K10_next+K10)+(K11_next+K11)+(K12_next+K12)+(K13_next+K13)+(K14_next+K14)+(K20_next+K20)+(K21_next+K21)+(K22_next+K22)+(K23_next+K23)+(K24_next+K24)+(K30_next+K30)+(K31_next+K31)+(K32_next+K32)+(K33_next+K33)+(K34_next+K34))+PHASE*((STATE)*((A_next+A)+(B_next+B)+(C_next+C)+(K_next+K)+(PHASE_next+PHASE)+(W11+inv1*inv1*inv1*inv1)+(W12+W11*W11*W11*W11)+(W13+W12*W12*W12*W12)+(W21+inv2*inv2*inv2*inv2)+(W22+W21*W21*W21*W21)+(W23+W22*W22*W22*W22)+(W31+inv3*inv3*inv3*inv3)+(W32+W31*W31*W31*W31)+(W33+W32*W32*W32*W32)+(W41+inv4*inv4*inv4*inv4)+(W42+W41*W41*W41*W41)+(W43+W42*W42*W42*W42)+(W51+inv5*inv5*inv5*inv5)+(W52+W51*W51*W51*W51)+(W53+W52*W52*W52*W52)+((FLAG1)*((STATE_next)+(RC_next+RC)+(K00_next+K00)+(K01_next+K01)+(K02_next+K02)+(K03_next+K03)+(K04_next+K04)+(K10_next+K10)+(K11_next+K11)+(K12_next+K12)+(K13_next+K13)+(K14_next+K14)+(K20_next+K20)+(K21_next+K21)+(K22_next+K22)+(K23_next+K23)+(K24_next+K24)+(K30_next+K30)+(K31_next+K31)+(K32_next+K32)+(K33_next+K33)+(K34_next+K34)+(FLAG1)*(FLAG2)*(FLAG1_next+(FLAG2_next)+(X10_next+X10)+(X11_next+X11)+(X12_next+X12)+(X13_next+X13)+(X14_next+X14)+(X20_next+X20)+(X21_next+X21)+(X22_next+X22)+(X23_next+X23)+(X24_next+X24)+(X30_next+X30)+(X31_next+X31)+(X32_next+X32)+(X33_next+X33)+(X34_next+X34)+(inv1*X00)*(X00+inv1)+(X00_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*X01)*(X01+inv2)+(X01_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*X02)*(X02+inv3)+(X02_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*X03)*(X03+inv4)+(X03_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*X04)*(X04+inv5)+(X04_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53))+(FLAG1)*FLAG2*((FLAG1_next)+FLAG2_next+(X00_next+X00)+(X01_next+X01)+(X02_next+X02)+(X03_next+X03)+(X04_next+X04)+(X20_next+X20)+(X21_next+X21)+(X22_next+X22)+(X23_next+X23)+(X24_next+X24)+(X30_next+X30)+(X31_next+X31)+(X32_next+X32)+(X33_next+X33)+(X34_next+X34)+(inv1*X10)*(X10+inv1)+(X14_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*X11)*(X11+inv2)+(X10_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*X12)*(X12+inv3)+(X11_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*X13)*(X13+inv4)+(X12_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*X14)*(X14+inv5)+(X13_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53))+FLAG1*(FLAG2)*((FLAG1_next)+(FLAG2_next)+(X00_next+X00)+(X01_next+X01)+(X02_next+X02)+(X03_next+X03)+(X04_next+X04)+(X10_next+X10)+(X11_next+X11)+(X12_next+X12)+(X13_next+X13)+(X14_next+X14)+(X30_next+X30)+(X31_next+X31)+(X32_next+X32)+(X33_next+X33)+(X34_next+X34)+(inv1*X20)*(X20+inv1)+(X23_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*X21)*(X21+inv2)+(X24_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*X22)*(X22+inv3)+(X20_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*X23)*(X23+inv4)+(X21_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*X24)*(X24+inv5)+(X22_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53))+FLAG1*FLAG2*((FLAG1_next)+(X00_next+X00)+(X01_next+X01)+(X02_next+X02)+(X03_next+X03)+(X04_next+X04)+(X10_next+X10)+(X11_next+X11)+(X12_next+X12)+(X13_next+X13)+(X14_next+X14)+(X20_next+X20)+(X21_next+X21)+(X22_next+X22)+(X23_next+X23)+(X24_next+X24)+(inv1*X30)*(X30+inv1)+(X32_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13)+(inv2*X31)*(X31+inv2)+(X33_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23)+(inv3*X32)*(X32+inv3)+(X34_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33)+(inv4*X33)*(X33+inv4)+(X30_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43)+(inv5*X34)*(X34+inv5)+(X31_next+inv5+inv5*inv5+W51+W51*W51+W52+W52*W52+W53+W53*W53)))+FLAG1*(FLAG1)*(FLAG1_next+(FLAG2_next)+(inv1*K14)*(K14+inv1)+(inv2*K24)*(K24+inv2)+(inv3*K34)*(K34+inv3)+(inv4*K04)*(K04+inv4)+(RC)*((RC_next+RC)+(STATE_next)+(K00_next+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC)+(K10_next+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10)+(K20_next+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20)+(K30_next+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30)+(K01_next+K00_next+K01)+(K11_next+K10_next+K11)+(K21_next+K20_next+K21)+(K31_next+K30_next+K31)+(K02_next+K01_next+K02)+(K12_next+K11_next+K12)+(K22_next+K21_next+K22)+(K32_next+K31_next+K32)+(K03_next+K02_next+K03)+(K13_next+K12_next+K13)+(K23_next+K22_next+K23)+(K33_next+K32_next+K33)+(K04_next+K03_next+K04)+(K14_next+K13_next+K14)+(K24_next+K23_next+K24)+(K34_next+K33_next+K34)+(X00_next+X00+X10+X20+X30+K00_next)+(X01_next+X01+X11+X21+X31+K01_next)+(X02_next+X02+X12+X22+X32+K02_next)+(X03_next+X03+X13+X23+X33+K03_next)+(X04_next+X04+X14+X24+X34+K04_next)+(X10_next+X00+X10+X20+X30+K10_next)+(X11_next+X01+X11+X21+X31+K11_next)+(X12_next+X02+X12+X22+X32+K12_next)+(X13_next+X03+X13+X23+X33+K13_next)+(X14_next+X04+X14+X24+X34+K14_next)+(X20_next+X00+X10+X20+X30+K20_next)+(X21_next+X01+X11+X21+X31+K21_next)+(X22_next+X02+X12+X22+X32+K22_next)+(X23_next+X03+X13+X23+X33+K23_next)+(X24_next+X04+X14+X24+X34+K24_next)+(X30_next+X00+X10+X20+X30+K30_next)+(X31_next+X01+X11+X21+X31+K31_next)+(X32_next+X02+X12+X22+X32+K32_next)+(X33_next+X03+X13+X23+X33+K33_next)+(X34_next+X04+X14+X24+X34+K34_next))+((RC)*invRC)*((STATE_next)+(X00_next+X00+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC)+(X10_next+X10+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10)+(X20_next+X20+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20)+(X30_next+X30+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30)+(X01_next+X01+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01)+(X11_next+X11+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11)+(X21_next+X21+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21)+(X31_next+X31+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31)+(X02_next+X02+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01+K02)+(X12_next+X12+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11+K12)+(X22_next+X22+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21+K22)+(X32_next+X32+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31+K32)+(X03_next+X03+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01+K02+K03)+(X13_next+X13+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11+K12+K13)+(X23_next+X23+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21+K22+K23)+(X33_next+X33+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31+K32+K33)+(X04_next+X04+inv1+inv1*inv1+W11+W11*W11+W12+W12*W12+W13+W13*W13+K00+RC+K01+K02+K03+K04)+(X14_next+X14+inv2+inv2*inv2+W21+W21*W21+W22+W22*W22+W23+W23*W23+K10+K11+K12+K13+K14)+(X24_next+X24+inv3+inv3*inv3+W31+W31*W31+W32+W32*W32+W33+W33*W33+K20+K21+K22+K23+K24)+(X34_next+X34+inv4+inv4*inv4+W41+W41*W41+W42+W42*W42+W43+W43*W43+K30+K31+K32+K33+K34)))))+(STATE)*((FLAG1)*((K_next+K)+(STATE_next)+(K01+K00*K00*K00*K00)+(K02+K01*K01*K01*K01)+(K03+K02*K02*K02*K02)+(K00+K03*K03*K03*K03)+(K10+K04*K04*K04*K04)+(K11+K10*K10*K10*K10)+(K12+K11*K11*K11*K11)+(K04+K12*K12*K12*K12)+(K14+K13*K13*K13*K13)+(K20+K14*K14*K14*K14)+(K21+K20*K20*K20*K20)+(K13+K21*K21*K21*K21)+(K23+K22*K22*K22*K22)+(K24+K23*K23*K23*K23)+(K30+K24*K24*K24*K24)+(K22+K30*K30*K30*K30)+(K32+K31*K31*K31*K31)+(K33+K32*K32*K32*K32)+(K34+K33*K33*K33*K33)+(K31+K34*K34*K34*K34)+(inv2+inv1*inv1*inv1*inv1)+(inv3+inv2*inv2*inv2*inv2)+(inv4+inv3*inv3*inv3*inv3)+(inv1+inv4*inv4*inv4*inv4)+(W11+inv5*inv5*inv5*inv5)+(W12+W11*W11*W11*W11)+(W13+W12*W12*W12*W12)+(inv5+W13*W13*W13*W13)+(W22+W21*W21*W21*W21)+(W23+W22*W22*W22*W22)+(W31+W23*W23*W23*W23)+(W21+W31*W31*W31*W31)+(W33+W32*W32*W32*W32)+(W41+W33*W33*W33*W33)+(W42+W41*W41*W41*W41)+(W32+W42*W42*W42*W42)+(W51+W43*W43*W43*W43)+(W52+W51*W51*W51*W51)+(W53+W52*W52*W52*W52)+(W43+W53*W53*W53*W53)+((FLAG2)*((B_next+B)+(C_next+C)+FLAG1_next+(FLAG2_next)+(A_next+W32+W43)+(A+K00+(K04+(K13+(K22+(K31+(inv1+(inv5+(W21))))))))+(X00_next+X00+K00)+(X10_next+X10+K04)+(X20_next+X20+K13)+(X30_next+X30+K22)+(X01_next+X01+K31)+(X11_next+X11+inv1)+(X21_next+X21+inv5)+(X31_next+X31+W21)+(X04_next+X04+W32)+(X14_next+X14+W43)+(X02_next+X02)+(X12_next+X12)+(X22_next+X22)+(X32_next+X32)+(X03_next+X03)+(X13_next+X13)+(X23_next+X23)+(X33_next+X33)+(X24_next+X24)+(X34_next+X34))+FLAG2*((FLAG1_next)+(FLAG2_next)+(C+A+(W32+W43))+(B+K00+(K04+(K13+(K22+(K31+(inv1+(inv5+(W21))))))))+(X02_next+X02+K00)+(X12_next+X12+K04)+(X22_next+X22+K13)+(X32_next+X32+K22)+(X03_next+X03+K31)+(X13_next+X13+inv1)+(X23_next+X23+inv5)+(X33_next+X33+W21)+(X24_next+X24+W32)+(X34_next+X34+W43)+(X00_next+X00)+(X10_next+X10)+(X20_next+X20)+(X30_next+X30)+(X01_next+X01)+(X11_next+X11)+(X21_next+X21)+(X31_next+X31)+(X04_next+X04)+(X14_next+X14))))+FLAG1*(FLAG2*((K)*(PHASE_next+(K_next+K)+(K00_next+X00)+(K01_next+X01)+(K02_next+X02)+(K03_next+X03)+(K04_next+X04)+(K10_next+X10)+(K11_next+X11)+(K12_next+X12)+(K13_next+X13)+(K14_next+X14)+(K20_next+X20)+(K21_next+X21)+(K22_next+X22)+(K23_next+X23)+(K24_next+X24)+(K30_next+X30)+(K31_next+X31)+(K32_next+X32)+(K33_next+X33)+(K34_next+X34))+((K_next)*A)*((PHASE_next)+(K_next+K)+(STATE_next)+(FLAG1_next)+(FLAG2_next)+X00+X01+X02+X03+X04+X10+X11+X12+X13+X14+X20+X21+X22+X23+X24+X30+X31+X32+X33+X34))))));
;
        //std::cout << "TMP!!degreeBound()=" << resTmp.deg_ << std::endl;
        return PolynomialDegree(resTmp.deg_);
    }
    

        std::unique_ptr<PolynomialInterface> clone()const{
            return std::unique_ptr<PolynomialInterface>(new polyAES160LOCIhashchain_class());
        }

        size_t numVars()const{
            return AES160LOCIhashchain::NUMREGS;
        }

    };
    
 } //anonymous namespace for polys
    
    AES160LOCIhashchain_CS::AES160LOCIhashchain_CS(){
    polys_.push_back(polyPtr_t(new polyAES160LOCIhashchain_class()));

    }
    
    AES160LOCIhashchain_CS* AES160LOCIhashchain_CS::clone() const{
        return new AES160LOCIhashchain_CS();
    }
    
    using std::vector;
    vector<FieldElement> AES160LOCIhashchain_CS::eval(const vector<FieldElement>& assignment) const{
        vector<FieldElement> res;
        for(const auto& p: polys_){
            res.push_back(p->eval(assignment));
        }

        return res;
    }
    
    } // ACSP_FOR_AES160LOCIhashchain namespace
    } // stark_dpm namespace
    
