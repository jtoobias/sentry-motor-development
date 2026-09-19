#pragma once
#include <gpiod.hpp>
#include <iostream>
#include <unordered_map>
#include <fmt/core.h>

using std::unordered_map;
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

struct PinLocation {
    std::string chip_label;
    unsigned int offset;
    unsigned int physical_pin;
};


class GpioPin{
  public:
    GpioPin(int gpio_pin_number,
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
    const PinLocation& get_gpio_details(int gpio_pin_number);
    int inverted_;
    PinMode mode_;
    gpiod::chip chip_;
    gpiod::line line_;
    unordered_map<int, PinLocation> gpio_map_ = {
        {2,  {"34120000.gpio", 8,  3}},   // I2C5_SDA
        {3,  {"34120000.gpio", 10, 5}},   // I2C5_SCL
        {4,  {"32150000.gpio", 9,  7}},   // I2S1_MCLK
        {14, {"34120000.gpio", 4,  8}},   // UART_TXD
        {15, {"34120000.gpio", 5,  10}},  // UART_RXD
        {17, {"34120000.gpio", 1,  11}},
        {18, {"32150000.gpio", 10, 12}},  // I2S1_BCLK
        {27, {"34120000.gpio", 0,  13}},
        {22, {"34120000.gpio", 9,  15}},
        {23, {"34120000.gpio", 3,  16}},
        {24, {"34120000.gpio", 23, 18}},
        {10, {"34120000.gpio", 19, 19}},  // SPI1_MOSI
        {9,  {"34120000.gpio", 18, 21}},  // SPI1_MISO
        {25, {"34120000.gpio", 8,  22}},  // shares line with GPIO2, see mux note
        {11, {"34120000.gpio", 16, 23}},  // SPI1_SCLK
        {8,  {"34120000.gpio", 15, 24}},  // SPI1_CSN0
        {7,  {"34120000.gpio", 17, 26}},  // SPI1_CSN1
        {0,  {"34130000.gpio", 8,  27}},  // I2C0_SDA
        {1,  {"34130000.gpio", 7,  28}},  // I2C0_SCL
        {5,  {"34120000.gpio", 20, 29}},
        {6,  {"34120000.gpio", 21, 31}},
        {12, {"34130000.gpio", 9,  32}},  // PWM6
        {13, {"34130000.gpio", 10, 33}},  // PWM7
        {19, {"32150000.gpio", 11, 35}},  // I2S1_LRCK -- typo fixed: was 34150000.gpio
        {16, {"34120000.gpio", 2,  36}},
        {26, {"32150000.gpio", 22, 37}},
        {20, {"32150000.gpio", 12, 38}},  // I2S1_SDIN
        {21, {"32150000.gpio", 13, 40}},  // I2S1_SDOUT
    };
};
