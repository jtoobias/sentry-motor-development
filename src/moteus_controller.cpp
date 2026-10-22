#include <iostream>
#include <fmt/core.h>
#include <optional>
// #include "spdlog/spdlog.h"
// #include "spdlog/sinks/stdout_color_sinks.h"
// #include "spdlog/async.h"

#include "moteus_controller.hpp"


std::optional<CurrentStatus> handle_moteus_resuls(auto &result){
  if (!result) return std::nullopt;
  CurrentStatus status;
  status.position = result->values.position;
  status.status_code = result->values.fault;
  status.speed = result->values.velocity;
  return status;
}

/**
 * @brief Construct a new Moteus Controller:: Moteus Controller object
 * 
 * @param motor_id - Reference a specific Motor
 */
MoteusController::MoteusController(int motor_id):
  controller_([motor_id]() {
      moteus::Controller::Options options;
      options.id = motor_id;
      return options;
    }()),
    id_(motor_id)
{
  std::cout << fmt::format("Moteus Controller {} Created",id_) << std::endl;
}

MoteusController::~MoteusController(){
  std::cout << "Moteus Controller Removed"<< std::endl; 
}

/**
 * @brief Move to specific position
 * 
 * @param position, 1 = 1 revolution
 * @param acceleration_limit 
 * @param max_speed 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::position_move_absolute(double position, double acceleration_limit, double max_speed){
  moteus::PositionMode::Command cmd;
  cmd.position = position;
  cmd.velocity = 0.0;
  cmd.accel_limit = acceleration_limit;
  cmd.velocity_limit = max_speed;
  auto result = controller_.SetPosition(cmd);
  return handle_moteus_resuls(result);
}

/**
 * @brief Move by x amount
 * 
 * @param pos_move 
 * @param acceleration_limit 
 * @param max_speed 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::position_move_relative(double pos_move, double acceleration_limit, double max_speed){
  // Get current position and offset by wanted move
  auto current_telemetry = controller_.SetQuery({}); 
  if (!current_telemetry) return std::nullopt; 
  double current_pos = current_telemetry->values.position;
  double relative_move = pos_move;

  mjbots::moteus::PositionMode::Command cmd;
  cmd.position = current_pos + pos_move; 
  cmd.velocity = 0.0;
  cmd.velocity_limit = max_speed;
  cmd.accel_limit = acceleration_limit;

  auto result = controller_.SetPosition(cmd);
  
  return handle_moteus_resuls(result);
}

/**
 * @brief Disconnects driver from motor - free spin
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::stop(void){
  auto result = controller_.SetStop();
  return handle_moteus_resuls(result);
}
/**
 * @brief Shorts motor coils together - faster and stop starting
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::brake(void){
  auto result = controller_.SetBrake();
  return handle_moteus_resuls(result);
}

/**
 * @brief Activate coils and ready for position commands
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::enable(void){
  auto result = controller_.SetStop();
  return handle_moteus_resuls(result);
}

/**
 * @brief Sets current position to the wanted position.
 * 
 * @param position 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::set_absolute_position(double position){
  moteus::OutputExact::Command command;
  command.position = position; 
  auto result = controller_.SetOutputExact(command);
  return handle_moteus_resuls(result);
}

/**
 * @brief Get the current motor position
 * 
 * @return double 
 */
double MoteusController::get_position(void){
  auto current_telemetry = controller_.SetQuery({}); 
  if (!current_telemetry) {
    throw std::runtime_error(fmt::format("Motor Position: Unknown - due to communication failure {0}",id_));
  }
  return (current_telemetry->values.position);
}


// void MoteusController::set_software_position_boundaries(double max_position,double min_position){
//   max_position_=max_position;
//   min_position_=min_position;
// }