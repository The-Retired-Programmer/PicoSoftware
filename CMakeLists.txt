cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")
include(/home/pi/pico/pico-sdk/external/pico_sdk_import.cmake)

project(raceofficersfriend C CXX ASM)

pico_sdk_init()

#
#     build the application image
#

add_executable(app)

pico_set_program_name(app "race_officers_friend")
pico_set_program_version(app "0.1")
target_compile_definitions(app PUBLIC VERSION="0.1" )

pico_enable_stdio_uart(app 1)
pico_enable_stdio_usb(app 0)

target_sources(app PUBLIC
    src/main.c
    src/screen.c
    src/controller.c
    src/graphiczoneUI.c
    src/statuszoneUI.c
    src/timezoneUI.c
    src/miscUI.c
)

target_link_libraries(app pico_stdlib hardware_pio hardware_dma hardware_gpio )

pico_add_extra_outputs(app)

#
#     build the testing image
#
add_executable(tests)

pico_set_program_name(tests "race_officers_friend tests")
pico_set_program_version(tests "0.1-tests")
target_compile_definitions(tests PUBLIC VERSION="0.1-tests")

pico_enable_stdio_uart(tests 1)
pico_enable_stdio_usb(tests 0)

target_compile_definitions(tests PUBLIC TESTINGBUILD)

target_sources(tests PUBLIC
    test/main.c
    test/ptest.c
)

target_link_libraries(tests pico_stdlib hardware_pio hardware_dma hardware_gpio )

pico_add_extra_outputs(tests)
