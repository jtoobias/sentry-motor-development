#include <iostream>
#include <fmt/core.h>
#include <optional>
#include "moteus_controller.hpp"

using std::lock_guard;
using std::mutex;

namespace moteus = mjbots::moteus;

std::optional<CurrentStatus> MoteusController::handle_moteus_results(auto &result){
  if (!result) {
    // Make this a one off log
    std::cerr << fmt::format("Moteus Controller {} communication error",id) << std::endl;
    return std::nullopt;
  }
  status_.position = result->values.position;
  status_.status_code = result->values.fault;
  status_.speed = result->values.velocity;
  return status_;
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
    id(motor_id)
{
  update_thread_ = std::thread(&MoteusController::motor_updater, this);
  std::cout << fmt::format("Moteus Controller {} Created",id) << std::endl;
}

MoteusController::~MoteusController(){
  update_thread_keep_running_.store(false);
  update_thread_.join();
  std::cout << fmt::format("Moteus Controller {} Removed", id) << std::endl;
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
  return handle_moteus_results(result);
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
  
  return handle_moteus_results(result);
}

/**
 * @brief Disconnects driver from motor - free spin
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::stop(void){
  auto result = controller_.SetStop();
  return handle_moteus_results(result);
}
/**
 * @brief Shorts motor coils together - faster and stop starting
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::brake(void){
  auto result = controller_.SetBrake();
  return handle_moteus_results(result);
}

/**
 * @brief Activate coils and ready for position commands
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::enable(void){
  auto result = controller_.SetStop();
  return handle_moteus_results(result);
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
  return handle_moteus_results(result);
}

/**
 * @brief Get the current motor position
 * 
 * @return double 
 */
double MoteusController::get_position(void){
  // Start query
  request_.target_state = DriverState::QUERY;
  update_tick_complete.store(true);
  instant_update_.notify_all();

  // Wait for response
  std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
  while (!update_tick_complete.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto now = std::chrono::steady_clock::now();
    if ((now - start_time) > std::chrono::milliseconds(100)) {
      std::cerr << fmt::format("Moteus Controller {} get_position timeout",id) << std::endl;
      return std::numeric_limits<double>::quiet_NaN();
    }
  }
  return status_.position;
}


void MoteusController::motor_updater(void){
  moteus::Optional<moteus::Controller::Result> result;
  DriverState last_state{DriverState::UNKNOWN};
  while (update_thread_keep_running_.load()) {
    {
      std::unique_lock<mutex> lock(update_mutex_);

      switch (request_.target_state){ 
        case DriverState::ENABLED:
        case DriverState::STOPPED:
          result = controller_.SetStop();
          handle_moteus_results(result);
          request_.target_position = status_.position;
          last_state = request_.target_state;
          break;
        case DriverState::RUNNING:
          update_position();
          break;
        case DriverState::BRAKED:
          result = controller_.SetBrake();
          handle_moteus_results(result);
          request_.target_position = status_.position;
          last_state = request_.target_state;
          break;
        case DriverState::QUERY:
          result = controller_.SetQuery({});
          handle_moteus_results(result);
          // Go back to the last active state
          request_.target_state = last_state;
          break;
        case DriverState::UNKNOWN:
          break;
        default:
        
          break;
      }
      update_tick_complete.store(true);
      instant_update_.wait_for(lock, std::chrono::milliseconds(update_rate_.count()));
    }
  }
}

std::optional<CurrentStatus> MoteusController::update_position(void){
  CurrentStatus status;
  mjbots::moteus::PositionMode::Command cmd;
  cmd.position = request_.target_position; 
  cmd.velocity = 0.0;
  cmd.velocity_limit = request_.max_velocity;
  cmd.accel_limit = request_.max_acceleration;

  auto result = controller_.SetPosition(cmd);
  return handle_moteus_results(result);
}