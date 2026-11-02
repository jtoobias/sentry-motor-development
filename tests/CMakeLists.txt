set(TEST_SOURCES
    test_homing.cpp
)

add_executable(homing_tests ${TEST_SOURCES})

target_include_directories(homing_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR} 
)

target_link_libraries(homing_tests PRIVATE 
    motor_core        
    gtest      
    gtest_main 
)

add_test(NAME HomingTests COMMAND homing_tests)