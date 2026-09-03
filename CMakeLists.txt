cmake_minimum_required(VERSION 3.10)
project(rdk5_moteus_test)

set(CMAKE_CXX_STANDARD 17)

# 1. Add the moteus folder so CMake processes its CMakeLists.txt
add_subdirectory(third_party/moteus)

# 2. Define your executable or test script target
add_executable(my_moteus_test main.cpp)

# 3. Link the moteus C++ interface target to your executable
target_link_libraries(my_moteus_test PRIVATE moteus::cpp)

add_subdirectory(test_lib)

target_link_libraries(my_moteus_test PRIVATE test_lib)