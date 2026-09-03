cmake_minimum_required(VERSION 3.10)
project(test_lib)
add_library(test_lib STATIC test_lib.cpp)

# Expose the 'test_lib' folder so files including "test_lib.h" can find it
target_include_directories(test_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})