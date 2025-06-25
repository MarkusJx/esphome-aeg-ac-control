#ifndef AEG_IR_HPP
#define AEG_IR_HPP

#include <array>
#include <cstddef>
#include <cstdint>

namespace aeg_ir {
  namespace util {
    void write_struct_to_array(const void *in_struct, size_t in_struct_len,
                               uint64_t *out_data, size_t start = 0);
  }  // namespace util

  namespace requests {
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

    typedef struct _aeg_ir_request {
      uint8_t mode_sleep = 0;
      uint8_t timer_0_5 = 0b00000000;
      uint8_t fan_speed_timer = 0;
      uint8_t fixed_1 = 0b00000000;
      uint8_t fixed_2 = 0b11100000;
      uint8_t temp_swing_1 = 0;
      uint8_t fixed_3 = 0b11000011;
      uint8_t temp_swing_2 = 0;
      uint8_t step_down = 0b00000000;
      uint8_t shift_1 = 0;
      uint8_t shift_2 = 0;
      uint8_t fixed_4 = 0b00000000;
    } aeg_ir_request_s;
  }  // namespace requests

  enum fan_mode { FAN_AUTO = 5, FAN_LOW = 3, FAN_MID = 2, FAN_HIGH = 1 };

  template <class T>
  class base_aeg_ir_request {
   public:
    [[nodiscard]] inline std::array<uint64_t, 2> get_raw_data() const {
      T data = get_data();
      std::array<uint64_t, 2> raw = {0, 0};

      util::write_struct_to_array(&data, _hi_size, &raw[0]);
      util::write_struct_to_array(&data, _hi_size + 5, &raw[1], _hi_size);

      return raw;
    }

   protected:
    explicit inline base_aeg_ir_request(size_t hi_size) : _hi_size(hi_size) {}

    [[nodiscard]] virtual T get_data() const = 0;

   private:
    const size_t _hi_size;
  };

  class aeg_auto_ir_request
      : public base_aeg_ir_request<requests::aeg_auto_ir_request_s> {
   public:
    aeg_auto_ir_request(uint8_t temp, bool swing);

   protected:
    requests::aeg_auto_ir_request_s get_data() const override;

    uint8_t _temp;
    bool _swing;
  };

  class aeg_ir_request
      : public base_aeg_ir_request<requests::aeg_ir_request_s> {
   protected:
    aeg_ir_request();
  };

  class aeg_fan_ir_request : public aeg_ir_request {
   public:
    aeg_fan_ir_request(fan_mode mode, bool swing);

   protected:
    requests::aeg_ir_request_s get_data() const override;

   private:
    fan_mode _mode;
    bool _swing;
  };

  class aeg_dry_ir_request : public aeg_ir_request {
   public:
    explicit aeg_dry_ir_request(bool swing);

   protected:
    requests::aeg_ir_request_s get_data() const override;

   private:
    bool _swing;
  };
}  // namespace aeg_ir

#endif  // AEG_IR_HPP
