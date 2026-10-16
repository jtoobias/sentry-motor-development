//ToDo:
/* Script to home a motor:
- I need a base class for a motor that has the following:
  - The ability to drive to an absolute position at a given speed (deg/s)
  - The ability to drive to a relative position at a given speed (deg/s)
  - The ability to set the current position as zero
*/

struct CurrentStatus{
  float position;
  float speed;
  float status_code;
};

class MotorBase{
  public:

    MotorBase() = default;
    virtual ~MotorBase() = default;
    /**
     * @brief Move to an absolute position
     * 
     * @param Position (pos) can be negative to determine wanted direction of travel.
     * Clock Wise (CW): +ve pos and Counter-Clock wise (CCW) -ve pos.
     * 
     * @param Max speed: Is in deg/s (user friendly)
     */
    virtual CurrentStatus position_move_absolute(float position, float max_speed)=0;

    /**
     * @brief Move to an relative position from current position
     * 
     * @param Position (pos) can be negative to determine wanted direction of travel.
     * Clock Wise (CW): +ve pos and Counter-Clock wise (CCW) -ve pos.
     * 
     * @param Max speed: Is in deg/s (user friendly)
     */
    virtual CurrentStatus position_move_relative(float position, float max_speed)=0;

    /**
     * @brief Stop all mostion and break if possible
     */
    virtual CurrentStatus stop(void)=0;

    /**
     * @brief Set the absolute position object
     * 
     * @param position 
     * @return CurrentStatus 
     */
    virtual CurrentStatus set_absolute_position(float position)=0;

};