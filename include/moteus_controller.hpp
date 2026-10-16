//ToDo:
/* 
- Make the moteus do the following through the wanted interface
  - The ability to drive to an absolute position at a given speed (deg/s)
  - The ability to drive to a relative position at a given speed (deg/s)
  - The ability to set the current position as zero
*/
#include "moteus.h"
#include "motor_base.hpp"

namespace moteus = mjbots::moteus;
using moteus::Controller;

class MoteusController : public MotorBase {
  public:
    MoteusController(std::shared_ptr<moteus::Transport> transport, int motor_id);
    ~MoteusController();

    std::optional<CurrentStatus> position_move_absolute(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> position_move_relative(double position, double acceleration_limit, double max_speed) override;
    std::optional<CurrentStatus> stop() override;
    std::optional<CurrentStatus> set_absolute_position(double position) override;
    int id;

  private:
    moteus::Controller controller_;
};