cmake_minimum_required(VERSION 3.13)

set(APPNAME picolib)
set(PROGNAME "Pico Lib")
set(VERSION 0.1)
#
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_SDK_PATH "$ENV{HOME}/pico/pico-sdk")
include($ENV{HOME}/pico/pico-sdk/external/pico_sdk_import.cmake)
project(${APPNAME} C CXX ASM)
pico_sdk_init()

#     build the library test image
add_executable(tests)

pico_set_program_name(tests "${PROGNAME} tests")
pico_set_program_version(tests "${VERSION}-test")
target_compile_definitions(tests PUBLIC VERSION="${VERSION}-tests")

pico_enable_stdio_uart(tests 1)
pico_enable_stdio_usb(tests 0)

pico_generate_pio_header(tests ${CMAKE_CURRENT_LIST_DIR}/src/events/debounce.pio)

target_compile_definitions(tests PUBLIC TESTINGBUILD)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/test/ 
    ${CMAKE_CURRENT_LIST_DIR}/src/ptest/
    ${CMAKE_CURRENT_LIST_DIR}/src/events/
)

target_sources(tests PUBLIC
    test/main.c
    test/integrationtest.c
    test/buttonactions.c
    test/timeractions.c

    src/ptest/ptest.c

    src/events/gpio_events.c
    src/events/timer_events.c
    src/events/eventhandler.c
)

target_link_libraries(tests PUBLIC pico_stdlib hardware_pio)

pico_add_extra_outputs(tests)



