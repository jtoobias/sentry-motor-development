cmake_minimum_required(VERSION 3.10)
project(gpio_wrapper)
set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)
pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)

target_include_directories(gpio_wrapper PRIVATE ${GPIODCXX_INCLUDE_DIRS})
target_link_libraries(gpio_wrapper PRIVATE ${GPIODCXX_LIBRARIES} ${GPIOD_LIBRARIES})

add_library(gpio_wrapper STATIC gpio_wrapper.cpp gpio_pin.cpp)
