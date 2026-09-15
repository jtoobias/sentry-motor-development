#include "gpio_pin.hpp"
#include <fmt/core.h>
#include <iostream>


static gpiod::chip find_chip_by_label(const std::string &label){
    for (auto &chip : gpiod::make_chip_iter()) {
        if (chip.label() == label) {
            return chip;
        }
    }
    return gpiod::chip();  // empty/invalid chip if not found
}

static int  pin_mode_to_request_type(PinMode mode){
    switch (mode){
        case PinMode::Input: return gpiod::line_request::DIRECTION_INPUT;
        case PinMode::Output: return gpiod::line_request::DIRECTION_OUTPUT;
    }
}

static std::bitset<32> pull_mode_to_pull_flag(PullMode pull){
    switch (pull){
        case PullMode::None: return gpiod::line_request::FLAG_BIAS_DISABLE;
        case PullMode::Up: return gpiod::line_request::FLAG_BIAS_PULL_UP;
        case PullMode::Down: return gpiod::line_request::FLAG_BIAS_PULL_DOWN;
    }
}

GpioPin::GpioPin(const std::string &label,
                int offset,
                PinMode mode,
                PullMode pull,
                bool inverted) {


    gpiod::line_request request;
    mode_ = mode;
    chip_ = find_chip_by_label(label);
    if (!chip_) {
        std::cerr << fmt::format("Couldn't locate the gpio chip {0} for offset {1}",label,offset);
        std::cerr << std::endl;
        return;
    }
    line_ = chip_.get_line(offset);
    request.request_type = pin_mode_to_request_type(mode_);
    request.flags |= pull_mode_to_pull_flag(pull);
    if (inverted) request.flags |= gpiod::line_request::FLAG_ACTIVE_LOW;
    line_.request(request);
}

void GpioPin::write(int value) {
    if (mode_ != PinMode::Output) {
        throw std::logic_error("write() called on a pin not configured as output");
    }
    execute_write(value);
}

int GpioPin::read() {
    return execute_read();
}