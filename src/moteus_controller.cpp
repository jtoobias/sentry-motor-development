#include <iostream>
#include <fmt/core.h>
#include <optional>

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
 * @param transport - Only one CAN transport for all devices
 * @param motor_id - Reference a specific Motor
 */
MoteusController::MoteusController(std::shared_ptr<moteus::Transport> transport, int motor_id):
  controller_([transport, motor_id]() {
      moteus::Controller::Options options;
      options.id = motor_id;
      options.transport = transport;
      return options;
    }()),
    id(motor_id)
{
  std::cout << fmt::format("Moteus Controller {} Created",id) << std::endl;
};

MoteusController::~MoteusController(){
  std::cout << "Moteus Controller Removed"<< std::endl; 
};

std::optional<CurrentStatus> MoteusController::position_move_absolute(double position, double acceleration_limit, double max_speed){
  moteus::PositionMode::Command cmd;
  cmd.position = position;
  cmd.velocity = 0.0;
  cmd.accel_limit = acceleration_limit;
  cmd.velocity_limit = max_speed;
  auto result = controller_.SetPosition(cmd);
  return handle_moteus_resuls(result);
}

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

std::optional<CurrentStatus> MoteusController::stop(void){
  auto result = controller_.SetStop();
  return handle_moteus_resuls(result);
}

std::optional<CurrentStatus> MoteusController::set_absolute_position(double position){
  moteus::OutputExact::Command command;
  command.position = position; 
  auto result = controller_.SetOutputExact(command);
  return handle_moteus_resuls(result);
}