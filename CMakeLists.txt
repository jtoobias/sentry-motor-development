cmake_minimum_required(VERSION 3.14)
project(moteus_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. FIND PACKAGES FIRST (So the variables actually exist)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)
pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)

# 2. Define your executable main program
add_executable(gpio_toggle gpio_toggle.cpp)

# 3. Include the subfolder library
add_subdirectory(gpio_wrapper)

# 4. Link everything to your executable target (gpio_toggle)
target_include_directories(gpio_toggle PRIVATE ${GPIODCXX_INCLUDE_DIRS})
target_link_libraries(gpio_toggle PRIVATE 
    gpio_wrapper           # Links your custom GPIO code
    ${GPIODCXX_LIBRARIES}  # Links libgpiod C++
    ${GPIOD_LIBRARIES}     # Links libgpiod C base
)
