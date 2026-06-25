find_library(CMOCKA_LIBRARY cmocka)

if(NOT CMOCKA_LIBRARY)
    message(FATAL_ERROR "cmocka library not found")
endif()

add_executable(capture_helpers_test
    capture_helpers_test.c
    ../main/capture_helpers.c
)

target_include_directories(capture_helpers_test PRIVATE
    ../main
)

target_link_libraries(capture_helpers_test PRIVATE ${CMOCKA_LIBRARY})

add_test(NAME capture_helpers_test COMMAND capture_helpers_test)
