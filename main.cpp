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

    GpioPin dir(20,
                    PinMode::Output,
                    PullMode::None,
                    false);
    GpioPin step(21,
                    PinMode::Output,
                    PullMode::None,
                    false);

    auto m1 = std::make_unique<MoteusController>(1);

    m1->set_absolute_position(0.0);
    auto result = m1->enable();

    auto status = m1->get_current_status();
    std::cout   << " Fault: " << static_cast<int>(status.status_code)
                << " ,Position: ," << status.position
                << " ,Velocity: ," << status.speed << "\n";
    if (result) std::cout<<"Moteus Pos " << result->position << " Fault: " << static_cast<int>(result->status_code) << std::endl;
    
    double increment = 0.01;
    double rotation = 1.1;
    double acceleration_limit = 0.1;
    double max_speed = 2.0;
    result = m1->position_move_absolute(rotation,acceleration_limit,max_speed);
    // auto result = m1->enable();
    while (true) {
        auto status = m1->get_current_status();
        std::cout   << " Fault: " << static_cast<int>(status.status_code)
                    << " ,Position: ," << status.position
                    << " ,Velocity: ," << status.speed << "\n";
        // if (step.read()){
        //     step.write(0);
        // }else{
        //     step.write(1);
        // }
        if ((status.position >= 1 && increment > 0) ||
            (status.position <= -1 && increment < 0)){

            std::cout << "Flip" << increment << std::endl;
            increment *= -1;
            rotation *= -1;
            result = m1->position_move_absolute(rotation,acceleration_limit,max_speed);
            // int current_dir = dir.read();
            // dir.write((1-current_dir));
            std::cout << "Flip" << increment << std::endl;
        }


        val = gpio_pin.read();
        if ((val == false)){
            std::cout<<"Pin is now Low " << val <<std::endl;
            // m1->set_absolute_position(0.0);
            led.write(1);
        } else if (led.read()){
            led.write(0);
            std::cout<<"Pin is now High " << val <<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
