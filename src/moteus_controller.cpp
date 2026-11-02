#include <iostream>
#include <fmt/core.h>
#include <optional>
#include <limits>
#include "moteus_controller.hpp"

using std::lock_guard;
using std::mutex;

namespace moteus = mjbots::moteus;


// Warning! Caller must hold update_mutex_
std::optional<CurrentStatus> MoteusController::handle_moteus_results(auto &result){
  if (!result) {
    // Make this a one off log
    std::cerr << fmt::format("Moteus Controller {} communication error",id) << std::endl;
    return std::nullopt;
  }
  status_.position = result->values.position;
  status_.status_code = result->values.fault;
  status_.speed = result->values.velocity;

  // After a fault or timeout moteus ignores position commands until it gets a stop
  const bool faulted = result->values.fault != 0 ||
                       static_cast<int>(result->values.mode) == ErrorPositionTimeout;
  // if (faulted && request_.target_state == DriverState::RUNNING) {
  //   request_.target_state = DriverState::ERROR;
  // }
  return status_;
}

CurrentStatus MoteusController::get_current_status(void){
  lock_guard<mutex> lock(update_mutex_);
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
      // enables velocity and accel control
      options.position_format.accel_limit = moteus::kFloat;
      options.position_format.velocity_limit = moteus::kFloat;
      return options;
    }()),
    id(motor_id)
{

  update_thread_ = std::thread(&MoteusController::keepalive, this);
  std::cout << fmt::format("Moteus Controller {} Created",id) << std::endl;
}

MoteusController::~MoteusController(){
  {
    lock_guard<mutex> lock(update_mutex_);
    update_thread_keep_running_.store(false);
  }
  instant_update_.notify_all();
  update_thread_.join();
  // Thread is gone, so it is safe to talk to the controller directly
  controller_.SetStop();
  std::cout << fmt::format("Moteus Controller {} Removed", id) << std::endl;
}

/**
 * @brief Move to specific position
 * 
 * @param position, 1 = 1 revolution
 * @param acceleration_limit 
 * @param max_speed 
 * @return std::optional<CurrentStatus> reply to this command, nullopt on failure or error state
 */
std::optional<CurrentStatus> MoteusController::position_move_absolute(double position, double acceleration_limit, double max_speed){
  lock_guard<mutex> lock(update_mutex_);
  // if (request_.target_state == DriverState::ERROR) {
  //   std::cerr << fmt::format("Moteus Controller {} is in error, call enable() first", id) << std::endl;
  //   return std::nullopt;
  // }
  request_.target_position = position;
  request_.max_acceleration = acceleration_limit;
  request_.max_velocity = max_speed;
  // From here the updater thread keeps this target alive
  request_.target_state = DriverState::RUNNING;
  return update_position();
}

/**
 * @brief Move by x amount
 * 
 * @param pos_move 
 * @param acceleration_limit 
 * @param max_speed 
 * @return std::optional<CurrentStatus> reply to this command, nullopt on failure or error state
 */
std::optional<CurrentStatus> MoteusController::position_move_relative(double pos_move, double acceleration_limit, double max_speed){
  lock_guard<mutex> lock(update_mutex_);
  // if (request_.target_state == DriverState::ERROR) {
  //   std::cerr << fmt::format("Moteus Controller {} is in error, call enable() first", id) << std::endl;
  //   return std::nullopt;
  // }
  // While running, offset from the last target so quick calls don't drift. Otherwise start from the real position
 if (request_.target_state != DriverState::RUNNING) {
    auto query = controller_.SetQuery({});
    if (!handle_moteus_results(query)) return std::nullopt;
    request_.target_position = status_.position;
  }
  request_.target_position += pos_move;
  request_.max_acceleration = acceleration_limit;
  request_.max_velocity = max_speed;
  request_.target_state = DriverState::RUNNING;
  return update_position();
}

/**
 * @brief Disconnects driver from motor - free spin. Also clears an error.
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::stop(void){
  lock_guard<mutex> lock(update_mutex_);
  // State first, so the updater never resends a position after this
  request_.target_state = DriverState::STOPPED;
  auto result = controller_.SetBrake();
  return handle_moteus_results(result);
}

/**
 * @brief Activate coils and ready for position commands. Also clears an error.
 * 
 * @return std::optional<CurrentStatus> 
 */
std::optional<CurrentStatus> MoteusController::enable(void){
  lock_guard<mutex> lock(update_mutex_);
  std::cout << "Enabling Motor: target state " << static_cast<int>(request_.target_state) << std::endl;
  // Make sure to hold at the current position
  if (request_.target_state != DriverState::RUNNING) {
    auto query = controller_.SetQuery({});
    if (!handle_moteus_results(query)) return std::nullopt;
    request_.target_position = status_.position;
  }
  request_.target_state = DriverState::RUNNING;
  auto result = controller_.SetStop();

  std::cout << "Enabled Motor" << std::endl;
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

  lock_guard<mutex> lock(update_mutex_);
  auto result = controller_.SetOutputExact(command);
  auto status = handle_moteus_results(result);
  request_.target_position = position;
  // The held target is in the old frame, so hold at the new origin instead of stopping
  if (status && request_.target_state == DriverState::RUNNING) {
    request_.target_position = position;
    return update_position();
  }
  return status;
}

/**
 * @brief Get the current motor position (fresh query, blocks for one CAN round trip)
 * 
 * @return double position, NaN on communication failure
 */
std::optional<double> MoteusController::get_position(void){
  lock_guard<mutex> lock(update_mutex_);
  auto result = controller_.SetQuery({});
  if (!handle_moteus_results(result)) {
    return std::nullopt;
  }
  return status_.position;
}

/**
 * @brief Keepalive: moteus times out without regular commands, so resend the held position while RUNNING
 */
void MoteusController::keepalive(void){
  while (update_thread_keep_running_.load()) {
    std::unique_lock<mutex> lock(update_mutex_);
    if (request_.target_state == DriverState::RUNNING) {
      update_position();
    }else if (request_.target_state == DriverState::STOPPED) {
      // check to see if this works
      controller_.SetBrake();
    }
    instant_update_.wait_for(lock, update_rate_);
  }
}

// Caller must hold update_mutex_
std::optional<CurrentStatus> MoteusController::update_position(void){
  mjbots::moteus::PositionMode::Command cmd;
  cmd.position = request_.target_position; 
  cmd.velocity = 0.0;
  cmd.velocity_limit = request_.max_velocity;
  cmd.accel_limit = request_.max_acceleration;
  auto result = controller_.SetPosition(cmd);
  return handle_moteus_results(result);
}
