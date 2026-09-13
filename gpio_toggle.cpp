#include <gpiod.hpp>
#include <iostream>
#include <chrono>
#include <thread>

// Find a chip by its stable hardware label, not its (unstable) chip number
gpiod::chip find_chip_by_label(const std::string &label) {
    for (auto &chip : gpiod::make_chip_iter()) {
        if (chip.label() == label) {
            return chip;
        }
    }
    return gpiod::chip();  // empty/invalid chip if not found
}

int main() {
    gpiod::chip chip = find_chip_by_label("34120000.gpio");
    if (!chip) {
        std::cerr << "Could not find chip with that label\n";
        return 1;
    }

    gpiod::line line = chip.get_line(1);  // your confirmed offset

    line.request({
        "gpio-toggle-example",              // consumer name (shows in gpioinfo)
        gpiod::line_request::DIRECTION_OUTPUT,
        0
    });

    for (int i = 0; i < 100; ++i) {
        line.set_value(1);
        line.get_value();
        std::cout<<"Pin is now high"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        line.set_value(0);
        std::cout<<"Pin is now low"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    line.release();
    return 0;
}
