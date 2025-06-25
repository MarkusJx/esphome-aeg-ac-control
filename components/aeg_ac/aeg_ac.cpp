#include "aeg_ac.hpp"

#include "aeg_ir.hpp"
#include "esphome/components/climate/climate_mode.h"

#define NO_LED_FEEDBACK_CODE
#define SEND_PWM_BY_TIMER
#include <IRremote.hpp>
#include <array>
#include <sstream>

#define DEBUG_AC 1
using namespace esphome;
using namespace esphome::aeg_ac;

void send_data(std::array<uint64_t, 2> data) {
#if DEBUG_AC
  std::stringstream stream;
  stream << std::uppercase << std::hex << data[0] << ", " << data[1];
  std::string raw_data(stream.str());

  ESP_LOGD("DEBUG", "Sending raw data: {%s}", raw_data.c_str());
#endif  // DEBUG_AC

  IrSender.sendPulseDistanceWidthFromArray(38, 9000, 4500, 500, 1700, 500, 600,
                                           (IRRawDataType *)data.data(), 104,
                                           PROTOCOL_IS_LSB_FIRST, 0, 0);
}

void AegAC::setup() {
  IrSender.begin(this->pin_->get_pin());
  delay(200);

  this->mode = mode;
  this->fan_mode = climate::CLIMATE_FAN_AUTO;
  this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
  this->target_temperature = 24;
  this->publish_state();
}

climate::ClimateTraits AegAC::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(false);
  traits.set_supports_two_point_target_temperature(false);

  traits.set_supported_modes({
      climate::CLIMATE_MODE_OFF,
      climate::CLIMATE_MODE_AUTO,
      // climate::CLIMATE_MODE_DRY, Not supported yet
      climate::CLIMATE_MODE_COOL,
      climate::CLIMATE_MODE_FAN_ONLY,
  });

  traits.set_supported_fan_modes({
      climate::CLIMATE_FAN_AUTO,
      climate::CLIMATE_FAN_LOW,
      climate::CLIMATE_FAN_MEDIUM,
      climate::CLIMATE_FAN_HIGH,
  });

  traits.set_supported_swing_modes({
      climate::CLIMATE_SWING_OFF,
      climate::CLIMATE_SWING_VERTICAL,
  });

  traits.set_visual_min_temperature(16);
  traits.set_visual_max_temperature(31);
  traits.set_visual_temperature_step(1);
  return traits;
}

void AegAC::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    // User requested mode change
    climate::ClimateMode mode = *call.get_mode();

    // Publish updated state
    this->mode = mode;
    this->publish_state();
  }

  // Code for what to do when the fan speed / mode is changed on the dashboard
  if (call.get_fan_mode().has_value()) {
    // User requested fan mode change
    climate::ClimateFanMode fan_mode = *call.get_fan_mode();

    this->fan_mode = fan_mode;
  }

  if (call.get_swing_mode().has_value()) {
    // User requested fan mode change
    climate::ClimateSwingMode swing_mode = *call.get_swing_mode();

    this->swing_mode = swing_mode;
  }

  // Code for what to do when the temperature is changed on the dashboard
  if (call.get_target_temperature().has_value()) {
    // User requested target temperature change
    float temp = *call.get_target_temperature();

    this->target_temperature = temp;
  }

  if (mode == climate::CLIMATE_MODE_OFF) {
    // Use fixed off command for auto, 21°, no swing
    send_data({0xA000E06FC3, 0xB705000000});
  } else if (mode == climate::CLIMATE_MODE_DRY) {
    this->fan_mode = climate::ClimateFanMode::CLIMATE_FAN_LOW;
    aeg_ir::aeg_dry_ir_request request(this->swing_mode ==
                                       climate::CLIMATE_SWING_VERTICAL);
    send_data(request.get_raw_data());
  } else if (mode == climate::CLIMATE_MODE_COOL) {
    // TODO
  } else if (mode == climate::CLIMATE_MODE_FAN_ONLY) {
    aeg_ir::fan_mode mode;
    if (this->fan_mode == climate::ClimateFanMode::CLIMATE_FAN_HIGH) {
      mode = aeg_ir::fan_mode::FAN_HIGH;
    } else if (this->fan_mode == climate::ClimateFanMode::CLIMATE_FAN_MEDIUM) {
      mode = aeg_ir::fan_mode::FAN_MID;
    } else {
      mode = aeg_ir::fan_mode::FAN_LOW;
      this->fan_mode = climate::ClimateFanMode::CLIMATE_FAN_LOW;
    }

    aeg_ir::aeg_fan_ir_request request(
        mode, this->swing_mode == climate::CLIMATE_SWING_VERTICAL);
    send_data(request.get_raw_data());
  } else if (mode == climate::CLIMATE_MODE_AUTO) {
    this->fan_mode = climate::ClimateFanMode::CLIMATE_FAN_AUTO;
    aeg_ir::aeg_auto_ir_request request(
        this->target_temperature,
        this->swing_mode == climate::CLIMATE_SWING_VERTICAL);
    send_data(request.get_raw_data());
  }

  this->publish_state();
}

void AegAC::set_pin(InternalGPIOPin *pin) {
  pin_ = pin;
  IrSender.setSendPin(pin->get_pin());
}
