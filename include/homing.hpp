/**
 * @file homing.hpp
 * @author your name (you@domain.com)
 * @brief Run the base homing statemachine. Takes a generic motor and trigger method.
 * Potientially different homing setup based which motor is being homed.
 * Homing will run in a thread and have a state flag that can be polled in the controlling code
 * @version 0.1
 * @date 2026-10-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once 
#include <atomic>
#include <functional>
#include <memory>
#include <chrono>
#include "motor_base.hpp"

enum class HomingState{
  UNKNOWN = 0,
  HUNTING = 1,
  TRIGGERED = 2,
  BACKING_OFF = 3,
  HOMED = 4,
  ERROR_TIMEOUT = 10
};

enum class MoveSpeed{
  STOP = 0,
  SLOW = 1,
  FAST = 2
};

enum class MoveDir{
  CW = 1,
  CCW = -1
};

/**
 * @brief changes -ve opetator to flip the direction
 * 
 * @param dir 
 * @return MoveDir 
 */
inline MoveDir operator-(MoveDir dir) {
    return static_cast<MoveDir>(-static_cast<int>(dir));
}

class Homing{
  public:

    // Homing constructor provides the REAL clock as a default argument!
    Homing(std::shared_ptr<MotorBase> motor, 
            std::function<bool()> home_check, 
            MoveDir init_dir,
            std::function<std::chrono::steady_clock::time_point()> clock = []() { 
                return std::chrono::steady_clock::now(); 
            });
    ~Homing()=default;
    HomingState get_homing_state(void);
    HomingState execute_homing(void);

    MoveDir get_direction() const { return direction_; }
    double get_slow_speed() const { return SLOW_SPEED; }
    double get_fast_speed() const { return FAST_SPEED; }
    std::chrono::seconds get_timeout() const { return HOMING_TIMEOUT; }

  private:
    static constexpr double SLOW_SPEED = 0.1; //rev/s
    static constexpr double FAST_SPEED = 1.0; //rev/s
    static constexpr double ACCEL_LIMIT = 10.0; // rev/s^2
    static constexpr double HOMING_ROTATION = 1.1; // revs
    static constexpr std::chrono::seconds HOMING_TIMEOUT = std::chrono::seconds(60);
    std::chrono::steady_clock::time_point start_time_; 
    std::chrono::steady_clock::time_point timer_; 
    // Done to make testing easier
    std::function<std::chrono::steady_clock::time_point()> get_time_;

    std::shared_ptr<MotorBase> motor_;
    MoveDir direction_ = MoveDir::CW;
    HomingState target_state_ = HomingState::HUNTING;


    bool execute_move(MoveDir direction ,MoveSpeed speed);
    void position_control(MoveSpeed speed);

      // Homing state functions
    void start_hunting(void);
    void check_to_start_rehoming(void);
    void start_backing_off(void);
    void home_check_decision(void);

    std::function<bool()> home_check_; 
    std::atomic<HomingState> current_state_{HomingState::UNKNOWN};

};