cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(relay C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()
pico_simple_hardware_target(pio)
# add_subdirectory(examples)

# include_directories(examples)
include(example_auto_set_url.cmake)
add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

add_executable(relay
    relay.c
)
pico_enable_stdio_usb(relay 1)
pico_enable_stdio_uart(relay 1)
pico_add_extra_outputs(relay)
target_link_libraries(relay pico_stdlib hardware_pio)
