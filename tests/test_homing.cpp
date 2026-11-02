
#include <vector>
#include <gtest/gtest.h>
#include <memory>
#include "homing.hpp"
#include "motor_mock.hpp"


// This structure defines a single test scenario layout
enum class ExpectedMotorAction {
    DONT_CARE,
    SHOULD_MOVE_FAST,
    SHOULD_MOVE_SLOW,
    SHOULD_STOP
};

struct HomingTestScenario {
    std::vector<bool> home_switch_states;
    std::vector<HomingState> expected_states;
    std::vector<ExpectedMotorAction> expected_motor_actions;
    std::vector<MoveDir> expected_directions;
};

class HomingParamTest : public ::testing::TestWithParam<HomingTestScenario> {
protected:
    std::shared_ptr<MockMotor> mock_motor;

    void SetUp() override {
        mock_motor = std::make_shared<MockMotor>();
    }
};

INSTANTIATE_TEST_SUITE_P(
  HomingFlowVariations,
  HomingParamTest,
  ::testing::Values(
    // Starting off switch
    HomingTestScenario{
      {false, true, true, false, true}, 
      {HomingState::HUNTING, HomingState::TRIGGERED, 
      HomingState::BACKING_OFF,HomingState::HUNTING,
      HomingState::HOMED},
      {ExpectedMotorAction::SHOULD_MOVE_FAST, ExpectedMotorAction::SHOULD_STOP,
      ExpectedMotorAction::SHOULD_MOVE_SLOW, ExpectedMotorAction::SHOULD_MOVE_SLOW,
      ExpectedMotorAction::SHOULD_STOP},
      {MoveDir::CW, MoveDir::CW, MoveDir::CCW, MoveDir::CW, MoveDir::CW}
    },

    // Starting on switch
    HomingTestScenario{
      {true, true, false, true}, 
      {HomingState::TRIGGERED, HomingState::BACKING_OFF,
      HomingState::HUNTING, HomingState::HOMED},
      {ExpectedMotorAction::SHOULD_STOP,ExpectedMotorAction::SHOULD_MOVE_SLOW, 
      ExpectedMotorAction::SHOULD_MOVE_SLOW, ExpectedMotorAction::SHOULD_STOP},
      {MoveDir::CW, MoveDir::CCW, MoveDir::CW, MoveDir::CW}
    },

    // No switch hit - expect timeout
    HomingTestScenario{
      {false}, 
      {HomingState::ERROR_TIMEOUT},
      {ExpectedMotorAction::DONT_CARE},
      {MoveDir::CW}
    }
  )
);


/**
 * @brief This integration test makes sure the full statmachine flow works correctly
 * 
 */
TEST_P(HomingParamTest, ExecuteSequenceSteps) {
  const auto& scenario = GetParam();
  size_t current_tick = 0;

  // A dynamic sensor lambda that reads our input vector based on the current step
  auto homing_switch = [&current_tick, &scenario]() {
      // Don't tick beyound stored states.
      if (current_tick >= scenario.home_switch_states.size()) {
          return scenario.home_switch_states.back();
      }
      return scenario.home_switch_states[current_tick];
  };

  std::chrono::steady_clock::time_point sim_time = std::chrono::steady_clock::now();
  auto mock_clock = [&sim_time]() { 
      return sim_time; 
  };
  Homing homing(mock_motor, homing_switch, MoveDir::CW, mock_clock);


  // Step through homing
  for (size_t i = 0; i < scenario.expected_states.size(); ++i) {
    if (scenario.expected_states[i] == HomingState::ERROR_TIMEOUT) {
      sim_time += homing.get_timeout() + std::chrono::seconds(1); // Force timeout
    }

    int prior_move_count = mock_motor->move_relative_called_count;
    int prior_stop_count = mock_motor->stop_called_count; 
      
    HomingState actual_state = homing.execute_homing();
    // Use SCOPED_TRACE to provide context in case of failure
    SCOPED_TRACE("Failed at Tick Index: " + std::to_string(i) + 
    ", Expected State: " + std::to_string(static_cast<int>(scenario.expected_states[i])) 
    + ", Actual State: " + std::to_string(static_cast<int>(actual_state)));

    EXPECT_EQ(actual_state, scenario.expected_states[i]);

    // check motor movements
    if (i < scenario.expected_motor_actions.size()) {
      ExpectedMotorAction action = scenario.expected_motor_actions[i];
      
      SCOPED_TRACE("Failed at Tick Index: " + std::to_string(i) +
      ", Expected Motor Movement " + std::to_string((mock_motor->move_relative_called_count>prior_move_count)) +
      ", ExpectedStop Called Count: " + std::to_string((mock_motor->stop_called_count>prior_stop_count)));

      if (action == ExpectedMotorAction::SHOULD_MOVE_FAST) {
        // Verify a move command was fired, and its speed is high
        EXPECT_GT(mock_motor->move_relative_called_count, prior_move_count);
        EXPECT_DOUBLE_EQ(mock_motor->last_move_speed, homing.get_fast_speed());
      }
      else if (action == ExpectedMotorAction::SHOULD_MOVE_SLOW) {
        // Verify a move command was fired, and its speed matches slow tracking limits
        EXPECT_GT(mock_motor->move_relative_called_count, prior_move_count);
        EXPECT_DOUBLE_EQ(mock_motor->last_move_speed, homing.get_slow_speed());
      }
      else if (action == ExpectedMotorAction::SHOULD_STOP) {
        // Verify that the stop function was explicitly invoked
        EXPECT_GT(mock_motor->stop_called_count, prior_stop_count);
      }

    }
    if (i < scenario.expected_directions.size()) {
      MoveDir expected_dir = scenario.expected_directions[i];
      MoveDir actual_dir = homing.get_direction();
      SCOPED_TRACE("Failed at Tick Index: " + std::to_string(i) + ", Expected Direction: " + 
      std::to_string(static_cast<int>(scenario.expected_directions[i])) 
      + ", Actual Direction: " + std::to_string(static_cast<int>(actual_dir)));

      EXPECT_EQ(actual_dir, expected_dir);
    }

    current_tick++;
  }
}
