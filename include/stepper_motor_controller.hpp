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
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "gpio_wrapper/gpio_pin.hpp"


enum class Direction{
  CW = 1,
  CCW = -1,
};

enum class StatusCodes{
  UNKNOWN = 0,
  RUNNING = 1,
  STOPPED = 2,
  ALERT = 3
};

class StepperController : public MotorBase {
  public:
    StepperController(int motor_id, double degrees_per_step, int direction_gpio, int step_gpio);
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


  private:

    int id_;
    GpioPin dir_;
    GpioPin step_;
    int max_step_pos_ = std::numeric_limits<int>::max();
    int min_step_pos_ = std::numeric_limits<int>::lowest();
    double max_rev_position_=std::numeric_limits<double>::max();
    double min_rev_position_=std::numeric_limits<double>::lowest();
    double revs_per_step_{0};

    std::atomic<int>    current_step_pos_{0};
    std::atomic<int>    target_step_pos_{0};
    std::atomic<double> max_speed_steps_{0.0};   // steps/s
    std::atomic<double> max_accel_steps_{0.0};   // steps/s²
    double velocity_steps_{0.0};                 // steps/s, controller thread only
    double fractional_steps_{0.0};               // earned but not yet emitted steps
    static constexpr std::chrono::milliseconds thread_step_interval_ms_{1};
    static constexpr double tick_s_ = std::chrono::duration<double>(thread_step_interval_ms_).count();

int plan_tick(double dt);                    // returns signed steps to emit

    void step_controller(void);
    void start_controller_thread(void);
    void stop_controller_thread(void);
    void set_direction(int position_error);
    void trigger_step(void);

    double target_velocity_{0.0}; // revs/second
    double current_velocity_{0.0}; // revs/second
    double max_acceleration_{0.0};

    std::atomic<StatusCodes> status_{StatusCodes::UNKNOWN};

    std::thread step_controller_thread_;
    std::atomic<bool> stop_requested_{false};
    std::condition_variable step_controller_cv_;
    mutable std::mutex step_mutex_;
};