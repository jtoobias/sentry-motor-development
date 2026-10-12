cmake_minimum_required(VERSION 3.14)
project(moteus_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. FIND PACKAGES FIRST (So the variables actually exist)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)
pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)

include(FetchContent)

FetchContent_Declare(
    gpio_wrapper
    # Replace this with the absolute or relative path to your moved library
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../gpio_wrapper
    DOWNLOAD_COMMAND ""
)

FetchContent_MakeAvailable(gpio_wrapper)

FetchContent_Declare(
  moteus
  GIT_REPOSITORY https://github.com/mjbots/moteus.git
  # Pin to a specific commit for a reproducible build.  The trailing
  # comment names the corresponding C++ release tag for readers.
  GIT_TAG        e4459e78aa80a90d40ccfc57584e1cd94b6903e2  # cpp/v1.0.0
)
FetchContent_MakeAvailable(moteus)



# 2. Define your executable main program
add_executable(main main.cpp)
add_executable(spin src/spin.cpp)
target_link_libraries(spin moteus::cpp)

# 4. Link everything to your executable target (gpio_toggle)
target_include_directories(main PRIVATE 
    ${GPIODCXX_INCLUDE_DIRS}
    # ✅ Fixes '#include "gpio_wrapper/..."' by adding the parent folder to the search path
    "${CMAKE_CURRENT_SOURCE_DIR}/.." 
)
target_link_libraries(main PRIVATE 
    gpio_wrapper           # Links your custom GPIO code
    ${GPIODCXX_LIBRARIES}  # Links libgpiod C++
    ${GPIOD_LIBRARIES}     # Links libgpiod C base
)
