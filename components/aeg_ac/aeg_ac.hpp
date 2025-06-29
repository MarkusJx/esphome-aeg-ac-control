#ifndef AEG_AC_COMPONENT_HPP
#define AEG_AC_COMPONENT_HPP

#include "esphome/components/climate/climate.h"
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"

namespace esphome::aeg_ac {
  class AegAC : public Component, public climate::Climate {
   public:
    void setup() override;

    climate::ClimateTraits traits() override;

    void control(const climate::ClimateCall &call) override;

    void set_pin(InternalGPIOPin *pin);

   protected:
    InternalGPIOPin *pin_ = nullptr;
  };
}  // namespace esphome::aeg_ac

#endif  // AEG_AC_COMPONENT_HPP