#include <iostream>
#include <chrono>
#include <thread>
#include "gpio_wrapper/gpio_pin.hpp"
#include "gpio_wrapper/gpio_wrapper.hpp"
#include <fmt/core.h>

int main() {
    PinRegistry reg;
    const int pin = 11;
    PinLocation pin_loc = reg.pin_map[pin];
    std::cout<<fmt::format("Gpio pin {0} for chip '{1}' for offset {2}",pin,pin_loc.chip_label,pin_loc.offset)
    <<std::endl;
    GpioPin gpio_pin(pin_loc.chip_label,
                    pin_loc.offset,
                    PinMode::Output,
                    PullMode::None,
                    true);


    for (int i = 0; i < 100; ++i) {
        gpio_pin.write(1);
        gpio_pin.read();
        std::cout<<"Pin is now high"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        gpio_pin.write(0);
        std::cout<<"Pin is now low"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    return 0;
}
