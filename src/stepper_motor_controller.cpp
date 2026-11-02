#include <iostream>
#include <fmt/core.h>
#include "stepper_motor_controller.hpp"

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

  std::cout << fmt::format("Stepper Controller {0} Created: rps: {1}",id_,revs_per_step_) << std::endl;
}

StepperController::~StepperController(){
  std::cout << "Stepper Controller Removed"<< std::endl; 
}

/**
 * @brief Move to specific position
 * 
 * @param position, 1 = 1 revolution
 * @param acceleration_limit 
 * @param max_speed 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> StepperController::position_move_absolute(double position, double acceleration_limit, double max_speed){
  CurrentStatus status;
  return status;
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
  // status.speed = current_velocity_.load();
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

bool motion_controller(int position_error){

  // Check difference in position
  // Accelerate at max acceleration by iterative approach, Todo workout acceleration given a 100 pulse difference
  // calculation to convert difference to target pulse count based on thread timing
}

void StepperController::set_direction(int position_error){
  if (position_error > 0){
    dir_.write(0);
  } else {
    dir_.write(1);
  }
}


void StepperController::step_controller(void){
  int difference{0};
    auto nextTick = std::chrono::steady_clock::now();
  while (!stop_requested_.load()) {
    {
      std::lock_guard<std::mutex> lock(step_mutex_);

      difference = target_step_pos_-current_step_pos_;
      if (difference == 0) continue;
      // motion_controller()
    }
        
    // Timer that takes into account work time
    nextTick += std::chrono::milliseconds(thread_step_interval_ms_);
    std::this_thread::sleep_until(nextTick);
  }
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
