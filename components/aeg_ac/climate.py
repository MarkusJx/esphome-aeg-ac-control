from esphome import pins
import esphome.codegen as cg
from esphome.components import climate
import esphome.config_validation as cv
from esphome.const import (
    CONF_OUTPUT_ID,
    CONF_PIN
)

LIBRARIES = ["arduino-irremote/IRremote"]

aeg_ac_ns = cg.esphome_ns.namespace('aeg_ac')

AegAC = aeg_ac_ns.class_("AegAC", cg.Component)

CONFIG_SCHEMA = (
    climate.climate_schema(AegAC)
    .extend(
        {
            cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(AegAC),
            cv.Required(CONF_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = await climate.new_climate(config)
    await cg.register_component(var, config)

    output_ = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(output_))

