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

    poly_str_fixed = poly_str_fixed.replace("INPUT1","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("INPUT2","xxdeg")
    poly_str_fixed = poly_str_fixed.replace("INPUT3","xxdeg")
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
        const ttdeg ST3_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::ST3 + AES160hashcLOCI::NUMREGS]);
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
        const ttdeg invRC_next = ttdeg(inputDegrees[AES160hashcLOCI::reg::invRC + AES160hashcLOCI::NUMREGS]);

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

    poly_str_fixed = poly_str_fixed.replace("INPUT1","")
    poly_str_fixed = poly_str_fixed.replace("INPUT2","")
    poly_str_fixed = poly_str_fixed.replace("INPUT3","")
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
    res_cpp += "case AES160hashcLOCI::reg::K00 : return " + str("K00" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K01 : return " + str("K01" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K02 : return " + str("K02" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K03 : return " + str("K03" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K04 : return " + str("K04" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K10 : return " + str("K10" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K11 : return " + str("K11" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K12 : return " + str("K12" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K13 : return " + str("K13" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K14 : return " + str("K14" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K20 : return " + str("K20" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K21 : return " + str("K21" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K22 : return " + str("K22" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K23 : return " + str("K23" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K24 : return " + str("K24" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K30 : return " + str("K30" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K31 : return " + str("K31" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K32 : return " + str("K32" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K33 : return " + str("K33" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K34 : return " + str("K34" in usedVars).lower() +";\n"

    res_cpp += "case AES160hashcLOCI::reg::B00 : return " + str("B00" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B01 : return " + str("B01" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B02 : return " + str("B02" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B03 : return " + str("B03" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B04 : return " + str("B04" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B10 : return " + str("B10" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B11 : return " + str("B11" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B12 : return " + str("B12" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B13 : return " + str("B13" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B14 : return " + str("B14" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B20 : return " + str("B20" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B21 : return " + str("B21" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B22 : return " + str("B22" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B23 : return " + str("B23" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B24 : return " + str("B24" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B30 : return " + str("B30" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B31 : return " + str("B31" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B32 : return " + str("B32" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B33 : return " + str("B33" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B34 : return " + str("B34" in usedVars).lower() +";\n"

    res_cpp += "case AES160hashcLOCI::reg::W11 : return " + str("W11" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W12 : return " + str("W12" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W13 : return " + str("W13" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W21 : return " + str("W21" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W22 : return " + str("W22" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W23 : return " + str("W23" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W31 : return " + str("W31" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W32 : return " + str("W32" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W33 : return " + str("W33" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W41 : return " + str("W41" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W42 : return " + str("W42" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W43 : return " + str("W43" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W51 : return " + str("W51" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W52 : return " + str("W52" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::W53 : return " + str("W53" in usedVars).lower() +";\n"
    
    res_cpp += "case AES160hashcLOCI::reg::inv1 : return " + str("inv1" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::inv2 : return " + str("inv2" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::inv3 : return " + str("inv3" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::inv4 : return " + str("inv4" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::inv5 : return " + str("inv5" in usedVars).lower() +";\n"
    
    res_cpp += "case AES160hashcLOCI::reg::STATE : return " + str("STATE" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::PHASE : return " + str("PHASE" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::MATCH : return " + str("MATCH" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::ST2 : return " + str("ST2" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::ST3 : return " + str("ST3" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::L1 : return " + str("L1" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::L2 : return " + str("L2" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::L3 : return " + str("L3" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::L4 : return " + str("L4" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::L5 : return " + str("L5" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::L6 : return " + str("L6" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::PartialMATCH : return " + str("PartialMATCH" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::isSecondPhaseComparingLOCI : return " + str("isSecondPhaseComparingLOCI" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::FLAG1 : return " + str("FLAG1" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::FLAG2 : return " + str("FLAG2" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::RC : return " + str("RC" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::A : return " + str("A" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::B : return " + str("B" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::C : return " + str("C" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::K : return " + str("K" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::reg::invRC : return " + str("invRC" in usedVars).lower() +";\n"
    
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K00 : return " + str("K00_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K01 : return " + str("K01_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K02 : return " + str("K02_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K03 : return " + str("K03_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K04 : return " + str("K04_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K10 : return " + str("K10_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K11 : return " + str("K11_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K12 : return " + str("K12_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K13 : return " + str("K13_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K14 : return " + str("K14_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K20 : return " + str("K20_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K21 : return " + str("K21_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K22 : return " + str("K22_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K23 : return " + str("K23_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K24 : return " + str("K24_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K30 : return " + str("K30_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K31 : return " + str("K31_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K32 : return " + str("K32_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K33 : return " + str("K33_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K34 : return " + str("K34_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B00 : return " + str("B00_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B01 : return " + str("B01_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B02 : return " + str("B02_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B03 : return " + str("B03_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B04 : return " + str("B04_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B10 : return " + str("B10_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B11 : return " + str("B11_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B12 : return " + str("B12_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B13 : return " + str("B13_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B14 : return " + str("B14_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B20 : return " + str("B20_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B21 : return " + str("B21_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B22 : return " + str("B22_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B23 : return " + str("B23_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B24 : return " + str("B24_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B30 : return " + str("B30_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B31 : return " + str("B31_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B32 : return " + str("B32_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B33 : return " + str("B33_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B34 : return " + str("B34_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W11 : return " + str("W11_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W12 : return " + str("W12_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W13 : return " + str("W13_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W21 : return " + str("W21_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W22 : return " + str("W22_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W23 : return " + str("W23_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W31 : return " + str("W31_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W32 : return " + str("W32_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W33 : return " + str("W33_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W41 : return " + str("W41_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W42 : return " + str("W42_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W43 : return " + str("W43_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W51 : return " + str("W51_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W52 : return " + str("W52_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::W53 : return " + str("W53_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::inv1 : return " + str("inv1_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::inv2 : return " + str("inv2_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::inv3 : return " + str("inv3_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::inv4 : return " + str("inv4_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::inv5 : return " + str("inv5_next" in usedVars).lower() +";\n"

    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::STATE : return " + str("STATE_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::PHASE : return " + str("PHASE_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::MATCH : return " + str("MATCH_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::ST2 : return " + str("ST2_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::ST3 : return " + str("ST3_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::L1 : return " + str("L1_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::L2 : return " + str("L2_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::L3 : return " + str("L3_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::L4 : return " + str("L4_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::L5 : return " + str("L5_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::L6 : return " + str("L6_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::PartialMATCH : return " + str("PartialMATCH_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::isSecondPhaseComparingLOCI : return " + str("isSecondPhaseComparingLOCI_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::FLAG1 : return " + str("FLAG1_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::FLAG2 : return " + str("FLAG2_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::RC : return " + str("RC_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::A : return " + str("A_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::B : return " + str("B_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::C : return " + str("C_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::K : return " + str("K_next" in usedVars).lower() +";\n"
    res_cpp += "case AES160hashcLOCI::NUMREGS + AES160hashcLOCI::reg::invRC : return " + str("invRC_next" in usedVars).lower() +";\n"
    res_cpp += """
            default: return false;
        }
    }"""

    return res_cpp

def poly_to_cpp_eval(poly_str):
    poly_str_fixed = poly_str.replace("X^minus1","AES160hashcLOCI::consts::xFEinv")

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

        const FieldElement& K00 = x[AES160hashcLOCI::reg::K00];
        const FieldElement& K01 = x[AES160hashcLOCI::reg::K01];
        const FieldElement& K02 = x[AES160hashcLOCI::reg::K02];
        const FieldElement& K03 = x[AES160hashcLOCI::reg::K03];
        const FieldElement& K04 = x[AES160hashcLOCI::reg::K04];
        const FieldElement& K10 = x[AES160hashcLOCI::reg::K10];
        const FieldElement& K11 = x[AES160hashcLOCI::reg::K11];
        const FieldElement& K12 = x[AES160hashcLOCI::reg::K12];
        const FieldElement& K13 = x[AES160hashcLOCI::reg::K13];
        const FieldElement& K14 = x[AES160hashcLOCI::reg::K14];
        const FieldElement& K20 = x[AES160hashcLOCI::reg::K20];
        const FieldElement& K21 = x[AES160hashcLOCI::reg::K21];
        const FieldElement& K22 = x[AES160hashcLOCI::reg::K22];
        const FieldElement& K23 = x[AES160hashcLOCI::reg::K23];
        const FieldElement& K24 = x[AES160hashcLOCI::reg::K24];
        const FieldElement& K30 = x[AES160hashcLOCI::reg::K30];
        const FieldElement& K31 = x[AES160hashcLOCI::reg::K31];
        const FieldElement& K32 = x[AES160hashcLOCI::reg::K32];
        const FieldElement& K33 = x[AES160hashcLOCI::reg::K33];
        const FieldElement& K34 = x[AES160hashcLOCI::reg::K34];

        const FieldElement& B00 = x[AES160hashcLOCI::reg::B00];
        const FieldElement& B01 = x[AES160hashcLOCI::reg::B01];
        const FieldElement& B02 = x[AES160hashcLOCI::reg::B02];
        const FieldElement& B03 = x[AES160hashcLOCI::reg::B03];
        const FieldElement& B04 = x[AES160hashcLOCI::reg::B04];
        const FieldElement& B10 = x[AES160hashcLOCI::reg::B10];
        const FieldElement& B11 = x[AES160hashcLOCI::reg::B11];
        const FieldElement& B12 = x[AES160hashcLOCI::reg::B12];
        const FieldElement& B13 = x[AES160hashcLOCI::reg::B13];
        const FieldElement& B14 = x[AES160hashcLOCI::reg::B14];
        const FieldElement& B20 = x[AES160hashcLOCI::reg::B20];
        const FieldElement& B21 = x[AES160hashcLOCI::reg::B21];
        const FieldElement& B22 = x[AES160hashcLOCI::reg::B22];
        const FieldElement& B23 = x[AES160hashcLOCI::reg::B23];
        const FieldElement& B24 = x[AES160hashcLOCI::reg::B24];
        const FieldElement& B30 = x[AES160hashcLOCI::reg::B30];
        const FieldElement& B31 = x[AES160hashcLOCI::reg::B31];
        const FieldElement& B32 = x[AES160hashcLOCI::reg::B32];
        const FieldElement& B33 = x[AES160hashcLOCI::reg::B33];
        const FieldElement& B34 = x[AES160hashcLOCI::reg::B34];

        const FieldElement& W11 = x[AES160hashcLOCI::reg::W11];
        const FieldElement& W12 = x[AES160hashcLOCI::reg::W12];
        const FieldElement& W13 = x[AES160hashcLOCI::reg::W13];
        const FieldElement& W21 = x[AES160hashcLOCI::reg::W21];
        const FieldElement& W22 = x[AES160hashcLOCI::reg::W22];
        const FieldElement& W23 = x[AES160hashcLOCI::reg::W23];
        const FieldElement& W31 = x[AES160hashcLOCI::reg::W31];
        const FieldElement& W32 = x[AES160hashcLOCI::reg::W32];
        const FieldElement& W33 = x[AES160hashcLOCI::reg::W33];
        const FieldElement& W41 = x[AES160hashcLOCI::reg::W41];
        const FieldElement& W42 = x[AES160hashcLOCI::reg::W42];
        const FieldElement& W43 = x[AES160hashcLOCI::reg::W43];
        const FieldElement& W51 = x[AES160hashcLOCI::reg::W51];
        const FieldElement& W52 = x[AES160hashcLOCI::reg::W52];
        const FieldElement& W53 = x[AES160hashcLOCI::reg::W53];

        const FieldElement& inv1 = x[AES160hashcLOCI::reg::inv1];
        const FieldElement& inv2 = x[AES160hashcLOCI::reg::inv2];
        const FieldElement& inv3 = x[AES160hashcLOCI::reg::inv3];
        const FieldElement& inv4 = x[AES160hashcLOCI::reg::inv4];
        const FieldElement& inv5 = x[AES160hashcLOCI::reg::inv5];

        const FieldElement& STATE = x[AES160hashcLOCI::reg::STATE];
        const FieldElement& PHASE = x[AES160hashcLOCI::reg::PHASE];
        const FieldElement& MATCH = x[AES160hashcLOCI::reg::MATCH];
        const FieldElement& ST2 = x[AES160hashcLOCI::reg::ST2];
        const FieldElement& ST3 = x[AES160hashcLOCI::reg::ST3];
        const FieldElement& L1 = x[AES160hashcLOCI::reg::L1];
        const FieldElement& L2 = x[AES160hashcLOCI::reg::L2];
        const FieldElement& L3 = x[AES160hashcLOCI::reg::L3];
        const FieldElement& L4 = x[AES160hashcLOCI::reg::L4];
        const FieldElement& L5 = x[AES160hashcLOCI::reg::L5];
        const FieldElement& L6 = x[AES160hashcLOCI::reg::L6];
        const FieldElement& PartialMATCH = x[AES160hashcLOCI::reg::PartialMATCH];
        const FieldElement& isSecondPhaseComparingLOCI = x[AES160hashcLOCI::reg::isSecondPhaseComparingLOCI];
        const FieldElement& FLAG1 = x[AES160hashcLOCI::reg::FLAG1];
        const FieldElement& FLAG2 = x[AES160hashcLOCI::reg::FLAG2];
        const FieldElement& RC = x[AES160hashcLOCI::reg::RC];
        const FieldElement& A = x[AES160hashcLOCI::reg::A];
        const FieldElement& B = x[AES160hashcLOCI::reg::B];
        const FieldElement& C = x[AES160hashcLOCI::reg::C];
        const FieldElement& K = x[AES160hashcLOCI::reg::K];
        const FieldElement& invRC = x[AES160hashcLOCI::reg::invRC];

        const FieldElement& K00_next = x[AES160hashcLOCI::reg::K00 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K01_next = x[AES160hashcLOCI::reg::K01 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K02_next = x[AES160hashcLOCI::reg::K02 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K03_next = x[AES160hashcLOCI::reg::K03 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K04_next = x[AES160hashcLOCI::reg::K04 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K10_next = x[AES160hashcLOCI::reg::K10 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K11_next = x[AES160hashcLOCI::reg::K11 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K12_next = x[AES160hashcLOCI::reg::K12 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K13_next = x[AES160hashcLOCI::reg::K13 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K14_next = x[AES160hashcLOCI::reg::K14 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K20_next = x[AES160hashcLOCI::reg::K20 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K21_next = x[AES160hashcLOCI::reg::K21 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K22_next = x[AES160hashcLOCI::reg::K22 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K23_next = x[AES160hashcLOCI::reg::K23 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K24_next = x[AES160hashcLOCI::reg::K24 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K30_next = x[AES160hashcLOCI::reg::K30 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K31_next = x[AES160hashcLOCI::reg::K31 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K32_next = x[AES160hashcLOCI::reg::K32 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K33_next = x[AES160hashcLOCI::reg::K33 + AES160hashcLOCI::NUMREGS];
        const FieldElement& K34_next = x[AES160hashcLOCI::reg::K34 + AES160hashcLOCI::NUMREGS];

        const FieldElement& B00_next = x[AES160hashcLOCI::reg::B00 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B01_next = x[AES160hashcLOCI::reg::B01 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B02_next = x[AES160hashcLOCI::reg::B02 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B03_next = x[AES160hashcLOCI::reg::B03 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B04_next = x[AES160hashcLOCI::reg::B04 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B10_next = x[AES160hashcLOCI::reg::B10 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B11_next = x[AES160hashcLOCI::reg::B11 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B12_next = x[AES160hashcLOCI::reg::B12 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B13_next = x[AES160hashcLOCI::reg::B13 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B14_next = x[AES160hashcLOCI::reg::B14 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B20_next = x[AES160hashcLOCI::reg::B20 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B21_next = x[AES160hashcLOCI::reg::B21 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B22_next = x[AES160hashcLOCI::reg::B22 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B23_next = x[AES160hashcLOCI::reg::B23 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B24_next = x[AES160hashcLOCI::reg::B24 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B30_next = x[AES160hashcLOCI::reg::B30 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B31_next = x[AES160hashcLOCI::reg::B31 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B32_next = x[AES160hashcLOCI::reg::B32 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B33_next = x[AES160hashcLOCI::reg::B33 + AES160hashcLOCI::NUMREGS];
        const FieldElement& B34_next = x[AES160hashcLOCI::reg::B34 + AES160hashcLOCI::NUMREGS];

        const FieldElement& W11_next = x[AES160hashcLOCI::reg::W11 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W12_next = x[AES160hashcLOCI::reg::W12 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W13_next = x[AES160hashcLOCI::reg::W13 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W21_next = x[AES160hashcLOCI::reg::W21 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W22_next = x[AES160hashcLOCI::reg::W22 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W23_next = x[AES160hashcLOCI::reg::W23 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W31_next = x[AES160hashcLOCI::reg::W31 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W32_next = x[AES160hashcLOCI::reg::W32 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W33_next = x[AES160hashcLOCI::reg::W33 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W41_next = x[AES160hashcLOCI::reg::W41 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W42_next = x[AES160hashcLOCI::reg::W42 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W43_next = x[AES160hashcLOCI::reg::W43 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W51_next = x[AES160hashcLOCI::reg::W51 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W52_next = x[AES160hashcLOCI::reg::W52 + AES160hashcLOCI::NUMREGS];
        const FieldElement& W53_next = x[AES160hashcLOCI::reg::W53 + AES160hashcLOCI::NUMREGS];
        
        const FieldElement& inv1_next = x[AES160hashcLOCI::reg::inv1 + AES160hashcLOCI::NUMREGS];
        const FieldElement& inv2_next = x[AES160hashcLOCI::reg::inv2 + AES160hashcLOCI::NUMREGS];
        const FieldElement& inv3_next = x[AES160hashcLOCI::reg::inv3 + AES160hashcLOCI::NUMREGS];
        const FieldElement& inv4_next = x[AES160hashcLOCI::reg::inv4 + AES160hashcLOCI::NUMREGS];
        const FieldElement& inv5_next = x[AES160hashcLOCI::reg::inv5 + AES160hashcLOCI::NUMREGS];
        
        const FieldElement& STATE_next = x[AES160hashcLOCI::reg::STATE + AES160hashcLOCI::NUMREGS];
        const FieldElement& PHASE_next = x[AES160hashcLOCI::reg::PHASE + AES160hashcLOCI::NUMREGS];
        const FieldElement& MATCH_next = x[AES160hashcLOCI::reg::MATCH + AES160hashcLOCI::NUMREGS];
        const FieldElement& ST2_next = x[AES160hashcLOCI::reg::ST2 + AES160hashcLOCI::NUMREGS];
        const FieldElement& ST3_next = x[AES160hashcLOCI::reg::ST3 + AES160hashcLOCI::NUMREGS];
        const FieldElement& L1_next = x[AES160hashcLOCI::reg::L1 + AES160hashcLOCI::NUMREGS];
        const FieldElement& L2_next = x[AES160hashcLOCI::reg::L2 + AES160hashcLOCI::NUMREGS];
        const FieldElement& L3_next = x[AES160hashcLOCI::reg::L3 + AES160hashcLOCI::NUMREGS];
        const FieldElement& L4_next = x[AES160hashcLOCI::reg::L4 + AES160hashcLOCI::NUMREGS];
        const FieldElement& L5_next = x[AES160hashcLOCI::reg::L5 + AES160hashcLOCI::NUMREGS];
        const FieldElement& L6_next = x[AES160hashcLOCI::reg::L6 + AES160hashcLOCI::NUMREGS];
        const FieldElement& PartialMATCH_next = x[AES160hashcLOCI::reg::PartialMATCH + AES160hashcLOCI::NUMREGS];
        const FieldElement& isSecondPhaseComparingLOCI_next = x[AES160hashcLOCI::reg::isSecondPhaseComparingLOCI + AES160hashcLOCI::NUMREGS];
        const FieldElement& FLAG1_next = x[AES160hashcLOCI::reg::FLAG1 + AES160hashcLOCI::NUMREGS];
        const FieldElement& FLAG2_next = x[AES160hashcLOCI::reg::FLAG2 + AES160hashcLOCI::NUMREGS];
        const FieldElement& RC_next = x[AES160hashcLOCI::reg::RC + AES160hashcLOCI::NUMREGS];
        const FieldElement& A_next = x[AES160hashcLOCI::reg::A + AES160hashcLOCI::NUMREGS];
        const FieldElement& B_next = x[AES160hashcLOCI::reg::B + AES160hashcLOCI::NUMREGS];
        const FieldElement& C_next = x[AES160hashcLOCI::reg::C + AES160hashcLOCI::NUMREGS];
        const FieldElement& K_next = x[AES160hashcLOCI::reg::K + AES160hashcLOCI::NUMREGS];
        const FieldElement& invRC_next = x[AES160hashcLOCI::reg::invRC + AES160hashcLOCI::NUMREGS];

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
    #include "AES160hashcLOCI_instance.hpp"
    #include <algebraLib/BitExtract.hpp>
    #include "languages/BREX/BrexInstance.hpp"
    #include "languages/BREX/BrexWitness.hpp"
    
    
    #define ttgenRand (Algebra::one())
    //#define ttgenRand (generateRandom())
    
    using Algebra::FieldElement;
    using Algebra::PolynomialInterface;
    using Algebra::PolynomialDegree;
    using Algebra::degreeOfProduct;
    using Algebra::one;
    using Algebra::generateRandom;
    
    namespace PCP_Project{
    namespace ACSP_FOR_AES160hashcLOCI{

    namespace { //anonymous namespace for polys and common vars
        using Algebra::mapIntegerToFieldElement;
        using AES160hashcLOCI::consts::polyFromMatrix;
        using AES160hashcLOCI::consts::MixColMat;
        using AES160hashcLOCI::consts::Transformator;
        using AES160hashcLOCI::consts::minus1;
        using AES160hashcLOCI::consts::B_Transformed;
        using AES160hashcLOCI::consts::xFETransformed;
        using AES160hashcLOCI::consts::Rcon_round11;
		
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
    theFile += poly_to_cpp_class(thePoly,"polyAES160hashcLOCI_class") + "\n"

    theFile += """ } //anonymous namespace for polys
    
    AES160hashcLOCI_CS::AES160hashcLOCI_CS(){
    """
    
    theFile += "polys_.insert(polyPtr_t(new polyAES160hashcLOCI_class()));\n"
    
    theFile += """
    }
    
    } // ACSP_FOR_AES160hashcLOCI namespace
    } // PCP_Project namespace
    """

    return theFile



if __name__ == "__main__":
    print(create_cpp_file_withInteractivness("constraints_AES160hashcLOCI.txt"))
