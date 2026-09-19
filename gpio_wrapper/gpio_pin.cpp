#include "gpio_pin.hpp"
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

const PinLocation& GpioPin::get_gpio_details(int gpio_pin_number){
    auto it = gpio_map_.find(gpio_pin_number);
    if (it != gpio_map_.end()) {
        return it->second;
    }
}

GpioPin::GpioPin(int gpio_pin_number,
                PinMode mode,
                PullMode pull,
                bool inverted){

    gpiod::line_request request;
    const PinLocation& pin_info = get_gpio_details(gpio_pin_number);
    mode_ = mode;
    chip_ = find_chip_by_label(pin_info.chip_label);
    if (!chip_) {
        throw std::runtime_error(fmt::format("Couldn't locate the gpio chip {0} for offset {1}",
            pin_info.chip_label,pin_info.offset));
    }

    try{
        line_ = chip_.get_line(pin_info.offset);
        request.request_type = pin_mode_to_request_type(mode_);
        request.flags |= pull_mode_to_pull_flag(pull);
        if (inverted) request.flags |= gpiod::line_request::FLAG_ACTIVE_LOW;
        line_.request(request);
    } catch (const std::exception& e) {
        throw std::runtime_error(fmt::format("GPIO {0} (Pin {1}) is already in use. Internal error: {2}",
            pin_info.chip_label,pin_info.offset,e.what()));
    }
    std::string mode_str = (mode_ == PinMode::Input) ? "Input" : "Output";
    std::cout << fmt::format("GPIO {0} => Hardware Pin {1} Successfully Acquired as {2}",gpio_pin_number,pin_info.physical_pin,mode_str) << std::endl;
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