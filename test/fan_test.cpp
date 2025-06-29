#include "../components/aeg_ac/aeg_ir.hpp"
#include "test_util.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

void assert_req(aeg_ir::fan_mode mode, bool swing, uint64_t expected_1,
                uint64_t expected_2) {
  aeg_ir::aeg_fan_ir_request request(mode, swing);
  std::array<uint64_t, 2> raw_data = request.get_raw_data();

  EXPECT_BIN_EQ(raw_data[0], expected_1);
  EXPECT_BIN_EQ(raw_data[1], expected_2);
}

TEST(FanTest, checkLowNoSwing) {
  assert_req(aeg_ir::FAN_LOW, false, 0xC0006000E007C3, 0xEE04002000);
}

TEST(FanTest, checkMidNoSwing) {
  assert_req(aeg_ir::FAN_MID, false, 0xC0004000E007C3, 0xCE04002000);
}

TEST(FanTest, checkHighNoSwing) {
  assert_req(aeg_ir::FAN_HIGH, false, 0xC0002000E007C3, 0xAE04002000);
}

TEST(FanTest, checkLowSwing) {
  assert_req(aeg_ir::FAN_LOW, true, 0xC0006000E000C3, 0xE704002000);
}

TEST(FanTest, checkMidSwing) {
  assert_req(aeg_ir::FAN_MID, true, 0xC0004000E000C3, 0xC704002000);
}

TEST(FanTest, checkHighSwing) {
  assert_req(aeg_ir::FAN_HIGH, true, 0xC0002000E000C3, 0xA704002000);
}
