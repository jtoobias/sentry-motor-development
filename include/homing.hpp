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

    Homing(std::shared_ptr<MotorBase> motor,std::function<bool()> home_check,MoveDir init_dir);
    ~Homing();
    HomingState get_homing_state(void);
    HomingState execute_homing(void);

  private:
    static constexpr double SLOW_SPEED = 0.1; //rev/s
    static constexpr double FAST_SPEED = 1.0; //rev/s
    static constexpr double ACCEL_LIMIT = 10.0; // rev/s^2
    static constexpr std::chrono::seconds HOMING_TIMEOUT = std::chrono::seconds(60);

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