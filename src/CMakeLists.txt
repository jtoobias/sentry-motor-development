# 1. Bundle your controller logic into a reusable local library target
add_library(motor_core STATIC
  moteus_controller.cpp
  homing.cpp
)

# 2. Apply include paths specifically to your core library logic
target_include_directories(motor_core PUBLIC 
  ${GPIODCXX_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}/../include" # Exposes include headers
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
  ${moteus_SOURCE_DIR}/lib/cpp/mjbots/moteus 
)

# 3. Link dependencies to your core library
target_link_libraries(motor_core PUBLIC
  moteus::cpp
  gpio_wrapper
  ${GPIODCXX_LIBRARIES}
  ${GPIOD_LIBRARIES}
)

# ========================================================
# 4. Define your executable targets using your local library
# ========================================================

# Main application executable
add_executable(main_app ${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp) # Path uses ../ because main.cpp is in root
target_include_directories(main_app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_link_libraries(main_app PRIVATE motor_core)

# Spin utility executable
add_executable(spin spin.cpp)
target_link_libraries(spin PRIVATE motor_core)
#Homing example
add_executable(home_example homing_example.cpp)
target_include_directories(home_example PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_link_libraries(home_example PRIVATE motor_core)
