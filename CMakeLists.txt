cmake_minimum_required(VERSION 3.14)
project(moteus_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. FIND SYSTEM PACKAGES GLOBALLY
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)
pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)

# 2. FETCH EXTERNAL DEPENDENCIES
include(FetchContent)

FetchContent_Declare(
    gpio_wrapper
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../gpio_wrapper
    DOWNLOAD_COMMAND ""
)
FetchContent_MakeAvailable(gpio_wrapper)

FetchContent_Declare(
  moteus
  GIT_REPOSITORY https://github.com/mjbots/moteus.git
  GIT_TAG        e4459e78aa80a90d40ccfc57584e1cd94b6903e2  # cpp/v1.0.0
)
FetchContent_MakeAvailable(moteus)
set(moteus_SOURCE_DIR "${moteus_SOURCE_DIR}" CACHE INTERNAL "Expose moteus path to subdirectories")


FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

# For Windows/MSVC runner compatibility, ensure runtime library matches
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) 
FetchContent_MakeAvailable(googletest)

# 3. SET UP TESTING CONFIGURATION
option(BUILD_TESTING "Build the test suite" ON)
if(BUILD_TESTING)
    enable_testing()
    # REMOVED: find_package(GTest REQUIRED) is gone.
    # Targets 'gtest' and 'gtest_main' are now natively available!
    add_subdirectory(tests) # Directs control to your tests/ folder
endif()

# 4. DELEGATE PRODUCTION COMPILATION TO SRC FOLDER
add_subdirectory(src)
