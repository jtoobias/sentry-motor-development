#include <iostream>
#include <limits>
#include <unistd.h>
#include <chrono>
#include "moteus_controller.hpp"

namespace moteus = mjbots::moteus;

int main(int argc, char** argv) {

  auto m1 = std::make_unique<MoteusController>(1);

  m1->set_absolute_position(0.0);
  auto result = m1->enable();

  double increment = 0.01;
  double position = 0.0;
  while (true) {
    position += increment;
    m1->position_move_relative(position,1.0,0.5);

    auto status = m1->get_current_status();
    std::cout   << " Fault: " << static_cast<int>(status.status_code)
                << " ,Position: ," << status.position
                << " ,Velocity: ," << status.speed << "\n";
    ::usleep(10000);
    if ((status.position >= 1.0 && increment > 0) ||
        (status.position <= -1.0 && increment < 0)){

      std::cout << "Flip" << increment << std::endl;
      increment *= -1;
      std::cout << "Flip" << increment << std::endl;
    }
    // test keepalive
  }
  return 0;
}