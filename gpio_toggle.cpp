#include <iostream>
#include <chrono>
#include <thread>
#include "gpio_wrapper/gpio_pin.hpp"

int main() {
    int val = 0;
    GpioPin gpio_pin(17,
                    PinMode::Output,
                    PullMode::None,
                    false);


    for (int i = 0; i < 100; ++i) {
        gpio_pin.write(1);
        val = gpio_pin.read();
        std::cout<<"Pin is now high " << val <<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        gpio_pin.write(0);
        val = gpio_pin.read();
        std::cout<<"Pin is now low " << val <<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}
