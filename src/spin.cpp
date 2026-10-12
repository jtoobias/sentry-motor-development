#include <iostream>
#include <limits>
#include <unistd.h>
#include "moteus.h"

namespace moteus = mjbots::moteus;

int main(int argc, char** argv) {
  moteus::Controller::DefaultArgProcess(argc, argv);

  moteus::Controller controller([]() {
    moteus::Controller::Options options;
    options.id = 1;
    return options;
  }());

  moteus::PositionMode::Command command;

  const auto query = controller.SetQuery();
    if (query) {
        // Access values inside the returned structure
        const auto& v = query->values;
        command.position = v.position;
        std::cout << "Current Motor Position: " << v.position << "\n";
    } else {
        std::cerr << "Failed to communicate with motor.\n";
    }

  controller.SetStop();
  double increment = 0.05;
  while (true) {
    command.position += increment;
    const auto maybe_result = controller.SetPosition(command);
    if (maybe_result) {
      const auto& v = maybe_result->values;
      std::cout << "Mode: " << static_cast<int>(v.mode)
                << " Fault: " << v.fault
                << " Position: " << v.position
                << " Velocity: " << v.velocity << "\n";
    }
    ::usleep(10000);
    if ((command.position >= 5.0 && increment > 0) ||
        (command.position <= -5.0 && increment < 0)){

      std::cout << "Flip" << increment << std::endl;
      increment *= -1;
      std::cout << "Flip" << increment << std::endl;
    }
  }
  return 0;
}