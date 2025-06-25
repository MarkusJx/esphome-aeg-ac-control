#include "../components/aeg_ac/aeg_ir.hpp"
#include "test_util.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

void assert_req(bool swing, uint64_t expected_1, uint64_t expected_2) {
  aeg_ir::aeg_dry_ir_request request(swing);
  std::array<uint64_t, 2> raw_data = request.get_raw_data();

  EXPECT_BIN_EQ(raw_data[0], expected_1);
  EXPECT_BIN_EQ(raw_data[1], expected_2);
}

TEST(DryTest, checkNoSwing) {
  assert_req(false, 0x40006000E077C3, 0xE006002000);
}

TEST(DryTest, checkSwing) { assert_req(true, 0x40006000E070C3, 0xD502002000); }
