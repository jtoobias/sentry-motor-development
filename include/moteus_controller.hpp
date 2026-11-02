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
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include "moteus.h"
#include "motor_base.hpp"

namespace moteus = mjbots::moteus;
using moteus::Controller;

enum class DriverState {
  UNKNOWN = 0,
  RUNNING = 1,
  STOPPED = 2,
  ERROR = 10
};

struct Request {
  double target_position{0.0};
  double max_acceleration{0.0};
  double max_velocity{0.0};
  DriverState target_state{DriverState::UNKNOWN};
};


class MoteusController : public MotorBase {
  public:
    MoteusController(int motor_id);
    ~MoteusController();

    std::optional<CurrentStatus> position_move_absolute(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> position_move_relative(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> stop() override;
    std::optional<CurrentStatus> enable() override;
    std::optional<CurrentStatus> set_absolute_position(double position) override;
    std::optional<double> get_position(void);
    CurrentStatus get_current_status(void);
    // void set_software_position_boundaries(double max_position,double min_position);

    int id;

  private:
    std::optional<CurrentStatus> handle_moteus_results(auto &result);
    std::optional<CurrentStatus> update_position(void);
    void keepalive(void);
    CurrentStatus status_;
    moteus::Controller controller_;
    Request request_;
    double max_position_=std::numeric_limits<double>::max();
    double min_position_=std::numeric_limits<double>::min();

    
    static constexpr int ErrorPositionTimeout = 11;

    static constexpr std::chrono::milliseconds update_rate_{25}; // must be less than 50ms
    std::mutex update_mutex_;
    std::thread update_thread_;
    std::condition_variable instant_update_;
    std::atomic<bool> update_thread_keep_running_{true};
    std::atomic<bool> update_tick_complete{false};
};