#include <gpiod.hpp>
#include <iostream>

constexpr int GPIO_MODE_INPUT = 0;
constexpr int GPIO_MODE_OUTPUT = 1;
constexpr int GPIO_MODE_PWM = 2;

class GpioPin{
  public:
    virtual ~GpioPin() = default;  // Gets overridden by child classes
    void configure(int mode, bool invert = false, bool pin_state = false);

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