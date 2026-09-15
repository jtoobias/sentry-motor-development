cmake_minimum_required(VERSION 3.14)
project(gpio_wrapper)

# 1. Force C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Automatically download the {fmt} library (Fixed the URL)
include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG        11.0.2
)
FetchContent_MakeAvailable(fmt)


# 3. Create the library target FIRST
add_library(gpio_wrapper STATIC gpio_wrapper.cpp gpio_pin.cpp)

# 4. Now attach settings to the target (Using PUBLIC so the main app inherits them)
target_include_directories(gpio_wrapper PUBLIC ${GPIODCXX_INCLUDE_DIRS})
target_link_libraries(gpio_wrapper PUBLIC ${GPIODCXX_LIBRARIES} ${GPIOD_LIBRARIES} fmt::fmt)
