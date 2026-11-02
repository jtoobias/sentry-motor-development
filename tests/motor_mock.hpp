#include <iostream>
#include "motor_base.hpp" // Wherever your MotorBase definition lives

class MockMotor : public MotorBase {
public:
    // Tracking Variables for position_move_relative
    int move_relative_called_count = 0;
    int move_absolute_called_count = 0;
    double last_move_distance = 0.0;
    double last_move_accel = 0.0;
    double last_move_speed = 0.0;

    // Tracking Variables for set_absolute_position
    int set_absolute_position_called_count = 0;
    double last_absolute_position_set = -1.0;
    int stop_called_count = 0;

    // Override the interface methods
    std::optional<CurrentStatus> set_absolute_position(double pos) override {
        set_absolute_position_called_count++;
        last_absolute_position_set = pos;
        return std::nullopt;
    }

    std::optional<CurrentStatus> position_move_relative(double distance, double accel, double speed) override {
        move_relative_called_count++;
        last_move_distance = distance;
        last_move_accel = accel;
        last_move_speed = speed;
        return std::nullopt;
    }


    std::optional<CurrentStatus> position_move_absolute(double distance, double accel, double speed) override {
        move_absolute_called_count++;
        last_move_distance = distance;
        last_move_accel = accel;
        last_move_speed = speed;
        return std::nullopt;
    }

    std::optional<CurrentStatus> stop(void) override {
        stop_called_count++;
        return std::nullopt;
    }

    std::optional<CurrentStatus> enable(void) override {
        return std::nullopt;
    }

    // Helper method to easily reset the mock data between tests
    void reset_mock() {
        move_relative_called_count = 0;
        last_move_distance = 0.0;
        last_move_accel = 0.0;
        last_move_speed = 0.0;
        set_absolute_position_called_count = 0;
        last_absolute_position_set = -1.0;
    }
};