#include <iostream>
#include <chrono>
#include <thread>
#include "gpio_wrapper/gpio_pin.hpp"
#include "moteus_controller.hpp"

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

    auto m1 = std::make_unique<MoteusController>(1);
    auto result = m1->enable();
    if (result) std::cout<<"Moteus Pos " << result->position <<std::endl;
    if (result) std::cout<<"Moteus Pos " << result->position <<std::endl;
    


    double increment = 0.01;
    double position = m1->get_position();

    while (true) {
        result = m1->position_move_relative(increment,2.0,1.0);
        if (result) {
        std::cout   << " Fault: " << result->status_code
                    << " Position: " << result->position
                    << " Velocity: " << result->speed << "\n";
        }
        if ((result->position >= -0.8 && increment > 0) ||
            (result->position <= -1.0 && increment < 0)){

        std::cout << "Flip" << increment << std::endl;
        increment *= -1;
        std::cout << "Flip" << increment << std::endl;
        }


        val = gpio_pin.read();
        if ((val == false)){
            std::cout<<"Pin is now Low " << val <<std::endl;
            led.write(1);
        } else if (led.read()){
            led.write(0);
            std::cout<<"Pin is now High " << val <<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
