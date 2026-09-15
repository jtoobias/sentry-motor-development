#pragma once
#include <gpiod.hpp>
#include <iostream>

/**
 * @brief Purpose of pin.
 * 
 */
enum class PinMode{
  Input = 0,
  Output = 1
};


/**
 * @brief This is the default state pull up/down configuration.
 * 
 */
enum class PullMode{
  None = 0,
  Up = 1,
  Down = 2
};


class GpioPin{
  public:
    GpioPin(const std::string &label,
                int offset,
                PinMode mode,
                PullMode pull = PullMode::None,
                bool inverted = 0);
    ~GpioPin() = default;
    void write(int value);
    int read(void);

    
  protected:
    void execute_write(int value) {
        line_.set_value(value);
    }
    int execute_read() {
        return line_.get_value();
    }

  private:
    int inverted_;
    PinMode mode_;
    gpiod::chip chip_;
    gpiod::line line_;
};
