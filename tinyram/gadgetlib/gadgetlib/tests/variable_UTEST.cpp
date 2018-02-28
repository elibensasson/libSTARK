#include <set>
#include <gtest/gtest.h>
#include <gadgetlib2/pp.hpp>
#include <gadgetlib2/variable.hpp>

using ::std::set;

namespace PCP_Project {

TEST(ConstraintsLib, Variable) {
    Variable v1;
    EXPECT_EQ(v1.name(), "");
    Variable v2("foo");
#   ifdef DEBUG
        EXPECT_EQ(v2.name(), "foo");
#   endif
    Variable::VariableStrictOrder orderFunc;
    EXPECT_TRUE(orderFunc(v1,v2) || orderFunc(v2,v1)); // check strict ordering
    v2 = v1;
    EXPECT_FALSE(orderFunc(v1,v2) || orderFunc(v2,v1)); 
    EXPECT_EQ(v2.name(), "");
    Variable::set s1;
    s1.insert(v1);
    s1.insert(v2);
    EXPECT_EQ(s1.size(), 1);
    Variable v3;
    s1.insert(v3);
    EXPECT_EQ(s1.size(), 2);
    Variable v4;
    s1.erase(v4);
    EXPECT_EQ(s1.size(), 2);
    v4 = v1;
    s1.erase(v4);
    EXPECT_EQ(s1.size(), 1);
}

TEST(ConstraintsLib, VariableArray) {
    Variable v1;
    Variable v2("v2");
    VariableArray vArr;
    vArr.push_back(v1);
    vArr.push_back(v2);
    EXPECT_EQ(vArr.size(),2);
    Variable::VariableStrictOrder orderFunc;
    EXPECT_TRUE(orderFunc(vArr[0],vArr[1]) || orderFunc(vArr[1],vArr[0])); // check strict ordering
    vArr[1] = vArr[0];
    EXPECT_FALSE(orderFunc(vArr[0],vArr[1]) || orderFunc(vArr[1],vArr[0])); // check strict ordering
    EXPECT_THROW(vArr.at(2) = v1, ::std::out_of_range);
}

TEST(ConstraintsLib, FElem_FConst) {
    initPublicParamsFromEdwardsParam();
    //FElem(const long n);
    FElem e0(long(0));
    EXPECT_TRUE(e0 == 0);
    //FElem(const int i);
    FElem e1(int(1));    
    EXPECT_TRUE(e1 == 1);
    FElem e2(2);
    EXPECT_EQ(e2, FElem(2));
    //FElem(const FElem& src);
    FElem e3(e1);
    EXPECT_TRUE(e3 == 1);
    e3 = 0;
    EXPECT_TRUE(e3 == 0);
    ASSERT_EQ(e1, 1);
    e0 = e1;
    EXPECT_EQ(e0, e1);
    e0 = 0;
    EXPECT_NE(e0, e1);
    //FElem& operator=(const FElem&& other);
    FElem e4(FElem(4));
    EXPECT_EQ(e4, FElem(4));
    //FElem& operator=(const FElem& other);
    //FElem& operator=(const long i) { *elem_ = i; return *this;}
    e0 = e1 = 42;
    EXPECT_EQ(e1, FElem(42));
    EXPECT_EQ(e0, e1);
    //::std::string asString() const {return elem_->asString();}
    #ifdef DEBUG
    EXPECT_EQ(e0.asString(),"42");
    #else // ifdef DEBUG
    EXPECT_EQ(e0.asString(),"");
    #endif // ifdef DEBUG
    //FieldType fieldType() const;
    EXPECT_EQ(e0.fieldType(), AGNOSTIC);
    //bool operator==(const FElem& other) const {return *elem_ == *other.elem_;}
    EXPECT_TRUE(e1 == e0);
    EXPECT_FALSE(e1 == e4);
    FElem eR1P = Fp(42);
    EXPECT_TRUE(e1 == eR1P);
    EXPECT_TRUE(eR1P == e1);
    //FElem& operator+=(const FElem& other) {*elem_ += *other.elem_; return *this;}
    e1 = e0 += e1;
    EXPECT_EQ(e0, FElem(84));
    EXPECT_TRUE(e1 == 84);
    //FElem& operator-=(const FElem& other) {*elem_ -= *other.elem_; return *this;}
    e1 = e0 -= e1;
    EXPECT_TRUE(e0 == 0);
    EXPECT_TRUE(e1 == 0);
    e0 = 21;
    e1 = 2;
    EXPECT_EQ(e0,FElem(21));
    EXPECT_TRUE(e1 == 2);
    //FElem& operator*=(const FElem& other) {*elem_ *= *other.elem_; return *this;}
    e1 = e0 *= e1;
    EXPECT_TRUE(e0 == 42);
    EXPECT_TRUE(e1 == 42);
    EXPECT_TRUE(e0 == e1);
    EXPECT_TRUE(e0 == 42);
    EXPECT_TRUE(42 == e0);        
    //FElem operator-() const;
    EXPECT_EQ(-e4, FElem(-4));
    //bool operator!=(const FElem& first, const FElem& second);
    EXPECT_TRUE(e4 != e0);        
    //bool operator==(const FElem& first, const long second);
    EXPECT_TRUE(e4 == 4);        
    //bool operator==(const long first, const FElem& second);
    EXPECT_TRUE(4 == e4);
    //bool operator!=(const FElem& first, const long second);
    EXPECT_TRUE(e4 != 5);
    //bool operator!=(const long first, const FElem& second);
    EXPECT_TRUE(5 != e4);
    //FElem inverse() const;
    e1 = e4.inverse(R1P);
    EXPECT_EQ(e1, FElem(Fp(e4.asLong()).inverse()));
}

TEST(ConstraintsLib, FElem_R1P_Elem) {
    initPublicParamsFromEdwardsParam();
    //FElem(const Fp& elem);
    FElem e0(Fp(0));
    EXPECT_EQ(e0,0);
    EXPECT_NE(e0,1);
    //FElem(const FElem& src);
    FElem e1(e0);
    EXPECT_EQ(e1,0);
    //FElem& operator=(const FElem& other);
    FElem  e2 = Fp(2);
    e1 = e2;
    EXPECT_EQ(e1,2);
    FElem e3 = 3;
    e1 = e3;
    EXPECT_EQ(e1,3);
    //FElem& operator=(const FElem&& other);
    e1 = FElem(Fp(2));
    EXPECT_EQ(e1,2);
    e1 = FElem(1);
    EXPECT_EQ(e1,1);
    //FElem& operator=(const long i) { *elem_ = i; return *this;}
    e1 = long(42);
    EXPECT_EQ(e1,42);
    //::std::string asString() const {return elem_->asString();}
    #ifdef DEBUG
    EXPECT_EQ(e1.asString(),"42");
    #else // ifdef DEBUG
    EXPECT_EQ(e1.asString(),"");
    #endif // ifdef DEBUG
    //FieldType fieldType() const;
    EXPECT_EQ(e1.fieldType(),R1P);
    //bool operator==(const FElem& other) const {return *elem_ == *other.elem_;}
    e0 = 42;
    EXPECT_TRUE(e0 == e1);
    EXPECT_FALSE(e0 == e2);
    EXPECT_FALSE(e0 != e1);
    EXPECT_TRUE(e0 == 42);
    EXPECT_FALSE(e0 == 41);
    EXPECT_TRUE(e0 != 41);
    EXPECT_TRUE(42 == e0);
    EXPECT_TRUE(41 != e0);
    // negative numbers
    e0 = e1 = -42;
    EXPECT_TRUE(e0 == e1);
    EXPECT_FALSE(e0 == e2);
    EXPECT_FALSE(e0 != e1);
    EXPECT_TRUE(e0 == -42);
    EXPECT_FALSE(e0 == -41);
    EXPECT_TRUE(e0 != -41);
    EXPECT_TRUE(-42 == e0);
    EXPECT_TRUE(-41 != e0);
    //FElem& operator*=(const FElem& other) {*elem_ *= *other.elem_; return *this;}
    e0 = 0;
    e1 = 1;
    e2 = 2;
    e3 = 3;
    EXPECT_TRUE(e0.fieldType() == R1P && e1.fieldType() == R1P && e2.fieldType() == R1P);
    e1 = e2 *= e3;
    EXPECT_EQ(e1,6);
    EXPECT_EQ(e2,6);
    EXPECT_EQ(e3,3);
    ////FElem& operator+=(const FElem& other) {*elem_ += *other.elem_; return *this;}
    e1 = e2 += e3;
    EXPECT_EQ(e1,9);
    EXPECT_EQ(e2,9);
    EXPECT_EQ(e3,3);
    //FElem& operator-=(const FElem& other) {*elem_ -= *other.elem_; return *this;}
    e1 = e2 -= e3;
    EXPECT_EQ(e1,6);
    EXPECT_EQ(e2,6);
    EXPECT_EQ(e3,3);
    //FElem operator-() const;
    e3 = -e2;
    EXPECT_EQ(e2,6);
    EXPECT_EQ(e3,-6);
    EXPECT_TRUE(e3.fieldType() == R1P);
    //FElem inverse() const;
    FElem e42 = Fp(42);
    EXPECT_EQ(e42.inverse(R1P),Fp(42).inverse());
}


TEST(ConstraintsLib, FElem_LD2_Elem) {
    initPublicParamsFromEdwardsParam();
    //FElem(const GF2E& elem);
    FElem e0(Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,0));
    EXPECT_EQ(e0,0);
    EXPECT_NE(e0,1);
    //FElem(const FElem& src);
    FElem e1(e0);
    EXPECT_EQ(e1,0);
    //FElem& operator=(const FElem& other);
    FElem  e2 = Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,1);
    e1 = e2;
    EXPECT_EQ(e1,1);
    FElem e3 = 0;
    e1 = e3;
    EXPECT_EQ(e1,0);
    e3 = 3;
    EXPECT_ANY_THROW(e1 = e3;);
    //FElem& operator=(const FElem&& other);
    e1 = FElem(Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,1));
    EXPECT_EQ(e1,1);
    e1 = FElem(0);
    EXPECT_EQ(e1,0);
    //FElem& operator=(const long i) { *elem_ = i; return *this;}
    e1 = long(1);
    EXPECT_EQ(e1,1);
    EXPECT_ANY_THROW(e1 = long(42));
    //::std::string asString() const {return elem_->asString();}
    EXPECT_EQ(e1.asString(),"[1]");
    e1 = 0;
    EXPECT_EQ(e1.asString(),"[]");
    //FieldType fieldType() const;
    EXPECT_EQ(e1.fieldType(),LD2);
    //bool operator==(const FElem& other) const {return *elem_ == *other.elem_;}
    e0 = 0;
    EXPECT_TRUE(e0 == e1);
    EXPECT_FALSE(e0 == e2);
    EXPECT_FALSE(e0 != e1);
    EXPECT_TRUE(e0 == 0);
    EXPECT_FALSE(e0 == 1);
    EXPECT_TRUE(e0 != 1);
    EXPECT_TRUE(0 == e0);
    EXPECT_TRUE(1 != e0);
    //FElem& operator*=(const FElem& other) {*elem_ *= *other.elem_; return *this;}
    FieldElement g0 = mapIntegerToFieldElement(0,Algebra::ExtensionDegree,1);
    FieldElement g1 = mapIntegerToFieldElement(0,Algebra::ExtensionDegree,2);
    FieldElement g2 = mapIntegerToFieldElement(0,Algebra::ExtensionDegree,4);
    FieldElement g3 = mapIntegerToFieldElement(0,Algebra::ExtensionDegree,6);
    e0 = Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,g0);
    e1 = Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,g1);
    e2 = Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,g2);
    e3 = Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,g3);
    EXPECT_EQ(e0.asString(),"[1]");
    EXPECT_EQ(e1.asString(),"[0 1]");
    EXPECT_EQ(e2.asString(),"[0 0 1]");
    EXPECT_EQ(e3.asString(),"[0 1 1]");
    e1 = e2 *= e3;
    EXPECT_EQ(e1.asString(),"[0 0 0 1 1]");
    EXPECT_EQ(e2.asString(),"[0 0 0 1 1]");
    EXPECT_EQ(e3.asString(),"[0 1 1]");
    //FElem& operator+=(const FElem& other) {*elem_ += *other.elem_; return *this;}
    e1 = e2 += e3;
    EXPECT_EQ(e1.asString(),"[0 1 1 1 1]");
    EXPECT_EQ(e2.asString(),"[0 1 1 1 1]");
    EXPECT_EQ(e3.asString(),"[0 1 1]");
    //FElem& operator-=(const FElem& other) {*elem_ -= *other.elem_; return *this;}
    e1 = e2 -= e3;
    EXPECT_EQ(e1.asString(),"[0 0 0 1 1]");
    EXPECT_EQ(e2.asString(),"[0 0 0 1 1]");
    EXPECT_EQ(e3.asString(),"[0 1 1]");
    //FElem operator-() const;
    e3 = -e2;
    EXPECT_EQ(e2.asString(),"[0 0 0 1 1]");
    EXPECT_EQ(e3.asString(),"[0 0 0 1 1]");
    //FElem inverse() const;
    e3 = e3 = Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,g3);
    EXPECT_EQ(e3.inverse(LD2), FElem(inv(Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,g3))));
    // Tests for added functionality, creating LD2_Elem from constant -1
    FElem eNeg1(-1);
    EXPECT_EQ(eNeg1, -1);
    eNeg1 = 0;
    EXPECT_NE(eNeg1, -1);
    eNeg1 = -1;
    EXPECT_TRUE(eNeg1 == -1);
}

TEST(ConstraintsLib, LinearTerm) {
    initPublicParamsFromEdwardsParam();
    //LinearTerm(const Variable& v) : variable_(v), coeff_(1) {}
    VariableArray x(10, "x");
    LinearTerm lt0(x[0]);
    VariableAssignment ass;
    ass[x[0]] = Fp(42);
    EXPECT_EQ(x[0].eval(ass), 42);
    EXPECT_NE(x[0].eval(ass), 17);
    EXPECT_EQ(lt0.eval(ass), 42);
    EXPECT_NE(lt0.eval(ass), 17);
    ass[x[0]] = Fp(2);
    EXPECT_EQ(lt0.eval(ass), 2);
    LinearTerm lt1(x[1]);
    ass[x[1]] = Algebra::mainIntegerToFieldElement(0,Algebra::ExtensionDegree,1);
    EXPECT_EQ(lt1.eval(ass), 1);
    LinearTerm lt2(x[2]);
    ass[x[2]] = 24;
    EXPECT_EQ(lt2.eval(ass), 24);
    //LinearTerm(const Variable& v, const FElem& coeff) : variable_(v), coeff_(coeff) {}
    LinearTerm lt3(x[3], Fp(3));
    ass[x[3]] = Fp(4);
    EXPECT_EQ(lt3.eval(ass), 3*4);
    LinearTerm lt4(x[4], Fp(4));
    ass[x[4]] = Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,4);
    EXPECT_ANY_THROW((lt4.eval(ass), 3*7)); // Mixed types
    //LinearTerm(const Variable& v, long n) : variable_(v), coeff_(n) {}
    LinearTerm lt5(x[5], long(2));
    ass[x[5]] = 5;
    EXPECT_EQ(lt5.eval(ass), 5*2);
    LinearTerm lt6(x[6], 2);
    ass[x[6]] = 6;
    EXPECT_EQ(lt6.eval(ass), 6*2);
    //LinearTerm operator-() const {return LinearTerm(variable_, -coeff_);}
    LinearTerm lt7 = -lt6;
    EXPECT_EQ(lt7.eval(ass), -6*2);
    //virtual FieldType fieldtype() const {return coeff_.fieldType();}
    EXPECT_EQ(lt6.fieldtype(), AGNOSTIC);
    EXPECT_EQ(lt3.fieldtype(), R1P);
    LinearTerm lt8(x[8], Algebra::one());
    EXPECT_EQ(lt8.fieldtype(), LD2);

    //virtual ::std::string asString() const;
    // R1P
#ifdef DEBUG
    LinearTerm lt10(x[0], Fp(-1));
    EXPECT_EQ(lt10.asString(), "-1 * x[0]");
    LinearTerm lt11(x[0], Fp(0));
    EXPECT_EQ(lt11.asString(), "0 * x[0]");
    LinearTerm lt12(x[0], Fp(1));
    EXPECT_EQ(lt12.asString(), "x[0]");
    LinearTerm lt13(x[0], Fp(2));
    EXPECT_EQ(lt13.asString(), "2 * x[0]");
    // LD2
    LinearTerm lt21(x[0], Algebra::zero());
    EXPECT_EQ(lt21.asString(), "0 * x[0]");
    LinearTerm lt22(x[0], Algebra::one());
    EXPECT_EQ(lt22.asString(), "x[0]");
    LinearTerm lt23(x[0], Algebra::xFE());
    EXPECT_EQ(lt23.asString(), "[0 1] * x[0]");
    // AGNOSTIC
    LinearTerm lt30(x[0], -1);
    EXPECT_EQ(lt30.asString(), "-1 * x[0]");
    LinearTerm lt31(x[0], 0);
    EXPECT_EQ(lt31.asString(), "0 * x[0]");
    LinearTerm lt32(x[0], Fp(1));
    EXPECT_EQ(lt32.asString(), "x[0]");
    LinearTerm lt33(x[0], Fp(2));
    EXPECT_EQ(lt33.asString(), "2 * x[0]");
#endif // DEBUG
    // LinearTerm& operator*=(const FElem& other);
    LinearTerm lt42(x[0], Fp(1));
    LinearTerm lt43(x[0], Fp(2));
    lt42 = lt43 *= FElem(4);
    EXPECT_EQ(lt42.eval(ass), 8*2);
    EXPECT_EQ(lt43.eval(ass), 8*2);
}

TEST(ConstraintsLib, LinearCombination) {
    initPublicParamsFromEdwardsParam();
//    LinearCombination() : linearTerms_(), constant_(0) {}
    LinearCombination lc0;
    VariableAssignment assignment;
    EXPECT_EQ(lc0.eval(assignment),0);
//    LinearCombination(const Variable& var) : linearTerms_(1,var), constant_(0) {}
    VariableArray x(10,"x");
    LinearCombination lc1(x[1]);
    assignment[x[1]] = 42;
    EXPECT_EQ(lc1.eval(assignment),42);
//    LinearCombination(const LinearTerm& linTerm) : linearTerms_(1,linTerm), constant_(0) {}
    LinearTerm lt(x[2], Fp(2));
    LinearCombination lc2 = lt;
    assignment[x[2]] = 2;
    EXPECT_EQ(lc2.eval(assignment),4);
//    LinearCombination(long i) : linearTerms_(), constant_(i) {} 
    LinearCombination lc3 = 3;
    EXPECT_EQ(lc3.eval(assignment),3);
//    LinearCombination(const FElem& elem) : linearTerms_(), constant_(elem) {}
    FElem elem = Fp(4);
    LinearCombination lc4 = elem;
    EXPECT_EQ(lc4.eval(assignment),4);
//    LinearCombination& operator+=(const LinearCombination& other);
    lc1 = lc4 += lc2;
    EXPECT_EQ(lc4.eval(assignment),4+4);
    EXPECT_EQ(lc1.eval(assignment),4+4);
    EXPECT_EQ(lc2.eval(assignment),4);
//    LinearCombination& operator-=(const LinearCombination& other);
    lc1 = lc4 -= lc3;
    EXPECT_EQ(lc4.eval(assignment),4+4-3);
    EXPECT_EQ(lc1.eval(assignment),4+4-3);
    EXPECT_EQ(lc3.eval(assignment),3);
//    ::std::string asString() const;
#   ifdef DEBUG
    EXPECT_EQ(lc1.asString(), "2 * x[2] + 1");
#   else // ifdef DEBUG
    EXPECT_EQ(lc1.asString(), "");
#   endif // ifdef DEBUG
//    Variable::set getUsedVariables() const;
    Variable::set sVar = lc1.getUsedVariables();
    EXPECT_EQ(sVar.size(),1);
    assignment[x[2]] = 83;
    EXPECT_EQ(assignment[*sVar.begin()], 83);    
    assignment[x[2]] = 2;
//  LinearCombination operator-(const LinearCombination& lc);
    lc2 = -lc1;
    EXPECT_EQ(lc2.eval(assignment),-5);
    lc2 = lc1 *= FElem(4);
    EXPECT_EQ(lc1.eval(assignment),5*4);
    EXPECT_EQ(lc2.eval(assignment),5*4);
}

TEST(ConstraintsLib, Monomial) {
    initPublicParamsFromEdwardsParam();
    //Monomial(const Variable& var) : coeff_(1), variables_(1, var) {}
    //FElem eval(const VariableAssignment& assignment) const;
    VariableArray x(10, "x");
    Monomial m0 = x[0];
    VariableAssignment assignment;
    assignment[x[0]] = 42;
    EXPECT_EQ(m0.eval(assignment), 42);
    //Monomial(const Variable& var, const FElem& coeff) : coeff_(coeff), variables_(1, var) {}
    Monomial m1(x[1], Fp(3));
    assignment[x[1]] = 2;
    EXPECT_EQ(m1.eval(assignment), 6);
    //Monomial(const FElem& val) : coeff_(val), variables_() {}
    Monomial m2 = Monomial(Algebra::one());
    EXPECT_EQ(m2.eval(assignment), 1);
    //Monomial(const LinearTerm& linearTerm);
    LinearTerm lt(x[3], 3);
    Monomial m3 = lt;
    assignment[x[3]] = 3;
    EXPECT_EQ(m3.eval(assignment), 9);    
    //Monomial operator-() const;
    Monomial m4 = -m3;
    EXPECT_EQ(m4.eval(assignment), -9);
    //Monomial& operator*=(const Monomial& other);
    m3 = m4 *= m0;
    EXPECT_EQ(m3.eval(assignment), -9 * 42);
    EXPECT_EQ(m4.eval(assignment), -9 * 42);
    EXPECT_EQ(m0.eval(assignment), 42);
    //Variable::set getUsedVariables() const;
    Variable::set varSet = m3.getUsedVariables();
    ASSERT_EQ(varSet.size(), 2);
    EXPECT_TRUE(varSet.find(x[0]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[3]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[4]) == varSet.end());
    //::std::string asString() const
#   ifdef DEBUG
    EXPECT_EQ(m3.asString(), "-3*x[0]*x[3]");
#   else
    EXPECT_EQ(m3.asString(), "");
#   endif
}


TEST(ConstraintsLib, Polynomial) {
    initPublicParamsFromEdwardsParam();
    VariableArray x(10,"x");
    VariableAssignment assignment;
    //Polynomial() : monomials_(), constant_(0) {}
    //FElem eval(const VariableAssignment& assignment) const;
    Polynomial p0;
    EXPECT_EQ(p0.eval(assignment), 0);    
    //Polynomial(const Monomial& monomial) : monomials_(1, monomial), constant_(0) {}
    Monomial m0(x[0], 3);
    Polynomial p1 = m0;
    assignment[x[0]] = 2;
    EXPECT_EQ(p1.eval(assignment), 6);
    //Polynomial(const Variable& var) : monomials_(1, Monomial(var)), constant_(0) {}
    Polynomial p2 = x[2];
    assignment[x[2]] = 2;
    EXPECT_EQ(p2.eval(assignment), 2);
    //Polynomial(const FElem& val) : constant_(val), monomials_() {}
    Polynomial p3 = FElem(Fp(3));
    EXPECT_EQ(p3.eval(assignment), 3);
    //Polynomial(const LinearCombination& linearCombination);
    LinearCombination lc(x[0]);
    lc += x[2];
    Polynomial p4 = lc;
    EXPECT_EQ(p4.eval(assignment), 4);    
    //Variable::set getUsedVariables() const;
    const Variable::set varSet = p4.getUsedVariables();
    EXPECT_EQ(varSet.size(), 2);
    EXPECT_TRUE(varSet.find(x[0]) != varSet.end());
    EXPECT_TRUE(varSet.find(x[2]) != varSet.end());
    //Polynomial& operator+=(const Polynomial& other);
    Polynomial p5 = p4 += p3;
    EXPECT_EQ(p5.eval(assignment), 7);    
    //::std::string asString() const;
#   ifdef DEBUG
    EXPECT_EQ(p0.asString(), "0");
    EXPECT_EQ(p1.asString(), "3*x[0]");
    EXPECT_EQ(p2.asString(), "x[2]");
    EXPECT_EQ(p3.asString(), "3");
    EXPECT_EQ(p4.asString(), "x[0] + x[2] + 3");
    EXPECT_EQ(p5.asString(), "x[0] + x[2] + 3");
#   endif // DEBUG
    //Polynomial& operator*=(const Polynomial& other);
    p0 = p4 *= p5;
    EXPECT_EQ(p0.eval(assignment), 7*7);
    EXPECT_EQ(p4.eval(assignment), 7*7);
    EXPECT_EQ(p5.eval(assignment), 7);
    //Polynomial& operator-=(const Polynomial& other);
    p0 = p4 -= p1;
    EXPECT_EQ(p0.eval(assignment), 7*7-6);
    EXPECT_EQ(p4.eval(assignment), 7*7-6);
    EXPECT_EQ(p1.eval(assignment), 6);
    //inline Polynomial operator-(const Polynomial& src) {return Polynomial(FElem(0)) -= src;}
    p2 = -p0;
    EXPECT_EQ(p0.eval(assignment), 7*7-6);
    EXPECT_EQ(p2.eval(assignment), -(7*7-6));
}

//
//#ifdef DEBUG
//TEST(ConstraintsLib,R1P_annotations)
//{
//    R1P_Variable v("v");
//    R1P_LinearTerm lt(v);
//    EXPECT_EQ(lt.annotation(), "v");
//    lt = 5 * lt;
//    EXPECT_EQ(lt.annotation(), "5*v");
//    R1P_Protoboard pb;
//    R1P_VariableArray arr = R1P_VariableArray::create(&pb,5,"a");
//    R1P_LinearCombination lc(lt + arr[0] + 2*arr[3] + 3*arr[4]*2);
//    EXPECT_EQ(lc.annotation(), "5*v + a[0] + 2*a[3] + 6*a[4]");
//    Rank1Constraint c(lc,v,lt, "lc*v = lt");
//    EXPECT_EQ(c.annotation(), "( 5*v + a[0] + 2*a[3] + 6*a[4] ) * ( v ) = 5*v");
//}
//#endif


} // namespace PCP_Project
