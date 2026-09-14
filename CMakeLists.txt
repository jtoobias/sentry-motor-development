cmake_minimum_required(VERSION 3.14)
project(moteus_test)

set(CMAKE_CXX_STANDARD 20)

# 1. Add the moteus folder so CMake processes its CMakeLists.txt
# add_subdirectory(third_party/moteus)

# 2. Define your executable or test script target
# add_executable(my_moteus_test main.cpp)
add_executable(gpio_toggle gpio_toggle.cpp)

# 3. Link the moteus C++ interface target to your executable
# target_link_libraries(my_moteus_test PRIVATE moteus::cpp)

# add_subdirectory(test_lib)

# target_link_libraries(my_moteus_test PRIVATE test_lib)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)
pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)

target_include_directories(gpio_toggle PRIVATE ${GPIODCXX_INCLUDE_DIRS})
target_link_libraries(gpio_toggle PRIVATE ${GPIODCXX_LIBRARIES} ${GPIOD_LIBRARIES})