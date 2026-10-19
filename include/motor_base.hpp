#include <limits>
//ToDo:
/* Script to home a motor:
- I need a base class for a motor that has the following:
  - The ability to drive to an absolute position at a given speed (rev/s)
  - The ability to drive to a relative position at a given speed (rev/s)
  - The ability to set the current position as zero
*/

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