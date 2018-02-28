#include <gtest/gtest.h>
#include <gadgetlib2/pp.hpp>
#include <gadgetlib2/protoboard.hpp>

namespace PCP_Project {

TEST(ConstraintsLib,R1P_enforceBooleanity) {
    initPublicParamsFromEdwardsParam();
    auto pb = Protoboard::create(R1P);
    Variable x;
    pb->enforceBooleanity(x);
    pb->val(x) = 0;
    EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    pb->val(x) = 1;
    EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    pb->val(x) = Fp(2);
    EXPECT_FALSE(pb->isSatisfied());
}

TEST(ConstraintsLib,LD2_enforceBooleanity) {
    auto pb = Protoboard::create(LD2);
    Variable x;
    pb->enforceBooleanity(x);
    pb->val(x) = 0;
    EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    pb->val(x) = 1;
    EXPECT_TRUE(pb->isSatisfied(PrintOptions::DBG_PRINT_IF_NOT_SATISFIED));
    FieldElement x_squared = Algebra::mapIntegerToFieldElement(2,1,1);
    pb->val(x) = x_squared;
    EXPECT_FALSE(pb->isSatisfied());
}

TEST(ConstraintsLib, Protoboard_unpackedWordAssignmentEqualsValue_R1P) {
    initPublicParamsFromEdwardsParam();
    auto pb = Protoboard::create(R1P);
    const UnpackedWord unpacked(8, "unpacked");
    pb->setValuesAsBitArray(unpacked, 42);
    ASSERT_TRUE(pb->unpackedWordAssignmentEqualsValue(unpacked, 42));
    ASSERT_FALSE(pb->unpackedWordAssignmentEqualsValue(unpacked, 43));
    ASSERT_FALSE(pb->unpackedWordAssignmentEqualsValue(unpacked, 1024 + 42));
}

TEST(ConstraintsLib, Protoboard_multipackedWordAssignmentEqualsValue_R1P) {
    initPublicParamsFromEdwardsParam();
    auto pb = Protoboard::create(R1P);
    const MultiPackedWord multipacked(8, R1P, "multipacked");
    pb->val(multipacked[0]) = 42;
    ASSERT_TRUE(pb->multipackedWordAssignmentEqualsValue(multipacked, 42));
    ASSERT_FALSE(pb->multipackedWordAssignmentEqualsValue(multipacked, 43));
    const MultiPackedWord multipackedAgnostic(AGNOSTIC);
    ASSERT_THROW(pb->multipackedWordAssignmentEqualsValue(multipackedAgnostic, 43),
                 ::std::runtime_error);
}

TEST(ConstraintsLib, Protoboard_dualWordAssignmentEqualsValue_R1P) {
    initPublicParamsFromEdwardsParam();
    auto pb = Protoboard::create(R1P);
    const DualWord dualword(8, R1P, "dualword");
    pb->setDualWordValue(dualword, 42);
    ASSERT_TRUE(pb->dualWordAssignmentEqualsValue(dualword, 42));
    ASSERT_FALSE(pb->dualWordAssignmentEqualsValue(dualword, 43));
    ASSERT_FALSE(pb->dualWordAssignmentEqualsValue(dualword, 42 + 1024));
}

} // namespace PCP_Project
