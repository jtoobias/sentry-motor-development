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
StepperController::StepperController(int motor_id, double degrees_per_step):
    id_(motor_id)
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
  status.speed = current_velocity_;
  status.status_code = status_;
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