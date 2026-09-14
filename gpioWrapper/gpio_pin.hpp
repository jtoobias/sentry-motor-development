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
 * @brief This is the default state of the pin.
 * 
 */
enum class DefaultState{
  Low = 0,
  High = 1
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


struct PinConfig{
  int pin;
  PinMode mode;
  PullMode pull;
  DefaultState default_state;
  bool inverted;
};

class GpioPin{
  public:
    GpioPin(const std::string &label, int offset);
    virtual ~GpioPin() = default;  // Gets overridden by child classes
    void configure(const PinConfig& config);
    virtual void write(int value);
    virtual int read(void);
    
  protected:
    void execute_write(int value) {
        line_.set_value(value);
    }
    int execute_read() {
        return line_.get_value();
    }

  private:
    gpiod::chip find_chip_by_label(const std::string &label);

    int inverted_;
    gpiod::chip chip_;
    gpiod::line line_;
};

class InputPin : public GpioPin{
  public:
    int read(void) override;
    
  private:
};

class OutputPin : public GpioPin{

  public:
    void write(int value) override;
    
  private:
  
};