cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")
include(/home/pi/pico/pico-sdk/external/pico_sdk_import.cmake)

project(logic_analyser C CXX ASM)
pico_sdk_init()
add_executable(logic_analyser)

pico_set_program_name(logic_analyser "logic_analyser")
pico_set_program_version(logic_analyser "0.2")

pico_enable_stdio_uart(logic_analyser 1)
pico_enable_stdio_usb(logic_analyser 0)

target_sources(logic_analyser PRIVATE logic_analyser.c logic_probe.c pio.c storage.c sourcesignal.c)

target_link_libraries(logic_analyser PRIVATE pico_stdlib hardware_pio hardware_dma)
pico_add_extra_outputs(logic_analyser)

