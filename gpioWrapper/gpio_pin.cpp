#include "gpio_pin.hpp"

  /**
   * @brief Set pin up as a input/output with configurations
   * 
   * input is standard digital, output can be digitial output/PWM
   * Each can have the default state set and 
   * also inversion can be set if wanted.
   * 
   * @param mode 
   * @param invert 
   * @param pin_state 
   */
  void GpioPin::configure(int mode, bool invert = false, bool pin_state = false){
    
  }

  void GpioPin::write(int value) {
      throw std::logic_error("write() not supported on this pin's mode");
  }

  int GpioPin::read(void) {
      throw std::logic_error("read() not supported on this pin's mode");
  }

  void GpioPin::execute_write(int value) {
      line_.set_value(value);
  }

  int GpioPin::execute_read(void) {
      return line_.get_value();
  }

  /**
   * @brief Writes the value to the pin.
   * 
   * @param state 
   */
  void OutputPin::write(int value){
    execute_write(value);
  }

  /**
   * @brief Reads the raw value from the pin.
   * 
   * @return int 
   */
  int InputPin::read(void){
    return execute_read();
  }
