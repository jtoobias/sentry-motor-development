/**
 * @file motor_base.hpp
 * @author Jonathan Tobias
 * @brief 
 * @version 0.1
 * @date 2026-10-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once 
#include <limits>
#include <optional>

struct CurrentStatus{
  double position = std::numeric_limits<double>::quiet_NaN();
  double speed = std::numeric_limits<double>::quiet_NaN();
  int8_t status_code = std::numeric_limits<int8_t>::quiet_NaN();
};

class MotorBase{
  public:

    MotorBase() = default;
    virtual ~MotorBase() = default;
    /**
     * @brief Move to an absolute position revolutions.
     * 
     * @param Position (pos) can be negative to determine wanted direction of travel.
     * Clock Wise (CW): +ve pos and Counter-Clock wise (CCW) -ve pos.
     * 
     * @param Max speed: Is in rev/s (user friendly)
     */
    virtual std::optional<CurrentStatus> position_move_absolute(double position,double acceleration_limit, double max_speed)=0;

    /**
     * @brief Move to an relative position from current position
     * 
     * @param Position (pos) can be negative to determine wanted direction of travel.
     * Clock Wise (CW): +ve pos and Counter-Clock wise (CCW) -ve pos.
     * 
     * @param Max speed: Is in rev/s (user friendly)
     */
    virtual std::optional<CurrentStatus> position_move_relative(double position,double acceleration_limit, double max_speed)=0;

    /**
     * @brief Stop all motion and disconnect
     */
    virtual std::optional<CurrentStatus> stop(void)=0;

    /**
     * @brief enable the motor so it is ready to move
     */
    virtual std::optional<CurrentStatus> enable(void)=0;

    /**
     * @brief Set the absolute position object
     * 
     * @param position 
     * @return CurrentStatus 
     */
    virtual std::optional<CurrentStatus> set_absolute_position(double position)=0;

};