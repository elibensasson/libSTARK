import re

def get_clauses_content(s):
    i = 0
    counter = 0
    while (counter >= 0):
        if(s[i] == '(') : counter+=1
        if(s[i] == ')') : counter-=1
        i+=1
    return s[:i-1]

def str_cartesizn_prod(l1,l2):
    lres = []
    for s1 in l1:
        if len(s1) == 0 or s1[-1] == '*' or s1[-1] == "+":
            for s2 in l2:
                lres += [s1+"("+s2+")"]
        else:
            lres += [s1]
    return lres

def parse_constraints(constraints_str):
    res = [""]
    i = 0
    while (i < len(constraints_str)):
        
        if (constraints_str[i] == '(') :
            clauses_content = get_clauses_content(constraints_str[i+1:])
            i+= 1+len(clauses_content)
            clauses_res = parse_constraints(clauses_content)
            res = str_cartesizn_prod(res,clauses_res)
        
        elif (constraints_str[i] == '&'):
            res += [""]

        else:
            res[-1] += constraints_str[i]

        i+=1

    return res


def get_polys_from_constraints(constraints_str):
    return parse_constraints(constraints_str.replace('\n','').replace(' ',''))

def get_poly_degree_bound_cpp(poly_str):
    poly_str_fixed = poly_str.replace("X^minus1","xxdeg")

    poly_str_fixed = poly_str_fixed.replace("converted","")
    poly_str_fixed = poly_str_fixed.replace("X^10","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^11","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^12","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^13","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^14","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^15","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^16","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^17","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^18","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^19","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^20","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^21","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^22","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^23","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^24","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^25","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^26","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^27","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^28","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^29","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^30","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^31","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^32","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^33","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^34","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^35","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^36","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^37","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^38","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^39","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^40","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^41","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^42","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^43","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^44","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^45","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^46","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^47","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^48","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^4","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^5","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^6","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^7","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^8","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("X^9","xxdeg")
    #poly_str_fixed = re.compile("(?<![\w])X(?<![\d])").sub("xxdeg",poly_str_fixed)
    poly_str_fixed = poly_str_fixed.replace("X)","xxdeg)")
    poly_str_fixed = poly_str_fixed.replace("X+","xxdeg+")
    poly_str_fixed = poly_str_fixed.replace("X*","xxdeg*")
    poly_str_fixed = poly_str_fixed.replace("X ","xxdeg ")
    
    poly_str_fixed = poly_str_fixed.replace("alpha_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_4","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_5","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_6","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_7","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("alpha_inv_lsb","xxdeg")
    
    poly_str_fixed = poly_str_fixed.replace("minus1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("VALUE1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("VALUE2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("VALUE3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("B_Transformed","xxdeg")

    poly_str_fixed = poly_str_fixed.replace("Transformator_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Transformator_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Transformator_2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Transformator_3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Transformator_4","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Transformator_5","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Transformator_6","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Transformator_7","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_4","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_5","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_6","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_7","xxdeg")

    poly_str_fixed = poly_str_fixed.replace("last_leaf_index","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("xFETransformed","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("Rcon_round11","xxdeg")

    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_00","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_01","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_02","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_03","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_04","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_05","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_06","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_07","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_08","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_09","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_10","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_11","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_12","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_13","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_14","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_15","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_16","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_17","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_18","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_19","xxdeg")
    
    poly_str_fixed = poly_str_fixed.replace("values_00_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_00_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_01_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_01_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_02_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_02_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_03_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_03_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_04_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_04_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_05_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_05_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_06_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_06_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_07_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_07_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_08_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_08_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_09_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_09_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_10_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_10_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_11_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_11_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_12_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_12_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_13_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_13_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_14_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_14_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_15_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_15_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_16_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_16_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_17_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_17_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_18_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_18_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_19_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("values_19_1","xxdeg")

    poly_str_fixed = poly_str_fixed.replace("MixColMat_0_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_0_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_0_2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_0_3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_1_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_1_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_1_2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_1_3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_2_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_2_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_2_2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_2_3","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_3_0","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_3_1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_3_2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("MixColMat_3_3","xxdeg")

    poly_str_fixed = poly_str_fixed.replace("last_pow","xxdeg")

    poly_str_fixed = re.compile("randomElements.vals\[\d*\]").sub("xxdeg",poly_str_fixed)
    poly_str_fixed = re.compile("k\[\d*\]").sub("xxdeg",poly_str_fixed)

    poly_str_fixed = re.compile("(?<![\w\d\_[])1").sub("xxdeg",poly_str_fixed)

    poly_str_fixed = poly_str_fixed.replace("*xxdeg","") #spaces were already removed for entire str
    poly_str_fixed = poly_str_fixed.replace("xxdeg*","")
    poly_str_fixed = poly_str_fixed.replace("+xxdeg","") 
    poly_str_fixed = poly_str_fixed.replace("xxdeg+","")
    
    res_cpp = """
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

        const ttdeg W11_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W11 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W12_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W12 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W13_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W13 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W21_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W21 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W22_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W22 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W23_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W23 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W31_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W31 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W32_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W32 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W33_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W33 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W41_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W41 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W42_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W42 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W43_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W43 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W51_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W51 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W52_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W52 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg W53_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::W53 + AES160LOCIhashchain::NUMREGS]);
     
        const ttdeg inv1_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv1 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg inv2_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv2 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg inv3_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv3 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg inv4_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv4 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg inv5_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::inv5 + AES160LOCIhashchain::NUMREGS]);
     
        const ttdeg STATE_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::STATE + AES160LOCIhashchain::NUMREGS]);
        const ttdeg PHASE_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::PHASE + AES160LOCIhashchain::NUMREGS]);
        const ttdeg MATCH_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::MATCH + AES160LOCIhashchain::NUMREGS]);
        const ttdeg AUX_REG_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::AUX_REG + AES160LOCIhashchain::NUMREGS]);
        const ttdeg PartialMATCH_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::PartialMATCH + AES160LOCIhashchain::NUMREGS]);
        const ttdeg isSecondPhaseComparingLOCI_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::isSecondPhaseComparingLOCI + AES160LOCIhashchain::NUMREGS]);
        const ttdeg FLAG1_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::FLAG1 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg FLAG2_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::FLAG2 + AES160LOCIhashchain::NUMREGS]);
        const ttdeg RC_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::RC + AES160LOCIhashchain::NUMREGS]);
        const ttdeg A_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::A + AES160LOCIhashchain::NUMREGS]);
        const ttdeg B_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::B + AES160LOCIhashchain::NUMREGS]);
        const ttdeg C_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::C + AES160LOCIhashchain::NUMREGS]);
        const ttdeg K_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::K + AES160LOCIhashchain::NUMREGS]);
        const ttdeg invRC_next = ttdeg(inputDegrees[AES160LOCIhashchain::reg::invRC + AES160LOCIhashchain::NUMREGS]);

        const ttdeg resTmp = """ + poly_str_fixed + """;
        //std::cout << "TMP!!degreeBound()=" << resTmp.deg_ << std::endl;
        return PolynomialDegree(resTmp.deg_);
    }
    """

    return res_cpp

def poly_to_cpp_is_used_var(poly_str):

    poly_str_fixed = re.compile("randomElements.vals\[\d*\]\*").sub("",poly_str)
    poly_str_fixed = poly_str_fixed.replace("X^minus1","")
    poly_str_fixed = poly_str_fixed.replace("alpha_inv_lsb","")
    poly_str_fixed = poly_str_fixed.replace("X^10","")
    poly_str_fixed = poly_str_fixed.replace("X^11","")
    poly_str_fixed = poly_str_fixed.replace("X^12","")
    poly_str_fixed = poly_str_fixed.replace("X^13","")
    poly_str_fixed = poly_str_fixed.replace("X^14","")
    poly_str_fixed = poly_str_fixed.replace("X^15","")
    poly_str_fixed = poly_str_fixed.replace("X^16","")
    poly_str_fixed = poly_str_fixed.replace("X^17","")
    poly_str_fixed = poly_str_fixed.replace("X^18","")
    poly_str_fixed = poly_str_fixed.replace("X^19","")
    poly_str_fixed = poly_str_fixed.replace("X^20","")
    poly_str_fixed = poly_str_fixed.replace("X^21","")
    poly_str_fixed = poly_str_fixed.replace("X^22","")
    poly_str_fixed = poly_str_fixed.replace("X^23","")
    poly_str_fixed = poly_str_fixed.replace("X^24","")
    poly_str_fixed = poly_str_fixed.replace("X^25","")
    poly_str_fixed = poly_str_fixed.replace("X^26","")
    poly_str_fixed = poly_str_fixed.replace("X^27","")
    poly_str_fixed = poly_str_fixed.replace("X^28","")
    poly_str_fixed = poly_str_fixed.replace("X^29","")
    poly_str_fixed = poly_str_fixed.replace("X^30","")
    poly_str_fixed = poly_str_fixed.replace("X^31","")
    poly_str_fixed = poly_str_fixed.replace("X^32","")
    poly_str_fixed = poly_str_fixed.replace("X^33","")
    poly_str_fixed = poly_str_fixed.replace("X^34","")
    poly_str_fixed = poly_str_fixed.replace("X^35","")
    poly_str_fixed = poly_str_fixed.replace("X^36","")
    poly_str_fixed = poly_str_fixed.replace("X^37","")
    poly_str_fixed = poly_str_fixed.replace("X^38","")
    poly_str_fixed = poly_str_fixed.replace("X^39","")
    poly_str_fixed = poly_str_fixed.replace("X^40","")
    poly_str_fixed = poly_str_fixed.replace("X^41","")
    poly_str_fixed = poly_str_fixed.replace("X^42","")
    poly_str_fixed = poly_str_fixed.replace("X^43","")
    poly_str_fixed = poly_str_fixed.replace("X^44","")
    poly_str_fixed = poly_str_fixed.replace("X^45","")
    poly_str_fixed = poly_str_fixed.replace("X^46","")
    poly_str_fixed = poly_str_fixed.replace("X^47","")
    poly_str_fixed = poly_str_fixed.replace("X^48","")
    poly_str_fixed = poly_str_fixed.replace("X^2","")
    poly_str_fixed = poly_str_fixed.replace("X^3","")
    poly_str_fixed = poly_str_fixed.replace("X^4","")
    poly_str_fixed = poly_str_fixed.replace("X^5","")
    poly_str_fixed = poly_str_fixed.replace("X^6","")
    poly_str_fixed = poly_str_fixed.replace("X^7","")
    poly_str_fixed = poly_str_fixed.replace("X^8","")
    poly_str_fixed = poly_str_fixed.replace("X^9","")

    #poly_str_fixed = poly_str_fixed.replace("X","")
    #poly_str_fixed = re.compile("(?<![\w])X").sub("",poly_str_fixed)
    poly_str_fixed = poly_str_fixed.replace("X)","xxdeg)")
    poly_str_fixed = poly_str_fixed.replace("X+","xxdeg+")
    poly_str_fixed = poly_str_fixed.replace("X*","xxdeg*")
    poly_str_fixed = poly_str_fixed.replace("X ","xxdeg ")

    poly_str_fixed = poly_str_fixed.replace("alpha_0","")
    poly_str_fixed = poly_str_fixed.replace("alpha_1","")
    poly_str_fixed = poly_str_fixed.replace("alpha_2","")
    poly_str_fixed = poly_str_fixed.replace("alpha_3","")
    poly_str_fixed = poly_str_fixed.replace("alpha_4","")
    poly_str_fixed = poly_str_fixed.replace("alpha_5","")
    poly_str_fixed = poly_str_fixed.replace("alpha_6","")
    poly_str_fixed = poly_str_fixed.replace("alpha_7","")
    poly_str_fixed = poly_str_fixed.replace("alpha_inv_lsb","")
	
    poly_str_fixed = poly_str_fixed.replace("minus1","")
    poly_str_fixed = poly_str_fixed.replace("B_Transformed","")

    poly_str_fixed = poly_str_fixed.replace("VALUE1","")
    poly_str_fixed = poly_str_fixed.replace("VALUE2","")
    poly_str_fixed = poly_str_fixed.replace("VALUE3","")
	
    poly_str_fixed = poly_str_fixed.replace("Transformator_0","")
    poly_str_fixed = poly_str_fixed.replace("Transformator_1","")
    poly_str_fixed = poly_str_fixed.replace("Transformator_2","")
    poly_str_fixed = poly_str_fixed.replace("Transformator_3","")
    poly_str_fixed = poly_str_fixed.replace("Transformator_4","")
    poly_str_fixed = poly_str_fixed.replace("Transformator_5","")
    poly_str_fixed = poly_str_fixed.replace("Transformator_6","")
    poly_str_fixed = poly_str_fixed.replace("Transformator_7","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_0","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_1","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_2","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_3","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_4","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_5","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_6","")
    poly_str_fixed = poly_str_fixed.replace("polyFromMatrix_7","")

    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_0","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_1","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_2","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_3","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_4","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_5","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_6","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_7","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_8","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_9","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_10","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_11","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_12","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_13","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_14","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_15","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_16","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_17","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_18","")
    poly_str_fixed = poly_str_fixed.replace("ROOT_HASH_19","")
    
	
    poly_str_fixed = re.compile("(?<![\w\d\_[])1").sub("",poly_str_fixed)

    usedVars = re.compile('[^()*+]+').findall(poly_str_fixed)

    res_cpp = """bool isEffectiveInput(const size_t varId)const{
        switch(varId)
        {
            """
    res_cpp += "case AES160LOCIhashchain::reg::K00 : return " + str("K00" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K01 : return " + str("K01" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K02 : return " + str("K02" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K03 : return " + str("K03" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K04 : return " + str("K04" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K10 : return " + str("K10" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K11 : return " + str("K11" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K12 : return " + str("K12" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K13 : return " + str("K13" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K14 : return " + str("K14" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K20 : return " + str("K20" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K21 : return " + str("K21" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K22 : return " + str("K22" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K23 : return " + str("K23" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K24 : return " + str("K24" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K30 : return " + str("K30" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K31 : return " + str("K31" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K32 : return " + str("K32" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K33 : return " + str("K33" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K34 : return " + str("K34" in usedVars).lower() +";\n"

    res_cpp += "case AES160LOCIhashchain::reg::X00 : return " + str("X00" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X01 : return " + str("X01" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X02 : return " + str("X02" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X03 : return " + str("X03" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X04 : return " + str("X04" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X10 : return " + str("X10" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X11 : return " + str("X11" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X12 : return " + str("X12" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X13 : return " + str("X13" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X14 : return " + str("X14" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X20 : return " + str("X20" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X21 : return " + str("X21" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X22 : return " + str("X22" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X23 : return " + str("X23" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X24 : return " + str("X24" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X30 : return " + str("X30" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X31 : return " + str("X31" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X32 : return " + str("X32" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X33 : return " + str("X33" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::X34 : return " + str("X34" in usedVars).lower() +";\n"

    res_cpp += "case AES160LOCIhashchain::reg::W11 : return " + str("W11" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W12 : return " + str("W12" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W13 : return " + str("W13" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W21 : return " + str("W21" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W22 : return " + str("W22" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W23 : return " + str("W23" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W31 : return " + str("W31" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W32 : return " + str("W32" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W33 : return " + str("W33" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W41 : return " + str("W41" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W42 : return " + str("W42" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W43 : return " + str("W43" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W51 : return " + str("W51" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W52 : return " + str("W52" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::W53 : return " + str("W53" in usedVars).lower() +";\n"
    
    res_cpp += "case AES160LOCIhashchain::reg::inv1 : return " + str("inv1" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::inv2 : return " + str("inv2" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::inv3 : return " + str("inv3" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::inv4 : return " + str("inv4" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::inv5 : return " + str("inv5" in usedVars).lower() +";\n"
    
    res_cpp += "case AES160LOCIhashchain::reg::STATE : return " + str("STATE" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::PHASE : return " + str("PHASE" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::MATCH : return " + str("MATCH" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::AUX_REG : return " + str("AUX_REG" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::PartialMATCH : return " + str("PartialMATCH" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::isSecondPhaseComparingLOCI : return " + str("isSecondPhaseComparingLOCI" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::FLAG1 : return " + str("FLAG1" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::FLAG2 : return " + str("FLAG2" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::RC : return " + str("RC" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::A : return " + str("A" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::B : return " + str("B" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::C : return " + str("C" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::K : return " + str("K" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::reg::invRC : return " + str("invRC" in usedVars).lower() +";\n"
    
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K00 : return " + str("K00_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K01 : return " + str("K01_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K02 : return " + str("K02_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K03 : return " + str("K03_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K04 : return " + str("K04_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K10 : return " + str("K10_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K11 : return " + str("K11_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K12 : return " + str("K12_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K13 : return " + str("K13_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K14 : return " + str("K14_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K20 : return " + str("K20_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K21 : return " + str("K21_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K22 : return " + str("K22_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K23 : return " + str("K23_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K24 : return " + str("K24_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K30 : return " + str("K30_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K31 : return " + str("K31_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K32 : return " + str("K32_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K33 : return " + str("K33_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K34 : return " + str("K34_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X00 : return " + str("X00_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X01 : return " + str("X01_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X02 : return " + str("X02_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X03 : return " + str("X03_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X04 : return " + str("X04_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X10 : return " + str("X10_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X11 : return " + str("X11_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X12 : return " + str("X12_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X13 : return " + str("X13_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X14 : return " + str("X14_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X20 : return " + str("X20_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X21 : return " + str("X21_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X22 : return " + str("X22_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X23 : return " + str("X23_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X24 : return " + str("X24_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X30 : return " + str("X30_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X31 : return " + str("X31_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X32 : return " + str("X32_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X33 : return " + str("X33_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::X34 : return " + str("X34_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W11 : return " + str("W11_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W12 : return " + str("W12_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W13 : return " + str("W13_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W21 : return " + str("W21_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W22 : return " + str("W22_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W23 : return " + str("W23_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W31 : return " + str("W31_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W32 : return " + str("W32_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W33 : return " + str("W33_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W41 : return " + str("W41_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W42 : return " + str("W42_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W43 : return " + str("W43_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W51 : return " + str("W51_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W52 : return " + str("W52_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::W53 : return " + str("W53_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv1 : return " + str("inv1_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv2 : return " + str("inv2_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv3 : return " + str("inv3_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv4 : return " + str("inv4_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::inv5 : return " + str("inv5_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::STATE : return " + str("STATE_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::PHASE : return " + str("PHASE_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::MATCH : return " + str("MATCH_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::AUX_REG : return " + str("AUX_REG_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::PartialMATCH : return " + str("PartialMATCH_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::isSecondPhaseComparingLOCI : return " + str("isSecondPhaseComparingLOCI_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::FLAG1 : return " + str("FLAG1_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::FLAG2 : return " + str("FLAG2_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::RC : return " + str("RC_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::A : return " + str("A_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::B : return " + str("B_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::C : return " + str("C_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::K : return " + str("K_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160LOCIhashchain::NUMREGS + AES160LOCIhashchain::reg::invRC : return " + str("invRC_next" in usedVars).lower() +";\n"
    res_cpp += """
            default: return false;
        }
    }"""

    return res_cpp

def poly_to_cpp_eval(poly_str):
    poly_str_fixed = poly_str.replace("X^minus1","AES160LOCIhashchain::consts::xFEinv")

    poly_str_fixed = poly_str_fixed.replace("X^10","X_aes10")
    poly_str_fixed = poly_str_fixed.replace("X^11","X_aes11")
    poly_str_fixed = poly_str_fixed.replace("X^12","X_aes12")
    poly_str_fixed = poly_str_fixed.replace("X^13","X_aes13")
    poly_str_fixed = poly_str_fixed.replace("X^14","X_aes14")
    poly_str_fixed = poly_str_fixed.replace("X^15","X_aes15")
    poly_str_fixed = poly_str_fixed.replace("X^16","X_aes16")
    poly_str_fixed = poly_str_fixed.replace("X^17","X_aes17")
    poly_str_fixed = poly_str_fixed.replace("X^18","X_aes18")
    poly_str_fixed = poly_str_fixed.replace("X^19","X_aes19")
    poly_str_fixed = poly_str_fixed.replace("X^20","X_aes20")
    poly_str_fixed = poly_str_fixed.replace("X^21","X_aes21")
    poly_str_fixed = poly_str_fixed.replace("X^22","X_aes22")
    poly_str_fixed = poly_str_fixed.replace("X^23","X_aes23")
    poly_str_fixed = poly_str_fixed.replace("X^24","X_aes24")
    poly_str_fixed = poly_str_fixed.replace("X^25","X_aes25")
    poly_str_fixed = poly_str_fixed.replace("X^26","X_aes26")
    poly_str_fixed = poly_str_fixed.replace("X^27","X_aes27")
    poly_str_fixed = poly_str_fixed.replace("X^28","X_aes28")
    poly_str_fixed = poly_str_fixed.replace("X^29","X_aes29")
    poly_str_fixed = poly_str_fixed.replace("X^30","X_aes30")
    poly_str_fixed = poly_str_fixed.replace("X^31","X_aes31")
    poly_str_fixed = poly_str_fixed.replace("X^32","X_aes32")
    poly_str_fixed = poly_str_fixed.replace("X^33","X_aes33")
    poly_str_fixed = poly_str_fixed.replace("X^34","X_aes34")
    poly_str_fixed = poly_str_fixed.replace("X^35","X_aes35")
    poly_str_fixed = poly_str_fixed.replace("X^36","X_aes36")
    poly_str_fixed = poly_str_fixed.replace("X^37","X_aes37")
    poly_str_fixed = poly_str_fixed.replace("X^38","X_aes38")
    poly_str_fixed = poly_str_fixed.replace("X^39","X_aes39")
    poly_str_fixed = poly_str_fixed.replace("X^40","X_aes40")
    poly_str_fixed = poly_str_fixed.replace("X^41","X_aes41")
    poly_str_fixed = poly_str_fixed.replace("X^42","X_aes42")
    poly_str_fixed = poly_str_fixed.replace("X^43","X_aes43")
    poly_str_fixed = poly_str_fixed.replace("X^44","X_aes44")
    poly_str_fixed = poly_str_fixed.replace("X^45","X_aes45")
    poly_str_fixed = poly_str_fixed.replace("X^46","X_aes46")
    poly_str_fixed = poly_str_fixed.replace("X^47","X_aes47")
    poly_str_fixed = poly_str_fixed.replace("X^48","X_aes48")
    poly_str_fixed = poly_str_fixed.replace("X^2","X_aes02")
    poly_str_fixed = poly_str_fixed.replace("X^3","X_aes03")
    poly_str_fixed = poly_str_fixed.replace("X^4","X_aes04")
    poly_str_fixed = poly_str_fixed.replace("X^5","X_aes05")
    poly_str_fixed = poly_str_fixed.replace("X^6","X_aes06")
    poly_str_fixed = poly_str_fixed.replace("X^7","X_aes07")
    poly_str_fixed = poly_str_fixed.replace("X^8","X_aes08")
    poly_str_fixed = poly_str_fixed.replace("X^9","X_aes09")

    #poly_str_fixed = poly_str_fixed.replace("alpha_inv_lsb","invExtrConsts[0]")
    poly_str_fixed = re.sub("alpha_(\d{1,2})", "invExtrConsts[\g<1>]", poly_str_fixed)
    poly_str_fixed = re.sub("polyFromMatrix_(\d{1,2})", "polyFromMatrix[\g<1>]", poly_str_fixed)
    poly_str_fixed = re.sub("Transformator_(\d{1,2})", "Transformator[\g<1>]", poly_str_fixed)
    poly_str_fixed = re.sub("ROOT_HASH_1(\d{1,2})", "RootHash[1\g<1>]", poly_str_fixed)
    poly_str_fixed = re.sub("ROOT_HASH_0(\d{1,2})", "RootHash[\g<1>]", poly_str_fixed)
    poly_str_fixed = re.sub("MixColMat_(\d{1,2})_(\d{1,2})", "MixColMat[\g<1>][\g<2>]", poly_str_fixed)
    poly_str_fixed = re.sub("values_0(\d{1,2})_(\d{1,2})", "values[\g<1>][\g<2>]", poly_str_fixed)
    poly_str_fixed = re.sub("values_1(\d{1,2})_(\d{1,2})", "values[1\g<1>][\g<2>]", poly_str_fixed)
    
    
    poly_str_fixed = re.compile("(?<![\w\d\_[])1").sub("ONE",poly_str_fixed)

    cpp_eval_func =""" 
        Algebra::FieldElement eval(const std::vector<FieldElement>& x, const std::vector<FieldElement> RootHash,
            const FieldElement values[][2], FieldElement last_pow)const{

        const FieldElement& K00 = x[AES160LOCIhashchain::reg::K00];
        const FieldElement& K01 = x[AES160LOCIhashchain::reg::K01];
        const FieldElement& K02 = x[AES160LOCIhashchain::reg::K02];
        const FieldElement& K03 = x[AES160LOCIhashchain::reg::K03];
        const FieldElement& K04 = x[AES160LOCIhashchain::reg::K04];
        const FieldElement& K10 = x[AES160LOCIhashchain::reg::K10];
        const FieldElement& K11 = x[AES160LOCIhashchain::reg::K11];
        const FieldElement& K12 = x[AES160LOCIhashchain::reg::K12];
        const FieldElement& K13 = x[AES160LOCIhashchain::reg::K13];
        const FieldElement& K14 = x[AES160LOCIhashchain::reg::K14];
        const FieldElement& K20 = x[AES160LOCIhashchain::reg::K20];
        const FieldElement& K21 = x[AES160LOCIhashchain::reg::K21];
        const FieldElement& K22 = x[AES160LOCIhashchain::reg::K22];
        const FieldElement& K23 = x[AES160LOCIhashchain::reg::K23];
        const FieldElement& K24 = x[AES160LOCIhashchain::reg::K24];
        const FieldElement& K30 = x[AES160LOCIhashchain::reg::K30];
        const FieldElement& K31 = x[AES160LOCIhashchain::reg::K31];
        const FieldElement& K32 = x[AES160LOCIhashchain::reg::K32];
        const FieldElement& K33 = x[AES160LOCIhashchain::reg::K33];
        const FieldElement& K34 = x[AES160LOCIhashchain::reg::K34];

        const FieldElement& X00 = x[AES160LOCIhashchain::reg::X00];
        const FieldElement& X01 = x[AES160LOCIhashchain::reg::X01];
        const FieldElement& X02 = x[AES160LOCIhashchain::reg::X02];
        const FieldElement& X03 = x[AES160LOCIhashchain::reg::X03];
        const FieldElement& X04 = x[AES160LOCIhashchain::reg::X04];
        const FieldElement& X10 = x[AES160LOCIhashchain::reg::X10];
        const FieldElement& X11 = x[AES160LOCIhashchain::reg::X11];
        const FieldElement& X12 = x[AES160LOCIhashchain::reg::X12];
        const FieldElement& X13 = x[AES160LOCIhashchain::reg::X13];
        const FieldElement& X14 = x[AES160LOCIhashchain::reg::X14];
        const FieldElement& X20 = x[AES160LOCIhashchain::reg::X20];
        const FieldElement& X21 = x[AES160LOCIhashchain::reg::X21];
        const FieldElement& X22 = x[AES160LOCIhashchain::reg::X22];
        const FieldElement& X23 = x[AES160LOCIhashchain::reg::X23];
        const FieldElement& X24 = x[AES160LOCIhashchain::reg::X24];
        const FieldElement& X30 = x[AES160LOCIhashchain::reg::X30];
        const FieldElement& X31 = x[AES160LOCIhashchain::reg::X31];
        const FieldElement& X32 = x[AES160LOCIhashchain::reg::X32];
        const FieldElement& X33 = x[AES160LOCIhashchain::reg::X33];
        const FieldElement& X34 = x[AES160LOCIhashchain::reg::X34];

        const FieldElement& W11 = x[AES160LOCIhashchain::reg::W11];
        const FieldElement& W12 = x[AES160LOCIhashchain::reg::W12];
        const FieldElement& W13 = x[AES160LOCIhashchain::reg::W13];
        const FieldElement& W21 = x[AES160LOCIhashchain::reg::W21];
        const FieldElement& W22 = x[AES160LOCIhashchain::reg::W22];
        const FieldElement& W23 = x[AES160LOCIhashchain::reg::W23];
        const FieldElement& W31 = x[AES160LOCIhashchain::reg::W31];
        const FieldElement& W32 = x[AES160LOCIhashchain::reg::W32];
        const FieldElement& W33 = x[AES160LOCIhashchain::reg::W33];
        const FieldElement& W41 = x[AES160LOCIhashchain::reg::W41];
        const FieldElement& W42 = x[AES160LOCIhashchain::reg::W42];
        const FieldElement& W43 = x[AES160LOCIhashchain::reg::W43];
        const FieldElement& W51 = x[AES160LOCIhashchain::reg::W51];
        const FieldElement& W52 = x[AES160LOCIhashchain::reg::W52];
        const FieldElement& W53 = x[AES160LOCIhashchain::reg::W53];

        const FieldElement& inv1 = x[AES160LOCIhashchain::reg::inv1];
        const FieldElement& inv2 = x[AES160LOCIhashchain::reg::inv2];
        const FieldElement& inv3 = x[AES160LOCIhashchain::reg::inv3];
        const FieldElement& inv4 = x[AES160LOCIhashchain::reg::inv4];
        const FieldElement& inv5 = x[AES160LOCIhashchain::reg::inv5];

        const FieldElement& STATE = x[AES160LOCIhashchain::reg::STATE];
        const FieldElement& PHASE = x[AES160LOCIhashchain::reg::PHASE];
        const FieldElement& MATCH = x[AES160LOCIhashchain::reg::MATCH];
        const FieldElement& AUX_REG = x[AES160LOCIhashchain::reg::AUX_REG];
        const FieldElement& PartialMATCH = x[AES160LOCIhashchain::reg::PartialMATCH];
        const FieldElement& isSecondPhaseComparingLOCI = x[AES160LOCIhashchain::reg::isSecondPhaseComparingLOCI];
        const FieldElement& FLAG1 = x[AES160LOCIhashchain::reg::FLAG1];
        const FieldElement& FLAG2 = x[AES160LOCIhashchain::reg::FLAG2];
        const FieldElement& RC = x[AES160LOCIhashchain::reg::RC];
        const FieldElement& A = x[AES160LOCIhashchain::reg::A];
        const FieldElement& B = x[AES160LOCIhashchain::reg::B];
        const FieldElement& C = x[AES160LOCIhashchain::reg::C];
        const FieldElement& K = x[AES160LOCIhashchain::reg::K];
        const FieldElement& invRC = x[AES160LOCIhashchain::reg::invRC];

        const FieldElement& K00_next = x[AES160LOCIhashchain::reg::K00 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K01_next = x[AES160LOCIhashchain::reg::K01 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K02_next = x[AES160LOCIhashchain::reg::K02 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K03_next = x[AES160LOCIhashchain::reg::K03 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K04_next = x[AES160LOCIhashchain::reg::K04 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K10_next = x[AES160LOCIhashchain::reg::K10 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K11_next = x[AES160LOCIhashchain::reg::K11 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K12_next = x[AES160LOCIhashchain::reg::K12 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K13_next = x[AES160LOCIhashchain::reg::K13 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K14_next = x[AES160LOCIhashchain::reg::K14 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K20_next = x[AES160LOCIhashchain::reg::K20 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K21_next = x[AES160LOCIhashchain::reg::K21 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K22_next = x[AES160LOCIhashchain::reg::K22 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K23_next = x[AES160LOCIhashchain::reg::K23 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K24_next = x[AES160LOCIhashchain::reg::K24 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K30_next = x[AES160LOCIhashchain::reg::K30 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K31_next = x[AES160LOCIhashchain::reg::K31 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K32_next = x[AES160LOCIhashchain::reg::K32 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K33_next = x[AES160LOCIhashchain::reg::K33 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K34_next = x[AES160LOCIhashchain::reg::K34 + AES160LOCIhashchain::NUMREGS];

        const FieldElement& X00_next = x[AES160LOCIhashchain::reg::X00 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X01_next = x[AES160LOCIhashchain::reg::X01 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X02_next = x[AES160LOCIhashchain::reg::X02 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X03_next = x[AES160LOCIhashchain::reg::X03 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X04_next = x[AES160LOCIhashchain::reg::X04 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X10_next = x[AES160LOCIhashchain::reg::X10 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X11_next = x[AES160LOCIhashchain::reg::X11 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X12_next = x[AES160LOCIhashchain::reg::X12 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X13_next = x[AES160LOCIhashchain::reg::X13 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X14_next = x[AES160LOCIhashchain::reg::X14 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X20_next = x[AES160LOCIhashchain::reg::X20 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X21_next = x[AES160LOCIhashchain::reg::X21 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X22_next = x[AES160LOCIhashchain::reg::X22 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X23_next = x[AES160LOCIhashchain::reg::X23 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X24_next = x[AES160LOCIhashchain::reg::X24 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X30_next = x[AES160LOCIhashchain::reg::X30 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X31_next = x[AES160LOCIhashchain::reg::X31 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X32_next = x[AES160LOCIhashchain::reg::X32 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X33_next = x[AES160LOCIhashchain::reg::X33 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& X34_next = x[AES160LOCIhashchain::reg::X34 + AES160LOCIhashchain::NUMREGS];

        const FieldElement& W11_next = x[AES160LOCIhashchain::reg::W11 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W12_next = x[AES160LOCIhashchain::reg::W12 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W13_next = x[AES160LOCIhashchain::reg::W13 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W21_next = x[AES160LOCIhashchain::reg::W21 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W22_next = x[AES160LOCIhashchain::reg::W22 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W23_next = x[AES160LOCIhashchain::reg::W23 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W31_next = x[AES160LOCIhashchain::reg::W31 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W32_next = x[AES160LOCIhashchain::reg::W32 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W33_next = x[AES160LOCIhashchain::reg::W33 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W41_next = x[AES160LOCIhashchain::reg::W41 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W42_next = x[AES160LOCIhashchain::reg::W42 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W43_next = x[AES160LOCIhashchain::reg::W43 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W51_next = x[AES160LOCIhashchain::reg::W51 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W52_next = x[AES160LOCIhashchain::reg::W52 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& W53_next = x[AES160LOCIhashchain::reg::W53 + AES160LOCIhashchain::NUMREGS];
        
        const FieldElement& inv1_next = x[AES160LOCIhashchain::reg::inv1 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& inv2_next = x[AES160LOCIhashchain::reg::inv2 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& inv3_next = x[AES160LOCIhashchain::reg::inv3 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& inv4_next = x[AES160LOCIhashchain::reg::inv4 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& inv5_next = x[AES160LOCIhashchain::reg::inv5 + AES160LOCIhashchain::NUMREGS];
        
        const FieldElement& STATE_next = x[AES160LOCIhashchain::reg::STATE + AES160LOCIhashchain::NUMREGS];
        const FieldElement& PHASE_next = x[AES160LOCIhashchain::reg::PHASE + AES160LOCIhashchain::NUMREGS];
        const FieldElement& MATCH_next = x[AES160LOCIhashchain::reg::MATCH + AES160LOCIhashchain::NUMREGS];
        const FieldElement& AUX_REG_next = x[AES160LOCIhashchain::reg::AUX_REG + AES160LOCIhashchain::NUMREGS];
        const FieldElement& PartialMATCH_next = x[AES160LOCIhashchain::reg::PartialMATCH + AES160LOCIhashchain::NUMREGS];
        const FieldElement& isSecondPhaseComparingLOCI_next = x[AES160LOCIhashchain::reg::isSecondPhaseComparingLOCI + AES160LOCIhashchain::NUMREGS];
        const FieldElement& FLAG1_next = x[AES160LOCIhashchain::reg::FLAG1 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& FLAG2_next = x[AES160LOCIhashchain::reg::FLAG2 + AES160LOCIhashchain::NUMREGS];
        const FieldElement& RC_next = x[AES160LOCIhashchain::reg::RC + AES160LOCIhashchain::NUMREGS];
        const FieldElement& A_next = x[AES160LOCIhashchain::reg::A + AES160LOCIhashchain::NUMREGS];
        const FieldElement& B_next = x[AES160LOCIhashchain::reg::B + AES160LOCIhashchain::NUMREGS];
        const FieldElement& C_next = x[AES160LOCIhashchain::reg::C + AES160LOCIhashchain::NUMREGS];
        const FieldElement& K_next = x[AES160LOCIhashchain::reg::K + AES160LOCIhashchain::NUMREGS];
        const FieldElement& invRC_next = x[AES160LOCIhashchain::reg::invRC + AES160LOCIhashchain::NUMREGS];

        FieldElement tval = """ + poly_str_fixed + """
        return tval;
    }
    """

    return cpp_eval_func


def poly_to_cpp_class(poly_str,class_name):
#        """ + poly_to_cpp_is_used_var(poly_str) + """
    classContent = "class " + class_name + """ : public PolynomialInterface{
        public:
          """ + class_name + "() {};" +  """
        """ + poly_to_cpp_eval(poly_str) + """
        """ + poly_to_cpp_is_used_var(poly_str) + """
        """ + get_poly_degree_bound_cpp(poly_str) + """

        std::unique_ptr<PolynomialInterface> clone()const{
            return std::unique_ptr<PolynomialInterface>(new """+class_name+""");
        }

    };
    """

    return classContent


def create_cpp_file_withInteractivness(row_file_path):
    
    with open (row_file_path, "r") as row_file:
        row_str = re.sub(re.compile("//.*?\n" ) ,"" , row_file.read()) #delete c++ style comments
        row_str = row_str.replace(' + 0','').replace('\n','').replace(' ','').replace('\t','').replace('&','+') #last for no rand_coeffs
    
    polys = get_polys_from_constraints(row_str)
    
    theFile = """
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
        using AES160LOCIhashchain::consts::polyFromMatrix;
        using AES160LOCIhashchain::consts::MixColMat;
        using AES160LOCIhashchain::consts::Transformator;
        using AES160LOCIhashchain::consts::minus1;
        using AES160LOCIhashchain::consts::B_Transformed;
        using AES160LOCIhashchain::consts::xFETransformed;
        using AES160LOCIhashchain::consts::Rcon_round11;
		
        const FieldElement ONE = mapIntegerToFieldElement(0,1,1);
        const FieldElement X = mapIntegerToFieldElement(1,1,1);
        
        const FieldElement X_aes02 =mapIntegerToFieldElement(2,1,1);
        const FieldElement X_aes03 =mapIntegerToFieldElement(3,1,1);
        const FieldElement X_aes04 =mapIntegerToFieldElement(4,1,1);
        const FieldElement X_aes05 =mapIntegerToFieldElement(5,1,1);
        const FieldElement X_aes06 =mapIntegerToFieldElement(6,1,1);
        const FieldElement X_aes07 =mapIntegerToFieldElement(7,1,1);
        const FieldElement X_aes08 =mapIntegerToFieldElement(8,1,1);
        const FieldElement X_aes09 =mapIntegerToFieldElement(9,1,1);
        const FieldElement X_aes10 =mapIntegerToFieldElement(10,1,1);
        const FieldElement X_aes11 =mapIntegerToFieldElement(11,1,1);
        const FieldElement X_aes12 =mapIntegerToFieldElement(12,1,1);
        const FieldElement X_aes13 =mapIntegerToFieldElement(13,1,1);
        const FieldElement X_aes14 =mapIntegerToFieldElement(14,1,1);
        const FieldElement X_aes15 =mapIntegerToFieldElement(15,1,1);
        const FieldElement X_aes16 =mapIntegerToFieldElement(16,1,1);
        const FieldElement X_aes17 =mapIntegerToFieldElement(17,1,1);
        const FieldElement X_aes18 =mapIntegerToFieldElement(18,1,1);
        const FieldElement X_aes19 =mapIntegerToFieldElement(19,1,1);
        const FieldElement X_aes20 =mapIntegerToFieldElement(20,1,1);
        const FieldElement X_aes21 =mapIntegerToFieldElement(21,1,1);
        const FieldElement X_aes22 =mapIntegerToFieldElement(22,1,1);
        const FieldElement X_aes23 =mapIntegerToFieldElement(23,1,1);
        const FieldElement X_aes24 =mapIntegerToFieldElement(24,1,1);
        const FieldElement X_aes25 =mapIntegerToFieldElement(25,1,1);
        const FieldElement X_aes26 =mapIntegerToFieldElement(26,1,1);
        const FieldElement X_aes27 =mapIntegerToFieldElement(27,1,1);
        const FieldElement X_aes28 =mapIntegerToFieldElement(28,1,1);
        const FieldElement X_aes29 =mapIntegerToFieldElement(29,1,1);
        const FieldElement X_aes30 =mapIntegerToFieldElement(30,1,1);
        const FieldElement X_aes31 =mapIntegerToFieldElement(31,1,1);
        const FieldElement X_aes32 =mapIntegerToFieldElement(32,1,1);
        const FieldElement X_aes33 =mapIntegerToFieldElement(33,1,1);
        const FieldElement X_aes34 =mapIntegerToFieldElement(34,1,1);
        const FieldElement X_aes35 =mapIntegerToFieldElement(35,1,1);
        const FieldElement X_aes36 =mapIntegerToFieldElement(36,1,1);
        const FieldElement X_aes37 =mapIntegerToFieldElement(37,1,1);
        const FieldElement X_aes38 =mapIntegerToFieldElement(38,1,1);
        const FieldElement X_aes39 =mapIntegerToFieldElement(39,1,1);
        const FieldElement X_aes40 =mapIntegerToFieldElement(40,1,1);
        const FieldElement X_aes41 =mapIntegerToFieldElement(41,1,1);
        const FieldElement X_aes42 =mapIntegerToFieldElement(42,1,1);
        const FieldElement X_aes43 =mapIntegerToFieldElement(43,1,1);
        const FieldElement X_aes44 =mapIntegerToFieldElement(44,1,1);
        const FieldElement X_aes45 =mapIntegerToFieldElement(45,1,1);
        const FieldElement X_aes46 =mapIntegerToFieldElement(46,1,1);
        const FieldElement X_aes47 =mapIntegerToFieldElement(47,1,1);
        const FieldElement X_aes48 =mapIntegerToFieldElement(48,1,1);

          class randomElements_t{
                public:
                  FieldElement vals[""" + str(len(polys)) + """];
                randomElements_t(){
                      Algebra::rng.seed(123);
    """


    
    thePoly=""    
    polyId = 0
    isFirst = True
    for p in polys:
        theFile += "vals[%d] = ttgenRand;\n"%polyId
        if(not isFirst):
            thePoly+="tval += "
        isFirst = False
        thePoly += "randomElements.vals[%d]*(%s);\n"%(polyId,p)
        polyId += 1
    
    theFile += """}
    }randomElements;
    """    
    theFile += poly_to_cpp_class(thePoly,"polyAES160LOCIhashchain_class") + "\n"

    theFile += """ } //anonymous namespace for polys
    
    AES160LOCIhashchain_CS::AES160LOCIhashchain_CS(){
    """
    
    theFile += "polys_.insert(polyPtr_t(new polyAES160LOCIhashchain_class()));\n"
    
    theFile += """
    }
    
    } // ACSP_FOR_AES160LOCIhashchain namespace
    } // stark_dpm namespace
    """

    return theFile



if __name__ == "__main__":
    print(create_cpp_file_withInteractivness("constraints_ACSPforLOCI_AES160HashChain.txt"))
