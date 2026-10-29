#pragma once
#include <algorithm>
#include <cmath>
#include <limits>

/// Trapezoidal motion planner for a step/direction stepper motor.
///
/// Pure maths: no GPIO, no threads, no clock. Time is passed in as `dt`, so the
/// planner can be unit-tested on a PC in "virtual time" (no sleeping).
/// Units everywhere: steps and seconds.
/// Not thread-safe: the owner must serialise access (e.g. with a mutex).
class StepPlanner {
 public:
  struct Limits {
    double max_speed = 0.0;   ///< cruise speed limit, steps/s (> 0 to move)
    double max_accel = 0.0;   ///< acceleration / deceleration limit, steps/s^2 (> 0 to move)
    /// Speed the motor can start or stop at with no ramp, steps/s.
    /// Take it from the datasheet or measure it with your load.
    /// infinity = "no extra limit" (fine in simulation, not on real hardware).
    double start_stop_speed = std::numeric_limits<double>::infinity();
  };

  void set_limits(const Limits& limits) { limits_ = limits; }
  void set_goal(int goal_step) { goal_step_ = goal_step; }
  void set_position(int step) { position_step_ = step; }   // e.g. "set current position as zero"

  int position() const { return position_step_; }
  int goal() const { return goal_step_; }
  double speed() const { return current_speed_; }          // signed, steps/s
  bool idle() const { return position_step_ == goal_step_ && current_speed_ == 0.0; }

  /// Advance the plan by `dt` seconds (dt > 0).
  /// Returns the signed number of whole steps to emit right now
  /// (always -1, 0 or +1, because speed is capped at 1 step per tick).
  int tick(double dt) {
    // 1. Where are we going, and how far is left?
    const double steps_to_go    = static_cast<double>(goal_step_ - position_step_);  // signed
    const double distance_to_go = std::abs(steps_to_go);
    const double direction      = (steps_to_go < 0.0) ? -1.0 : 1.0;

    // 2. Speed limits for this tick.
    const double pulse_rate_limit    = 1.0 / dt;                                   // 1 step per tick
    const double cruise_speed        = std::min(limits_.max_speed, pulse_rate_limit);
    const double braking_speed_limit = std::sqrt(2.0 * limits_.max_accel * distance_to_go);  // v^2 = 2ad

    // 3. Speed we WANT right now (signed): cruise, unless we must already be braking.
    const double desired_speed = direction * std::min(cruise_speed, braking_speed_limit);

    // 4. Speed we HAVE: move current_speed_ toward desired_speed, by at most a*dt per tick.
    const double max_speed_change = limits_.max_accel * dt;
    current_speed_ += std::clamp(desired_speed - current_speed_, -max_speed_change, max_speed_change);
    current_speed_ = std::clamp(current_speed_, -pulse_rate_limit, pulse_rate_limit);

    // 5. At the goal and slow enough for the motor to stop on a step boundary: stop dead.
    //    sqrt(2a) is the speed of the final single step on the braking curve.
    const double settle_speed = std::min(std::sqrt(2.0 * limits_.max_accel) + max_speed_change,
                                         limits_.start_stop_speed);
    if (distance_to_go == 0.0 && std::abs(current_speed_) <= settle_speed) {
      current_speed_  = 0.0;
      step_remainder_ = 0.0;
      return 0;
    }

    // 6. Turn speed into whole steps. The fractional part is kept for later ticks.
    step_remainder_ += current_speed_ * dt;
    const int steps_to_emit = static_cast<int>(step_remainder_);                   // toward zero
    step_remainder_ -= steps_to_emit;
    position_step_  += steps_to_emit;
    return steps_to_emit;
  }

 private:
  Limits limits_{};
  int    position_step_{0};        ///< commanded position (steps emitted so far)
  int    goal_step_{0};
  double current_speed_{0.0};      ///< signed steps/s, the speed we are running at right now
  double step_remainder_{0.0};     ///< steps "earned" by speed but not yet emitted (|x| < 1)
};