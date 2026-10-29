#include <iostream>
#include <fmt/core.h>
#include "stepper_motor_controller.hpp"
#include <cmath>
#include <algorithm>

/**
 * @brief Todo
 * 
 * Thread to manage pulses
 * target pulse count
 */

/**
 * @brief Construct a new Moteus Controller:: Moteus Controller object
 * 
 * @param motor_id - Reference a specific Motor
 */
StepperController::StepperController(int motor_id, double degrees_per_step, int direction_gpio, int step_gpio):
    id_(motor_id),
    dir_(direction_gpio,PinMode::Output),
    step_(step_gpio,PinMode::Output)
{
  revs_per_step_ = degrees_per_step/360.0;
  if (revs_per_step_ == 0.0) {
    throw std::runtime_error(fmt::format("degrees per step can not be zero, {0}",degrees_per_step));
  }

  std::cout << fmt::format("Stepper Controller {0} Created: rps: {1}",id_,revs_per_step_) << std::endl;
}

StepperController::~StepperController(){
  std::cout << "Stepper Controller Removed"<< std::endl; 
}

/**
 * @brief Move by x amount
 * 
 * @param pos_move 
 * @param acceleration_limit 
 * @param max_speed 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> StepperController::position_move_relative(double pos_move, double acceleration_limit, double max_speed){
  CurrentStatus status;
  return status;
}

/**
 * @brief Disconnects driver from motor - free spin
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> StepperController::stop(void){
  CurrentStatus status;
  return status;
}
/**
 * @brief Shorts motor coils together - faster and stop starting
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> StepperController::brake(void){
  CurrentStatus status;
  return status;
}

/**
 * @brief Activate coils and ready for position commands
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> StepperController::enable(void){
  CurrentStatus status;
  return status;
}

/**
 * @brief Sets current position to the wanted position.
 * 
 * @param position 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> StepperController::set_absolute_position(double position){
  CurrentStatus status;
  current_step_pos_ = position;
  status.position = position;
  status.speed = current_velocity_;
  status.status_code = static_cast<int>(status_.load());
  return status;
}

/**
 * @brief Get the current motor revolution position
 * 
 * @return double 
 */
double StepperController::get_rev_position(void){
  return current_step_pos_*revs_per_step_;
}

/**
 * @brief Fetch the current step position
 * 
 * @return double 
 */
double StepperController::get_step_position(void){
  return current_step_pos_;
}

/**
 * @brief Sets the max step position boundary
 * 
 * @param max_steps 
 */
void StepperController::set_max_steps_position(int max_steps){
  max_step_pos_ = max_steps;
  max_rev_position_ = max_steps*revs_per_step_;
}

/**
 * @brief Sets the min step position boundary
 * 
 * @param min_steps 
 */

void StepperController::set_min_steps_position(int min_steps){
  min_step_pos_ = min_steps;
  min_rev_position_ = min_steps*revs_per_step_;
}

void StepperController::set_direction(int position_error){
  if (position_error > 0){
    dir_.write(0);
  } else {
    dir_.write(1);
  }
}

void StepperController::trigger_step(void){
  step_.write(1);
  std::this_thread::sleep_for(std::chrono::microseconds(5));
  step_.write(0);
}


int StepperController::plan_tick(double dt) {
  const double err   = static_cast<double>(target_step_pos_.load() - current_step_pos_.load());
  const double a_max = max_accel_steps_.load();
  const double v_max = max_speed_steps_.load();

  const double v_brake = std::sqrt(2.0 * a_max * std::abs(err));        // v² = 2ad
  const double v_cmd   = std::copysign(std::min(v_max, v_brake), err);
  const double dv_max  = a_max * dt;
  velocity_steps_ += std::clamp(v_cmd - velocity_steps_, -dv_max, dv_max);

  if (err == 0.0 && std::abs(velocity_steps_) < dv_max) {               // settled
    velocity_steps_ = 0.0;
    fractional_steps_ = 0.0;
    return 0;
  }
  
  fractional_steps_ += velocity_steps_ * dt;
  const int steps = static_cast<int>(fractional_steps_);                // toward zero
  fractional_steps_ -= steps;
  return steps;
}

void StepperController::step_controller() {
  auto next = std::chrono::steady_clock::now();
  while (!stop_requested_.load()) {
    const int steps = plan_tick(tick_s_);
    if (steps != 0) {                       // |steps| <= 1 because v_max <= 1/tick_s_
      set_direction(steps);
      trigger_step();                       // spin-based pulse, not sleep_for
      current_step_pos_ += steps;
    }
    next += thread_step_interval_ms_;
    std::this_thread::sleep_until(next);
  }
}

std::optional<CurrentStatus> StepperController::position_move_absolute(
    double position, double accel, double max_speed) {
  const int target = std::clamp(static_cast<int>(std::lround(position / revs_per_step_)),
                                min_step_pos_, max_step_pos_);
  max_accel_steps_ = accel / revs_per_step_;
  max_speed_steps_ = std::min(max_speed / revs_per_step_, 1.0 / tick_s_);
  target_step_pos_ = target;
  CurrentStatus status;
  status.position = get_rev_position();
  return status;
}

/**
 * @brief Begin thread safely. Closing others first.
 * 
 */
void StepperController::start_controller_thread(void) {
    stop_controller_thread();  // make sure nothing stale is still running first
    stop_requested_ = false;
    {
        std::lock_guard<std::mutex> lock(step_mutex_);
    }
    step_controller_thread_ = std::thread(&StepperController::step_controller_thread_, this);
}

/**
 * @brief Stop the watcher thread if it is running.
 * 
 */
void StepperController::stop_controller_thread(void) {
    stop_requested_ = true;
    if (step_controller_thread_.joinable()) {
        step_controller_thread_.join();
    }
}
