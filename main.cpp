#include <iostream>
#include <chrono>
#include <thread>
#include "gpio_wrapper/gpio_pin.hpp"

int main() {
    int val = 0;
    GpioPin gpio_pin(27,
                    PinMode::Input,
                    PullMode::None,
                    false);

    GpioPin led(17,
                    PinMode::Output,
                    PullMode::None,
                    false);

    while (true) {
        val = gpio_pin.read();
        if ((val == false)){
            std::cout<<"Pin is now Low " << val <<std::endl;
            led.write(1);
        } else if (led.read()){
            led.write(0);
            std::cout<<"Pin is now High " << val <<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
