cmake_minimum_required(VERSION 3.16)

project(esp32_s3_eye_tests C)

option(BUILD_TESTS "Build host unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(examples/lcd_display/tests)
endif()
