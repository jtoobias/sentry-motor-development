#include "gpio_pin.hpp"
#include <fmt/core.h>
#include <iostream>


GpioPin::GpioPin(const std::string &label,int offset) {
    chip_ = find_chip_by_label(label);
    if (!chip_) {
        std::cerr << fmt::format("Couldn't locate the gpio chip {0} for offset {1}",label,offset);
        std::cerr << std::endl;
        return;
    }
    line_ = chip_.get_line(offset);
}

/**
 * @brief Set pin up as a input/output with configurations
 * 
 * input is standard digital, output can be digitial output/PWM
 * Each can have the default state set and 
 * also inversion can be set if wanted.
 * 
 * @param mode 
 * @param invert 
 * @param pin_state 
 */
void GpioPin::configure(const PinConfig& config){
    gpiod::line_request request;
    int default_value = 0;
    if (config.mode == PinMode::Input){
        request.request_type = gpiod::line_request::DIRECTION_INPUT;
    } else if (config.mode == PinMode::Output){
        request.request_type = gpiod::line_request::DIRECTION_OUTPUT;
    }

    if (config.inverted){
        request.flags |= gpiod::line_request::FLAG_ACTIVE_LOW;
    }
    if (config.pull == PullMode::None) {
        request.flags |= gpiod::line_request::FLAG_BIAS_DISABLE;
    } else if (config.pull == PullMode::Up) {
        request.flags |= gpiod::line_request::FLAG_BIAS_PULL_UP;
    } else {
        request.flags |= gpiod::line_request::FLAG_BIAS_PULL_DOWN;
    }

    line_.request(request,int(config.default_state));
}


gpiod::chip GpioPin::find_chip_by_label(const std::string &label) {
for (auto &chip : gpiod::make_chip_iter()) {
    if (chip.label() == label) {
        return chip;
    }
}
return gpiod::chip();  // empty/invalid chip if not found
}

void GpioPin::write(int value) {
    throw std::logic_error("write() not supported on this pin's mode");
}

int GpioPin::read(void) {
    throw std::logic_error("read() not supported on this pin's mode");
}

void GpioPin::execute_write(int value) {
    line_.set_value(value);
}

int GpioPin::execute_read(void) {
    return line_.get_value();
}

/**
 * @brief Writes the value to the pin.
 * 
 * @param state 
 */
void OutputPin::write(int value){
execute_write(value);
}

/**
 * @brief Reads the raw value from the pin.
 * 
 * @return int 
 */
int InputPin::read(void){
return execute_read();
}
