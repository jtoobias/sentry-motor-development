#pragma once

#include <iostream>
#include <stdlib.h>
#include <unordered_map>
#include <string.h>
#include "gpio_pin.hpp"

using std::unordered_map;
using std::string;

struct PinLocation {
    std::string chip_label;  // e.g. "34120000.gpio"
    unsigned int offset;     // e.g. 1
};

class PinRegistry{
    public:

    private:
    unordered_map<int, PinLocation> pin_map = {
        {3,  {"34120000.gpio", 8}},   // I2C5_SDA -- shares line with pin 22 (GPIO25), see TODO
        {5,  {"34120000.gpio", 10}},  // I2C5_SCL
        {7,  {"32150000.gpio", 9}},   // I2S1_MCLK
        {8,  {"34120000.gpio", 4}},   // UART_TXD
        {10, {"34120000.gpio", 5}},   // UART_RXD
        {11, {"34120000.gpio", 1}},   // GPIO17 -- confirmed working
        {12, {"32150000.gpio", 10}},  // I2S1_BCLK
        {13, {"34120000.gpio", 0}},   // GPIO27
        {15, {"34120000.gpio", 9}},   // GPIO22
        {16, {"34120000.gpio", 3}},   // GPIO23
        {18, {"34120000.gpio", 23}},  // GPIO24
        {19, {"34120000.gpio", 19}},  // SPI1_MOSI
        {21, {"34120000.gpio", 18}},  // SPI1_MISO
        {22, {"34120000.gpio", 8}},   // GPIO25 -- shares line with pin 3 (I2C5_SDA), see TODO
        {23, {"34120000.gpio", 16}},  // SPI1_SCLK
        {24, {"34120000.gpio", 17}},  // SPI1_SSN0
        {26, {"34120000.gpio", 15}},  // SPI1_SSN1
        {27, {"34130000.gpio", 8}},   // I2C0_SDA
        {28, {"34130000.gpio", 7}},   // I2C0_SCL
        {29, {"34120000.gpio", 20}},  // GPIO5
        {31, {"34120000.gpio", 21}},  // GPIO6
        {32, {"34130000.gpio", 9}},   // PWM6
        {33, {"34130000.gpio", 10}},  // PWM7
        {35, {"34120000.gpio", 22}},  // GPIO26
        {36, {"34120000.gpio", 2}},   // GPIO16
        {37, {"32150000.gpio", 12}},  // I2S1_SDIN
        {38, {"32150000.gpio", 13}},  // I2S1_SDOUT
    };
    // Define and fill the map with elements

    // pin_number -> PinLocation{chip_label, offset}
    // TODO: pins 3 and 22 physically share the same SoC line (I2C5_SDA / GPIO25 mux).
    //       Claiming one currently locks out the other at the hardware level.
    //       Consider adding a conflict-tracking field to PinLocation so the
    //       registry/factory can warn on this before attempting a request.

};