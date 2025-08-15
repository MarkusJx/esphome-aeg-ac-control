#include "aeg_ir.hpp"

#include <algorithm>
#include <vector>
#include <sys/types.h>

using namespace aeg_ir;

bool is_little_endian() {
  ushort a = 0x1234;
  if (*((char *)&a) == 0x12) {
    return false;
  } else {
    return true;
  }
}

void reverse_bytes(uint8_t *start, size_t size) {
  uint8_t *istart = start, *iend = istart + size;
  std::reverse(istart, iend);
}

void util::write_struct_to_array(const void *in_struct,
                                 const size_t in_struct_len,
                                 uint64_t *const out_data, const size_t start) {
  size_t num_bytes = in_struct_len - start;
  std::vector<uint8_t> bytes(num_bytes);

  for (size_t i = start; i < in_struct_len; i++) {
    bytes[i - start] = ((uint8_t *)in_struct)[i];
  }

  if (is_little_endian()) {
    reverse_bytes(bytes.data(), num_bytes);
  }

  for (size_t i = 0; i < num_bytes; i++) {
    ((uint8_t *)out_data)[i] = bytes[i];
  }
}

aeg_auto_ir_request::aeg_auto_ir_request(uint8_t temp, bool swing)
    : base_aeg_ir_request<requests::aeg_auto_ir_request_s>(5),
      _temp(temp),
      _swing(swing) {}

requests::aeg_auto_ir_request_s aeg_auto_ir_request::get_data() const {
  requests::aeg_auto_ir_request_s res;
  res.temp_swing_1 = (_temp - 8) << 3;

  uint8_t temp_switch_1 = _swing ? 28 : 27;
  uint8_t temp_switch_2 = _swing ? 29 : 28;

  if (_temp < temp_switch_1) {
    res.temp_swing_2 = (_temp + (_swing ? 4 : 5)) << 3;
  } else if (_temp > temp_switch_2) {
    res.temp_swing_2 = (_temp - (_swing ? 28 : 27)) << 3;
  } else if (_temp == temp_switch_1) {
    res.temp_swing_2 = 0b00000010;
  } else if (_temp == temp_switch_2) {
    res.temp_swing_2 = 0b00001010;
  }

  res.shift_1 = 0;
  res.shift_2 = 0b00100000;

  if (_swing) {
    if (_temp == temp_switch_1 || _temp == temp_switch_2) {
      res.temp_swing_2 |= 0b00000001;
    } else {
      res.temp_swing_2 |= 0b00000011;
    }
  } else {
    res.temp_swing_1 |= 0b00000111;
    if (_temp != temp_switch_1 && _temp != temp_switch_2) {
      res.temp_swing_2 |= 0b00000010;
    }
  }

  return res;
}

aeg_ir_request::aeg_ir_request()
    : base_aeg_ir_request<requests::aeg_ir_request_s>(7) {}

aeg_fan_ir_request::aeg_fan_ir_request(fan_mode mode, bool swing)
    : _mode(mode), _swing(swing) {}

requests::aeg_ir_request_s aeg_fan_ir_request::get_data() const {
  requests::aeg_ir_request_s res;
  res.mode_sleep = 0b11000000;
  res.fan_speed_timer = _mode << 5;

  if (!_swing) {
    res.temp_swing_1 = 0b00000111;
  }

  res.temp_swing_2 = _mode << 5;
  if (_swing) {
    res.temp_swing_2 |= 0b10000111;
  } else {
    res.temp_swing_2 |= 0b10001110;
  }

  res.step_down = 0b00000100;
  res.shift_2 = 0b00100000;

  return res;
}

aeg_dry_ir_request::aeg_dry_ir_request(bool swing) : _swing(swing) {}

requests::aeg_ir_request_s aeg_dry_ir_request::get_data() const {
  requests::aeg_ir_request_s res;
  res.mode_sleep = 0b01000000;
  res.fan_speed_timer = 0b01100000;

  if (_swing) {
    res.temp_swing_1 = 0b01110000;
    res.temp_swing_2 = 0b11010101;
    res.step_down = 0b00000010;
  } else {
    res.temp_swing_1 = 0b01110111;
    res.temp_swing_2 = 0b11100000;
    res.step_down = 0b00000110;
  }

  res.shift_2 = 0b00100000;

  return res;
}
