#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
	size_t res = RUN_ALL_TESTS();
    return res;
}
