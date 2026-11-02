#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <atomic>
#include "gpio_wrapper/gpio_pin.hpp"
#include "moteus_controller.hpp"
#include "homing.hpp"

// Global control token to keep the LED indicator loop alive safely
std::atomic<bool> run_application{true};

void led_indicator_loop(GpioPin &sensor, GpioPin &led) {
    while (run_application.load()) {
        // Read your limit switch state directly
        bool switch_hit = sensor.read();
        
        if (switch_hit) {
            led.write(0); // Turn ON LED when home switch is active
        } else {
            led.write(1); // Turn OFF LED when open
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void run_asynchronous_homing(Homing& sequence) {
    std::cout << "[Homing Thread] Asynchronous state execution loop started.\n";
    
    HomingState active_state = HomingState::UNKNOWN;
    
    // Continue ticking until a definitive terminal state is reached
    while (sequence.check_homing_outcome() == HomingOutcome::RUNNING) {
        // Execute exactly one single state-machine step calculation
        active_state = sequence.execute_homing();
        
        // 10ms high-frequency real-time cyclic control loop sleep rate
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "[Homing Thread] Execution loop terminated with state code: " 
              << static_cast<int>(active_state) << "\n";
}

int main() {
    std::cout << "--- Initializing Physical Joint Hardware Layer ---\n";

    // 1. Configure the Physical Pins
    auto gpio_pin = GpioPin(27, PinMode::Input, PullMode::None, false);
    auto led      = GpioPin(17, PinMode::Output, PullMode::None, false);
    
    // 2. Initialize and Enable the Real Moteus Actuator Driver
    auto real_motor = std::make_shared<MoteusController>(1);
    auto init_result = real_motor->enable();
    
    if (!init_result) {
        std::cerr << "CRITICAL: Failed to establish CAN communication with Moteus ID 1!\n";
        return -1;
    }
    std::cout << "Moteus Actuator Connected. Position: " << init_result->position << "\n";

    // 3. Build your sensor function adapter mapping
    // This connects your physical GpioPin reading method straight to the state engine slot
    auto hardware_switch_checker = [&gpio_pin]() {
        return !gpio_pin.read(); 
    };

    Homing homing_sequence(real_motor, hardware_switch_checker, MoveDir::CW);

    // Visual showing
    std::thread led_thread(led_indicator_loop, std::ref(gpio_pin), std::ref(led));
    

    std::thread homing_thread(run_asynchronous_homing, std::ref(homing_sequence));


    while (true) {
        HomingState current_state = homing_sequence.get_homing_state();
        std::cout << "[Monitor Loop] Current State: " << static_cast<int>(current_state) << "\n";
        HomingOutcome homing_outcome = homing_sequence.check_homing_outcome();
        if (homing_outcome == HomingOutcome::SUCCESS) {
            std::cout << "SUCCESS: Joint Homing Routine verified physically! Hardware zeroed.\n";
            break;
        } 
        else if (homing_outcome == HomingOutcome::FAILURE) {
            std::cerr << "FATAL ERROR: Homing execution dropped due to Watchdog Safety Timeout!\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // Test absolute position move
    int count = 0;
    real_motor->position_move_absolute(0.25, 0.1, 2.0);
    while (count < 5) {
        auto status = real_motor->get_current_status();
        std::cout   << " Fault: " << static_cast<int>(status.status_code)
                    << " ,Position: ," << status.position
                    << " ,Velocity: ," << status.speed << "\n";

        if (status.position >= 0.25) {
            std::cout << "Reached 0.25 revs, moving back to 0.0\n";
            real_motor->position_move_absolute(-0.25, 0.1, 2.0);
        } else if (status.position <= -0.25) {
            std::cout << "Reached -0.25 revs, moving back to 0.0\n";
            real_motor->position_move_absolute(0.25, 0.1, 2.0);
            count ++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    run_application.store(false); // Shutdown the LED loop flag
    
    if (homing_thread.joinable()) homing_thread.join();
    if (led_thread.joinable()) led_thread.join();

    std::cout << "System shutting down cleanly. Ready for operational trajectories.\n";
    return 0;
}
