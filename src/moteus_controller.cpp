#include <iostream>
#include "moteus_controller.hpp"
#include <fmt/core.h>

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

CurrentStatus MoteusController::position_move_absolute(float position, float max_speed){
  CurrentStatus status;
  return status;
}

CurrentStatus MoteusController::position_move_relative(float position, float max_speed){
  CurrentStatus status;
  return status;
}

CurrentStatus MoteusController::stop(void){
  CurrentStatus status;
  return status;
}

CurrentStatus MoteusController::set_absolute_position(float position){
  CurrentStatus status;
  return status;
}