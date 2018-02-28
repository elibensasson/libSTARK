#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include <gadgetlib2/pp.hpp>
#include <gadgetlib2/protoboard.hpp>
#include <gadgetlib2/gadget.hpp>

using ::std::cerr;
using ::std::cout;
using ::std::endl;
using ::std::stringstream;

#define EXHAUSTIVE_N 4

namespace PCP_Project {

TEST(ConstraintsLib,R1P_AND_Gadget) {
    initPublicParamsFromEdwardsParam();
    auto pb = Protoboard::create(R1P);

    VariableArray x(3, "x");
    Variable y("y");
    auto andGadget = AND_Gadget::create(pb, x, y);
    andGadget->generateConstraints();

    pb->val(x[0]) = 0;
    pb->val(x[1]) = 1;
    pb->val(x[2]) = 1;
    andGadget->generateWitness();
    EXPECT_TRUE(pb->val(y) == 0);
    EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    pb->val(y) = 1;
    EXPECT_FALSE(pb->isSatisfied());

    pb->val(x[0]) = 1;
    andGadget->generateWitness();
    EXPECT_TRUE(pb->val(y) == 1);
    EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));

    pb->val(y) = 0;
    EXPECT_FALSE(pb->isSatisfied());
}

TEST(ConstraintsLib,LD2_AND_Gadget) {
    auto pb = Protoboard::create(LD2);

    VariableArray x(3,"x");
    Variable y("y");
    auto andGadget = AND_Gadget::create(pb, x, y);
    andGadget->generateConstraints();

    pb->val(x[0]) = 0;
    pb->val(x[1]) = 1;
    pb->val(x[2]) = 1;
    andGadget->generateWitness();
    EXPECT_TRUE(pb->val(y) == 0);
    EXPECT_TRUE(pb->isSatisfied());
    pb->val(y) = 1;
    EXPECT_FALSE(pb->isSatisfied());

    pb->val(x[0]) = 1;
    andGadget->generateWitness();
    EXPECT_TRUE(pb->val(y) == 1);
    EXPECT_TRUE(pb->isSatisfied());

    pb->val(y) = 0;
    EXPECT_FALSE(pb->isSatisfied());
}


void andGadgetExhaustiveTest(ProtoboardPtr pb, size_t n); // Forward declaration

TEST(ConstraintsLib,R1P_ANDGadget_Exhaustive) {
    initPublicParamsFromEdwardsParam();
    for(int n = 1; n <= EXHAUSTIVE_N; ++n) {
        SCOPED_TRACE(FMT("n = %u \n", n));
        auto pb = Protoboard::create(R1P);
        andGadgetExhaustiveTest(pb, n);
    }
}

TEST(ConstraintsLib,LD2_ANDGadget_Exhaustive) {
    for(int n = 2; n <= EXHAUSTIVE_N; ++n) {
        SCOPED_TRACE(FMT("n = %u \n", n));
        auto pb = Protoboard::create(LD2);
        andGadgetExhaustiveTest(pb, n);
    }
}

TEST(ConstraintsLib,BinaryAND_Gadget) {
    auto pb = Protoboard::create(LD2);
    Variable input1("input1");
    Variable input2("input2");
    Variable result("result");
    auto andGadget = AND_Gadget::create(pb, input1, input2, result);
    andGadget->generateConstraints();
    pb->val(input1) = pb->val(input2) = 0;
    andGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_EQ(pb->val(result), 0);
    pb->val(result) = 1;
    ASSERT_FALSE(pb->isSatisfied());
    pb->val(result) = 0;
    pb->val(input1) = 1;
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    pb->val(input2) = 1;
    ASSERT_FALSE(pb->isSatisfied());
    andGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_EQ(pb->val(result), 1);
}

void orGadgetExhaustiveTest(ProtoboardPtr pb, size_t n); // Forward declaration

TEST(ConstraintsLib,R1P_ORGadget_Exhaustive) {
    initPublicParamsFromEdwardsParam();
    for(int n = 1; n <= EXHAUSTIVE_N; ++n) {
        SCOPED_TRACE(FMT("n = %u \n", n));
        auto pb = Protoboard::create(R1P);
        orGadgetExhaustiveTest(pb, n);
    }
}

TEST(ConstraintsLib,LD2_ORGadget_Exhaustive) {
    for(int n = 2; n <= EXHAUSTIVE_N; ++n) {
        SCOPED_TRACE(FMT("n = %u \n", n));
        auto pb = Protoboard::create(LD2);
        orGadgetExhaustiveTest(pb, n);
    }
}

TEST(ConstraintsLib,BinaryOR_Gadget) {
    auto pb = Protoboard::create(LD2);
    Variable input1("input1");
    Variable input2("input2");
    Variable result("result");
    auto orGadget = OR_Gadget::create(pb, input1, input2, result);
    orGadget->generateConstraints();
    pb->val(input1) = pb->val(input2) = 0;
    orGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_EQ(pb->val(result), 0);
    pb->val(result) = 1;
    ASSERT_FALSE(pb->isSatisfied());
    pb->val(result) = 0;
    pb->val(input1) = 1;
    ASSERT_FALSE(pb->isSatisfied());
    pb->val(result) = 1;
    ASSERT_CONSTRAINTS_SATISFIED(pb);
    pb->val(input2) = 1;
    orGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_EQ(pb->val(result), 1);
}

TEST(ConstraintsLib,R1P_InnerProductGadget_Exhaustive) {
    initPublicParamsFromEdwardsParam();
    const size_t n = EXHAUSTIVE_N;
    auto pb = Protoboard::create(R1P);
    VariableArray A(n, "A");
    VariableArray B(n, "B");
    Variable result("result");
    auto g = InnerProduct_Gadget::create(pb, A, B, result);
    g->generateConstraints();
    for (size_t i = 0; i < 1u<<n; ++i) {
        for (size_t j = 0; j < 1u<<n; ++j) {
            size_t correct = 0;
            for (size_t k = 0; k < n; ++k) {
                pb->val(A[k]) = i & (1u<<k) ? 1 : 0;
                pb->val(B[k]) = j & (1u<<k) ? 1 : 0;
                correct += (i & (1u<<k)) && (j & (1u<<k)) ? 1 : 0;
            }
            g->generateWitness();
            EXPECT_EQ(pb->val(result) , FElem(correct));
            EXPECT_TRUE(pb->isSatisfied());
            // negative test
            pb->val(result) = 100*n+19;
            EXPECT_FALSE(pb->isSatisfied());
        }
    }
}

TEST(ConstraintsLib,LD2_InnerProductGadget_Exhaustive) {
    initPublicParamsFromEdwardsParam();
    const size_t n = EXHAUSTIVE_N > 1 ? EXHAUSTIVE_N -1 : EXHAUSTIVE_N;
    for(int len = 1; len <= n; ++len) {
        auto pb = Protoboard::create(LD2);
        VariableArray a(len, "a");
        VariableArray b(len, "b");
        Variable result("result");
        auto ipGadget = InnerProduct_Gadget::create(pb, a, b, result);
        ipGadget->generateConstraints();
        // Generate Inputs & Witnesses
        vec_GF2E a_vec;
        a_vec.SetLength(len);
        for(int h = 0; h < len; ++h) { // iterate over a's elements
            for(int j = 0; j < 1u<<n; ++j) { // all possible options for a[h]
                GF2X a_h;
                for(int coeff = 0; coeff < n; ++coeff) { // set a[h] coefficients
                    SetCoeff(a_h, coeff, j & 1<<coeff);
                }
                a_vec[h] = to_GF2E(a_h);
                pb->val(a[h]) = to_GF2E(a_h);
                vec_GF2E b_vec;
                b_vec.SetLength(len);
                for(int i = 0; i < len; ++i) {
                    pb->val(b[i]) = 0;
                }
                for(int i = 0; i < len; ++i) { // iterate over b's elements
                    for(int k = 0; k < 1u<<n; ++k) { // all possible options for b[i]
                        GF2X b_i;
                        for(int coeff = 0; coeff < n; ++coeff) { // set b[i] coefficients
                            SetCoeff(b_i, coeff, k & 1<<coeff);
                        }
                        b_vec[i] = to_GF2E(b_i);
                        pb->val(b[i]) = to_GF2E(b_i);
                        ipGadget->generateWitness();
                        GF2E resultGF2E;
                        InnerProduct(resultGF2E, a_vec, b_vec);
                        ::std::stringstream s;
                        s << endl << "i = " << i << endl
                          << "< " << a_vec << " > * < " << b_vec << " > = " << resultGF2E << endl
                          << pb->annotation();
                        SCOPED_TRACE(s.str());
                        EXPECT_EQ(pb->val(result), FElem(resultGF2E));
                        EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
                        // Negative test
                        pb->val(result) = resultGF2E + to_GF2E(1);
                        EXPECT_FALSE(pb->isSatisfied());
                    }
                }
            }
        }
    }
}

TEST(ConstraintsLib,R1P_LooseMUX_Gadget_Exhaustive) {
initPublicParamsFromEdwardsParam();
const size_t n = EXHAUSTIVE_N;
    auto pb = Protoboard::create(R1P);
    VariableArray arr(1<<n, "arr");
    Variable index("index");
    Variable result("result");
    Variable success_flag("success_flag");
    auto g = LooseMUX_Gadget::create(pb, arr, index, result, success_flag);
    g->generateConstraints();
    for (size_t i = 0; i < 1u<<n; ++i) {
        pb->val(arr[i]) = (19*i) % (1u<<n);
    }
    for (int idx = -1; idx <= (1<<n); ++idx) {
        pb->val(index) = idx;
        g->generateWitness();
        if (0 <= idx && idx <= (1<<n) - 1) {
            EXPECT_EQ(pb->val(result) , (19*idx) % (1u<<n));
            EXPECT_EQ(pb->val(success_flag) , 1);
            EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
            pb->val(result) -= 1;
            EXPECT_FALSE(pb->isSatisfied());
        }
        else {
            EXPECT_EQ(pb->val(success_flag) , 0);
            EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
            pb->val(success_flag) = 1;
            EXPECT_FALSE(pb->isSatisfied());
        }
    }
}

// Forward declaration
void packing_Gadget_R1P_ExhaustiveTest(ProtoboardPtr unpackingPB, ProtoboardPtr packingPB,
                                       const int n, VariableArray packed, VariableArray unpacked,
                                       GadgetPtr packingGadget, GadgetPtr unpackingGadget);

TEST(ConstraintsLib,R1P_Packing_Gadgets) {
    initPublicParamsFromEdwardsParam();
    auto unpackingPB = Protoboard::create(R1P);
    auto packingPB = Protoboard::create(R1P);
    const int n = EXHAUSTIVE_N;
    { // test CompressionPacking_Gadget
        SCOPED_TRACE("testing CompressionPacking_Gadget");
        VariableArray packed(1, "packed");
        VariableArray unpacked(n, "unpacked");
        auto packingGadget = CompressionPacking_Gadget::create(packingPB, unpacked, packed,
                                                               PackingMode::PACK);
        auto unpackingGadget = CompressionPacking_Gadget::create(unpackingPB, unpacked, packed,
                                                                 PackingMode::UNPACK);
        packing_Gadget_R1P_ExhaustiveTest(unpackingPB, packingPB, n, packed, unpacked, packingGadget,
                                          unpackingGadget);
    }
    { // test IntegerPacking_Gadget
        SCOPED_TRACE("testing IntegerPacking_Gadget");
        VariableArray packed(1, "packed");
        VariableArray unpacked(n, "unpacked");
        auto packingGadget = IntegerPacking_Gadget::create(packingPB, unpacked, packed,
                                                           PackingMode::PACK);
        auto unpackingGadget = IntegerPacking_Gadget::create(unpackingPB, unpacked, packed,
                                                             PackingMode::UNPACK);
        packing_Gadget_R1P_ExhaustiveTest(unpackingPB, packingPB, n, packed, unpacked, packingGadget,
                                          unpackingGadget);
    }
}

TEST(ConstraintsLib,LD2_CompressionPacking_Gadget) {
    auto unpackingPB = Protoboard::create(LD2);
    auto packingPB = Protoboard::create(LD2);
    const int n = EXHAUSTIVE_N;
    VariableArray packed(1, "packed");
    VariableArray unpacked(n, "unpacked");
    auto packingGadget = CompressionPacking_Gadget::create(packingPB, unpacked, packed, 
                                                           PackingMode::PACK);
    auto unpackingGadget = CompressionPacking_Gadget::create(unpackingPB, unpacked, packed,
                                                             PackingMode::UNPACK);
    packingGadget->generateConstraints();
    unpackingGadget->generateConstraints();
    for(int i = 0; i < 1u<<n; ++i) {
        ::std::vector<int> bits(n);
        size_t packedGF2X = 0;
        for(int j = 0; j < n; ++j) {
            bits[j] = i & 1u<<j ? 1 : 0 ;
            packedGF2X |= bits[j]<<j;
            packingPB->val(unpacked[j]) = bits[j]; // set unpacked bits in the packing protoboard
        }
        // set the packed value in the unpacking protoboard
        unpackingPB->val(packed[0]) = Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,packedGF2X);
        unpackingGadget->generateWitness();
        packingGadget->generateWitness();
        stringstream s;
        s << endl << "i = " << i << ", Packed Value: " << Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,packedGF2X) << endl;
        SCOPED_TRACE(s.str());
        ASSERT_TRUE(unpackingPB->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
        ASSERT_TRUE(packingPB->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
        // check packed value is correct
        ASSERT_EQ(packingPB->val(packed[0]), Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,packedGF2X)); 
        for(int j = 0; j < n; ++j) {
            // Tests for unpacking gadget
            SCOPED_TRACE(FMT("\nValue being packed/unpacked: %u, bits[%u] = %u" , i, j, bits[j]));
            ASSERT_EQ(unpackingPB->val(unpacked[j]), bits[j] ? 1 : 0); // check bit correctness
            packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = 1-bits[j]; // flip bit
            ASSERT_FALSE(unpackingPB->isSatisfied());
            ASSERT_FALSE(packingPB->isSatisfied());
            packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = bits[j]; // restore bit
            // special case to test booleanity checks. Cause arithmetic constraints to stay
            // satisfied while ruining Booleanity
            if (j > 0 && bits[j]==1 && bits[j-1]==0 ) { 
                packingPB->val(unpacked[j-1]) = unpackingPB->val(unpacked[j-1]) = getGF2E_X();
                packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = 0; 
                ASSERT_FALSE(unpackingPB->isSatisfied());
                ASSERT_TRUE(packingPB->isSatisfied()); // packing should not enforce Booleanity
                // restore correct state
                packingPB->val(unpacked[j-1]) = unpackingPB->val(unpacked[j-1]) = 0; 
                packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = 1;
            }
        }
    }
 }

TEST(ConstraintsLib, LD2_CompressionPacking_Gadget_largeNums) {
    // Test packing/unpacking for the case of more than 1 packed element.
    using ::std::vector;
    
    //initialize the context field
    const int packedSize = 2;
    const int unpackedSize = packedSize * IRR_DEGREE;
    vector<int64_t> packingVal(packedSize);
    for(int i = 0; i < packedSize; ++i) {
        packingVal[i] = i;
    }
    packingVal[0] = 42; // The Answer To Life, the Universe and Everything
    packingVal[1] = 26-9-1984; // My birthday
    auto unpackingPB = Protoboard::create(LD2);
    auto packingPB = Protoboard::create(LD2);
    VariableArray packed(packedSize, "packed");
    VariableArray unpacked(unpackedSize, "unpacked");
    auto packingGadget = CompressionPacking_Gadget::create(packingPB, unpacked, packed,
                                                           PackingMode::PACK);
    auto unpackingGadget = CompressionPacking_Gadget::create(unpackingPB, unpacked, packed,
                                                             PackingMode::UNPACK);
    packingGadget->generateConstraints();
    unpackingGadget->generateConstraints();

    vector<int> bits(unpackedSize);
    vector<size_t> packedGF2X(packedSize,0);
    for(int j = 0; j < unpackedSize; ++j) {
        bits[j] = packingVal[j / IRR_DEGREE] & 1ul<<(j % IRR_DEGREE) ? 1 : 0;
        packedGF2X[j / IRR_DEGREE] |=  bits[j]<<(j % IRR_DEGREE);
        packingPB->val(unpacked[j]) = bits[j]; // set unpacked bits in the packing protoboard
    }
    // set the packed value in the unpacking protoboard
    for(int j = 0; j < packedSize; ++j) {
        unpackingPB->val(packed[j]) = Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,packedGF2X[j]);
    }
    unpackingGadget->generateWitness();
    packingGadget->generateWitness();
    ASSERT_TRUE(unpackingPB->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_TRUE(packingPB->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    // check packed values are correct
    for(int j = 0; j < packedSize; ++j) {
        ASSERT_EQ(packingPB->val(packed[j]), Algebra::mapIntegerToFieldElement(0,Algebra::ExtensionDegree,packedGF2X[j]); 
    }
    for(int j = 0; j < unpackedSize; ++j) {
        // Tests for unpacking gadget
        SCOPED_TRACE(FMT("j = %lu", j));
        ASSERT_EQ(unpackingPB->val(unpacked[j]), bits[j] ? 1 : 0); // check bit correctness
        packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = 1-bits[j]; // flip bit
        ASSERT_FALSE(unpackingPB->isSatisfied());
        ASSERT_FALSE(packingPB->isSatisfied());
        packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = bits[j]; // restore bit
    }
}


void equalsConstTest(ProtoboardPtr pb); // Forward declaration

TEST(ConstraintsLib,R1P_EqualsConst_Gadget) {
    initPublicParamsFromEdwardsParam();
    auto pb = Protoboard::create(R1P);
    equalsConstTest(pb);
}

TEST(ConstraintsLib,LD2_EqualsConst_Gadget) {
    auto pb = Protoboard::create(LD2);
    equalsConstTest(pb);
}

TEST(ConstraintsLib,ConditionalFlag_Gadget) {
    initPublicParamsFromEdwardsParam();
    auto pb = Protoboard::create(R1P);
    FlagVariable flag;
    Variable condition("condition");
    auto cfGadget = ConditionalFlag_Gadget::create(pb, condition, flag);
    cfGadget->generateConstraints();
    pb->val(condition) = 1;
    cfGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    pb->val(condition) = 42;
    cfGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_EQ(pb->val(flag),1);
    pb->val(condition) = 0;
    ASSERT_FALSE(pb->isSatisfied());
    cfGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_EQ(pb->val(flag),0);
    pb->val(flag) = 1;
    ASSERT_FALSE(pb->isSatisfied());
}

TEST(ConstraintsLib,LogicImplication_Gadget) {
    auto pb = Protoboard::create(LD2);
    FlagVariable flag;
    Variable condition("condition");
    auto implyGadget = LogicImplication_Gadget::create(pb, condition, flag);
    implyGadget->generateConstraints();
    pb->val(condition) = 1;
    pb->val(flag) = 0;
    ASSERT_FALSE(pb->isSatisfied());
    implyGadget->generateWitness();
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    ASSERT_EQ(pb->val(flag), 1);
    pb->val(condition) = 0;
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    implyGadget->generateWitness();
    ASSERT_EQ(pb->val(flag), 1);
    pb->val(flag) = 0;
    ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
}

void andGadgetExhaustiveTest(ProtoboardPtr pb, size_t n) {
    VariableArray inputs(n, "inputs");
    Variable output("output");
    auto andGadget = AND_Gadget::create(pb, inputs, output);
    andGadget->generateConstraints();
    for (size_t curInput = 0; curInput < 1u<<n; ++curInput) {
        for (size_t maskBit = 0; maskBit < n; ++maskBit) {
            pb->val(inputs[maskBit]) = (curInput & (1u<<maskBit)) ? 1 : 0;
        }
        andGadget->generateWitness();
        {
            SCOPED_TRACE(FMT("Positive (completeness) test failed. curInput: %u", curInput));
            EXPECT_TRUE(pb->isSatisfied());
        }
        {
            SCOPED_TRACE(pb->annotation());
            SCOPED_TRACE(FMT("Negative (soundness) test failed. curInput: %u, Constraints "
                                                                                "are:", curInput));
            pb->val(output) = (curInput == ((1u<<n) - 1)) ? 0 : 1;
            EXPECT_FALSE(pb->isSatisfied());
        }
    }
}

void orGadgetExhaustiveTest(ProtoboardPtr pb, size_t n) {
    VariableArray inputs(n, "inputs");
    Variable output("output");
    auto orGadget = OR_Gadget::create(pb, inputs, output);
    orGadget->generateConstraints();
    for (size_t curInput = 0; curInput < 1u<<n; ++curInput) {
        for (size_t maskBit = 0; maskBit < n; ++maskBit) {
            pb->val(inputs[maskBit]) = (curInput & (1u<<maskBit)) ? 1 : 0;
        }
        orGadget->generateWitness();
        {
            SCOPED_TRACE(FMT("Positive (completeness) test failed. curInput: %u", curInput));
            ASSERT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
        }
        {
            SCOPED_TRACE(pb->annotation());
            SCOPED_TRACE(FMT("Negative (soundness) test failed. curInput: %u, Constraints "
                                                                                "are:", curInput));
            pb->val(output) = (curInput == 0) ? 1 : 0;
            ASSERT_FALSE(pb->isSatisfied());
        }
    }
}

void packing_Gadget_R1P_ExhaustiveTest(ProtoboardPtr unpackingPB, ProtoboardPtr packingPB,
                                       const int n, VariableArray packed, VariableArray unpacked,
                                       GadgetPtr packingGadget, GadgetPtr unpackingGadget) {
    packingGadget->generateConstraints();
    unpackingGadget->generateConstraints();
    for(int i = 0; i < 1u<<n; ++i) {
        ::std::vector<int> bits(n);
        for(int j = 0; j < n; ++j) {
            bits[j] = i & 1u<<j ? 1 : 0 ;
            packingPB->val(unpacked[j]) = bits[j]; // set unpacked bits in the packing protoboard
        }
        unpackingPB->val(packed[0]) = i; // set the packed value in the unpacking protoboard
        unpackingGadget->generateWitness();
        packingGadget->generateWitness();
        ASSERT_TRUE(unpackingPB->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
        ASSERT_TRUE(packingPB->isSatisfied());
        ASSERT_EQ(packingPB->val(packed[0]), i); // check packed value is correct
        for(int j = 0; j < n; ++j) {
            // Tests for unpacking gadget
            SCOPED_TRACE(FMT("\nValue being packed/unpacked: %u, bits[%u] = %u" , i, j, bits[j]));
            ASSERT_EQ(unpackingPB->val(unpacked[j]), bits[j]); // check bit correctness
            packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = 1-bits[j]; // flip bit
            ASSERT_FALSE(unpackingPB->isSatisfied());
            ASSERT_FALSE(packingPB->isSatisfied());
            packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = bits[j]; // restore bit
            // special case to test booleanity checks. Cause arithmetic constraints to stay
            // satisfied while ruining Booleanity
            if (j > 0 && bits[j]==1 && bits[j-1]==0 ) { 
                packingPB->val(unpacked[j-1]) = unpackingPB->val(unpacked[j-1]) = 2;
                packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = 0; 
                ASSERT_FALSE(unpackingPB->isSatisfied());
                ASSERT_TRUE(packingPB->isSatisfied()); // packing should not enforce Booleanity
                // restore correct state
                packingPB->val(unpacked[j-1]) = unpackingPB->val(unpacked[j-1]) = 0; 
                packingPB->val(unpacked[j]) = unpackingPB->val(unpacked[j]) = 1;
            }
        }
    }
}

void equalsConstTest(ProtoboardPtr pb) {
    Variable input("input");
    Variable result("result");
    auto gadget = EqualsConst_Gadget::create(pb, 0, input, result);
    gadget->generateConstraints();
    pb->val(input) = 0;
    gadget->generateWitness();
    // Positive test for input == n
    EXPECT_EQ(pb->val(result), 1);
    EXPECT_TRUE(pb->isSatisfied());
    // Negative test
    pb->val(result) = 0;
    EXPECT_FALSE(pb->isSatisfied());
    // Positive test for input != n
    pb->val(input) = 1;
    gadget->generateWitness();
    EXPECT_EQ(pb->val(result), 0);
    EXPECT_TRUE(pb->isSatisfied());
    // Negative test
    pb->val(input) = 0;
    EXPECT_FALSE(pb->isSatisfied());
}


} // namespace PCP_Project 
