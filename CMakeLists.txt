cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")
include(/home/pi/pico/pico-sdk/external/pico_sdk_import.cmake)

project(logic_analyser C CXX ASM)

pico_sdk_init()

#
#     build the application image
#

add_executable(app)

pico_set_program_name(app "logic_analyser")
pico_set_program_version(app "0.4")

pico_enable_stdio_uart(app 1)
pico_enable_stdio_usb(app 1)

target_sources(app PRIVATE src/main.c src/logic_analyser.c src/logic_probe.c src/probe_controls.c src/digitalsampling.c)

target_link_libraries(app pico_stdlib hardware_pio hardware_dma)

pico_add_extra_outputs(app)

#
#     build the testing image
#

add_executable(tests)

pico_set_program_name(tests "logic_analyser tests")
pico_set_program_version(tests "0.4")

target_sources(tests PRIVATE test/main.c test/ptest.c test/test_probe_controls.c src/probe_controls.c test/test_digitalsampling.c src/digitalsampling.c)

target_link_libraries(tests pico_stdlib hardware_pio hardware_dma)

pico_add_extra_outputs(tests)
