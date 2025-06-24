#include "../components/aeg_ac/aeg_ir.hpp"
#include "test_util.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

void assert_req(int temp, bool swing, uint64_t expected_1, uint64_t expected_2) {
    aeg_auto_ir_request request(temp, swing);
    std::array<uint64_t, 2> raw_data = request.get_raw_data();

    EXPECT_BIN_EQ(raw_data[0], expected_1);
    EXPECT_BIN_EQ(raw_data[1], expected_2);
}

TEST(AutoTest, Check21DegNoSwing) {
    assert_req(21, false, 0xA000E06FC3, 0xD200002000);
}
