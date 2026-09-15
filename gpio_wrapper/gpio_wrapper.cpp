#include "gpio_wrapper.hpp"
#include <iostream>
#include <gpiod.hpp>

namespace gpio {
    void say_hello() {
        std::cout << "Hello from my custom library!" << std::endl;
    }
    
}
