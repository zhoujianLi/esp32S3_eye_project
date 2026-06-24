idf_component_register(SRCS "main.c" "capture_helpers.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp32_s3_eye)

target_compile_definitions(${COMPONENT_LIB} PRIVATE FW_VERSION="${PROJECT_VER}")
