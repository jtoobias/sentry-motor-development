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
#include <optional>
#include "motor_base.hpp"
#include <atomic>



enum class Direction{
  CW = 0,
  CCW = 1,
};

enum class StatusCodes{
  UNKNOWN = 0,
  RUNNING = 1,
  STOPPED = 2,
  ALERT = 3,
};

class StepperController : public MotorBase {
  public:
    StepperController(int motor_id, double degrees_per_step);
    ~StepperController();

    std::optional<CurrentStatus> position_move_absolute(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> position_move_relative(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> stop() override;
    std::optional<CurrentStatus> enable() override;
    std::optional<CurrentStatus> set_absolute_position(double position) override;
    std::optional<CurrentStatus> brake(void);
    double get_rev_position(void); // calculated from the step position
    double get_step_position(void);
    void set_step_position(int current_steps);
    void set_max_steps_position(int max_steps); // setting this sets the rev max as well
    void set_min_steps_position(int min_steps);

    int id_;

  private:
    std::atomic<int> current_step_pos_{0};
    int max_step_pos_ = std::numeric_limits<int>::max();
    int min_step_pos_ = std::numeric_limits<int>::min();
    double max_rev_position_=std::numeric_limits<double>::max();
    double min_rev_position_=std::numeric_limits<double>::min();
    double revs_per_step_{0};

    std::atomic<double> target_velocity_{0.0}; // revs/second
    std::atomic<double> current_velocity_{0.0}; // revs/second

    std::atomic<int> status_{0};
};