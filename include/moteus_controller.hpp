//ToDo:
/* 
- Make the moteus do the following through the wanted interface
  - The ability to drive to an absolute position at a given speed (deg/s)
  - The ability to drive to a relative position at a given speed (deg/s)
  - The ability to set the current position as zero
  - enable function
  - Set software pos limits (pre-configured)
  - add logger
*/
#pragma once 
#include <memory>
#include "moteus.h"
#include "motor_base.hpp"

namespace moteus = mjbots::moteus;
using moteus::Controller;

class MoteusController : public MotorBase {
  public:
    MoteusController(int motor_id);
    ~MoteusController();

    std::optional<CurrentStatus> position_move_absolute(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> position_move_relative(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> stop() override;
    std::optional<CurrentStatus> enable() override;
    std::optional<CurrentStatus> set_absolute_position(double position) override;
    std::optional<CurrentStatus> brake(void);
    double get_position(void);
    // void set_software_position_boundaries(double max_position,double min_position);

    int id_;

  private:
    moteus::Controller controller_;
    double max_position_=std::numeric_limits<double>::max();
    double min_position_=std::numeric_limits<double>::min();
};