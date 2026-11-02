/**
 * @file homing.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <chrono>
#include <thread>
#include "homing.hpp"
#include <iostream>

Homing::Homing(std::shared_ptr<MotorBase> motor,
  std::function<bool()> home_check,
  MoveDir init_dir,
  std::function<std::chrono::steady_clock::time_point()> clock):
  motor_(std::move(motor)),
  home_check_(std::move(home_check)),
  direction_(init_dir),
  get_time_(std::move(clock)){
}

HomingState Homing::get_homing_state(void){
  return current_state_.load();
}


HomingState Homing::execute_homing(void) {
    if (current_state_.load() == HomingState::HOMED) {
        return HomingState::HOMED;
    }

    switch (current_state_.load()) {
        case HomingState::UNKNOWN:
            start_hunting();
            break;

        case HomingState::HUNTING:
            home_check_decision();
            break;

        case HomingState::TRIGGERED:
            start_backing_off();
            break;
            
        case HomingState::BACKING_OFF:
            check_to_start_rehoming();
            break;

        case HomingState::HOMED:
            motor_->set_absolute_position(0.0);
            break;
            
        default:
            break;
    }

    if (get_time_() - start_time_ >= HOMING_TIMEOUT) {
        current_state_.store(HomingState::ERROR_TIMEOUT);
        return HomingState::ERROR_TIMEOUT;
    }
    return current_state_.load();
}

void Homing::start_hunting(void){
  start_time_ = std::chrono::steady_clock::now();
  if (home_check_()){
    std::cout << "Homing: Home switch triggered -> backoff.\n";
    position_control(MoveSpeed::STOP);
    current_state_.store(HomingState::TRIGGERED);
    target_state_ = HomingState::TRIGGERED;
  } else {
    std::cout << "Homing: Home switch not triggered.\n";
    position_control(MoveSpeed::FAST);
    current_state_.store(HomingState::HUNTING);
    target_state_ = HomingState::TRIGGERED;
  }
}

void Homing::check_to_start_rehoming(void){
  if (!home_check_()){
    std::cout << "Homing: Start Rehoming.\n";
    current_state_.store(HomingState::HUNTING);
    target_state_ = HomingState::HOMED;
    direction_ = -direction_;
    position_control(MoveSpeed::SLOW);
  }
}

void Homing::start_backing_off(void){

  std::cout << "Homing: Start Backing Off.\n";
  direction_ = -direction_;
  position_control(MoveSpeed::SLOW);
  current_state_.store(HomingState::BACKING_OFF);
}

void Homing::home_check_decision(void){
  if (home_check_()){
    position_control(MoveSpeed::STOP);
    if (target_state_ == HomingState::HOMED){ 
      current_state_.store(HomingState::HOMED);

    }else if(target_state_ == HomingState::TRIGGERED){
      current_state_.store(HomingState::TRIGGERED);
    }
  }
}

void Homing::position_control(MoveSpeed speed){
    // Move the motor at least one revolution
  double target_position = HOMING_ROTATION*static_cast<int>(direction_);

  std::cout << "Homing: Target Position: " << target_position << " Current state: " << static_cast<int>(current_state_.load()) <<  "\n";
  if (speed == MoveSpeed::SLOW){
    motor_->position_move_relative(target_position,ACCEL_LIMIT,SLOW_SPEED);
  } else if (speed == MoveSpeed::FAST){
    motor_->position_move_relative(target_position,ACCEL_LIMIT,FAST_SPEED);
  } else if (speed == MoveSpeed::STOP){
    motor_->stop();
  }
}