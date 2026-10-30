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

Homing::Homing(std::shared_ptr<MotorBase> motor,std::function<bool()> home_check,MoveDir init_dir):
  motor_(std::move(motor)),
  home_check_(std::move(home_check)),
  direction_(init_dir){
}


HomingState Homing::get_homing_state(void){
  return current_state_.load();
}

HomingState Homing::execute_homing(void){
  bool check_outcome = false;
  
  auto start_time = std::chrono::steady_clock::now();

  while (current_state_.load() != HomingState::HOMED){
    switch (current_state_.load()){
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
        return HomingState::HOMED;
        break;
    }

    if (std::chrono::steady_clock::now() - start_time > HOMING_TIMEOUT){
      current_state_.store(HomingState::ERROR_TIMEOUT);
      return HomingState::ERROR_TIMEOUT;
    }
  }
}

void Homing::start_hunting(void){
  position_control(MoveSpeed::FAST);
  direction_ = -direction_;
  current_state_.store(HomingState::HUNTING);
  target_state_ = HomingState::TRIGGERED;
}

void Homing::check_to_start_rehoming(void){
  if (!home_check_()){
    current_state_.store(HomingState::HUNTING);
    target_state_ = HomingState::HOMED;
    direction_ = -direction_;
    position_control(MoveSpeed::SLOW);
  }
}

void Homing::start_backing_off(void){
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
  double target_position = 1.1*static_cast<int>(direction_);
  if (speed == MoveSpeed::SLOW){
    motor_->position_move_relative(target_position,ACCEL_LIMIT,SLOW_SPEED);
  } else {
    motor_->position_move_relative(target_position,ACCEL_LIMIT,FAST_SPEED);
  }
}