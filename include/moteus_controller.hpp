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

    CurrentStatus position_move_absolute(float position, float max_speed) override;
    CurrentStatus position_move_relative(float position, float max_speed) override;
    CurrentStatus stop() override;
    CurrentStatus set_absolute_position(float position) override;
    int id;

  private:
    moteus::Controller controller_;
};