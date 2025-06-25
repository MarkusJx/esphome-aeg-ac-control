#include "../components/aeg_ac/aeg_ir.hpp"
#include "test_util.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

void assert_req(int temp, bool swing, uint64_t expected_1, uint64_t expected_2) {
    aeg_ir::aeg_auto_ir_request request(temp, swing);
    std::array<uint64_t, 2> raw_data = request.get_raw_data();

    EXPECT_BIN_EQ(raw_data[0], expected_1);
    EXPECT_BIN_EQ(raw_data[1], expected_2);
}

TEST(AutoTest, Check21DegNoSwing) {
    assert_req(21, false, 0xA000E06FC3, 0xD200002000);
}

TEST(AutoTest, Check21DegSwing) {
    assert_req(21, true, 0xA000E068C3, 0xCB00002000);
}

TEST(AutoTest, Check26DegNoSwing) {
    assert_req(26, false, 0xA000E097C3, 0xFA00002000);
}

TEST(AutoTest, Check26DegSwing) {
    assert_req(26, true, 0xA000E090C3, 0xF300002000);
}

TEST(AutoTest, Check27DegNoSwing) {
    assert_req(27, false, 0xA000E09FC3, 0x200002000);
}

TEST(AutoTest, Check27DegSwing) {
    assert_req(27, true, 0xA000E098C3, 0xFB00002000);
}

TEST(AutoTest, Check28DegNoSwing) {
    assert_req(28, false, 0xA000E0A7C3, 0xA00002000);
}

TEST(AutoTest, Check28DegSwing) {
    assert_req(28, true, 0xA000E0A0C3, 0x300002000);
}

TEST(AutoTest, Check29DegNoSwing) {
    assert_req(29, false, 0xA000E0AFC3, 0x1200002000);
}

TEST(AutoTest, Check29DegSwing) {
    assert_req(29, true, 0xA000E0A8C3, 0xB00002000);
}

TEST(AutoTest, Check30DegNoSwing) {
    assert_req(30, false, 0xA000E0B7C3, 0x1A00002000);
}

TEST(AutoTest, Check30DegSwing) {
    assert_req(30, true, 0xA000E0B0C3, 0x1300002000);
}
