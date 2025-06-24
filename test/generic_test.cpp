#include <gtest/gtest.h>
#include "../components/aeg_ac/aeg_ir.hpp"

TEST(GenericTest, checkAutoRequestSize) {
    EXPECT_EQ(10, sizeof(aeg_auto_ir_request_s));
}