#ifndef AEG_IR_HPP
#define AEG_IR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <vector>

enum aeg_ir_state { AUTO, COOL, DRY, FAN, OFF };

template <class T> class base_aeg_ir_request {
public:
  inline std::array<uint64_t, 2> get_raw_data() const {
    T data = get_data();
    std::array<uint64_t, 2> raw = {0, 0};

    write_struct_to_array(&data, get_hi_size(), &raw[0]);
    write_struct_to_array(&data, get_hi_size() + get_lo_size(), &raw[1],
                          get_hi_size());

    return raw;
  }

protected:
  virtual T get_data() const = 0;
  virtual size_t get_hi_size() const noexcept = 0;
  virtual size_t get_lo_size() const noexcept = 0;

private:
  static bool is_little_endian() {
    ushort a = 0x1234;
    if (*((char *)&a) == 0x12) {
      return false;
    } else {
      return true;
    }
  }

  static void reverse_bytes(uint8_t *start, size_t size) {
    uint8_t *istart = start, *iend = istart + size;
    std::reverse(istart, iend);
  }

  static void write_struct_to_array(const void *in_struct,
                                    const size_t in_struct_len,
                                    uint64_t *const out_data,
                                    const size_t start = 0) {
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
};

typedef struct _aeg_auto_ir_request {
  uint8_t fan_speed_timer = 0b10100000;
  uint8_t fixed_1 = 0b00000000;
  uint8_t fixed_2 = 0b11100000;
  uint8_t temp_swing_1 = 0;
  uint8_t fixed_3 = 0b11000011;
  uint8_t temp_swing_2 = 0;
  uint8_t step_down = 0b00000000;
  uint8_t shift_1 = 0;
  uint8_t shift_2 = 0;
  uint8_t fixed_4 = 0b00000000;
} aeg_auto_ir_request_s;

class aeg_auto_ir_request : public base_aeg_ir_request<aeg_auto_ir_request_s> {
public:
  aeg_auto_ir_request(uint8_t temp, bool swing) : _temp(temp), _swing(swing) {}

protected:
  inline size_t get_hi_size() const noexcept override { return 5; }

  inline size_t get_lo_size() const noexcept override { return 5; }

  inline aeg_auto_ir_request_s get_data() const override {
    aeg_auto_ir_request_s res;
    res.temp_swing_1 = (_temp - 8) << 3;

    if (_temp < 27) {
      res.temp_swing_2 = (_temp + 5) << 3;
    } else if (_temp > 28) {
      res.temp_swing_2 = (_temp - 28) << 3;
    } else if (_temp == 27) {
      res.temp_swing_2 = 0b00100000;
    } else if (_temp == 28) {
      res.temp_swing_2 = 0b10100000;
    }

    if (_temp == 27 || _temp == 28) {
      res.shift_1 = 0b00000010;
      res.shift_2 = 0;
    } else {
      res.shift_1 = 0;
      res.shift_2 = 0b00100000;
    }

    if (_swing) {
      if (_temp == 27 || _temp == 28) {
        res.temp_swing_2 |= 0b00010000;
      } else {
        res.temp_swing_2 |= 0b00000011;
      }
    } else {
      res.temp_swing_1 |= 0b00000111;
      if (_temp != 27 && _temp != 28) {
        res.temp_swing_2 |= 0b00000010;
      }
    }

    return res;
  }

  uint8_t _temp;
  bool _swing;
};

#endif // AEG_IR_HPP
