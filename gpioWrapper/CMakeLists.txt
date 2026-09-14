cmake_minimum_required(VERSION 3.14)
project(gpio_wrapper)
# 1. Force C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Automatically download the {fmt} library
include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com
  GIT_TAG        11.0.2 # Uses a stable version
)
FetchContent_MakeAvailable(fmt)

target_include_directories(gpio_wrapper PRIVATE ${GPIODCXX_INCLUDE_DIRS})
target_link_libraries(gpio_wrapper PRIVATE ${GPIODCXX_LIBRARIES} ${GPIOD_LIBRARIES})

add_library(gpio_wrapper STATIC gpio_wrapper.cpp gpio_pin.cpp)
