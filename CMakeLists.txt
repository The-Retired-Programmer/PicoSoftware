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

pico_enable_stdio_uart(app 0)
pico_enable_stdio_usb(app 1)

target_sources(app PUBLIC
    src/main.c
    src/logic_probe.c
    src/probe_controls.c
    src/digitalsampling.c
    src/run_length_encoder.c
    src/pio_digitalsampling.c
    src/dma_digitalsampling.c
    src/pio_program.c
    src/gpio_probe_event.c
    src/frontend_commands.c
    src/square_wave_generator.c
)

target_link_libraries(app pico_stdlib hardware_pio hardware_dma hardware_gpio )

pico_add_extra_outputs(app)

#
#     build the testing image
#
add_executable(tests)

pico_set_program_name(tests "logic_analyser tests")
pico_set_program_version(tests "0.4")

pico_enable_stdio_uart(tests 1)
pico_enable_stdio_usb(tests 1)

target_compile_definitions(tests PUBLIC TESTINGBUILD)

target_sources(tests PUBLIC
    test/main.c
    test/ptest.c
    test/test_probe_controls.c src/probe_controls.c
    test/test_run_length_encoder.c src/run_length_encoder.c
    test/test_pio_digitalsampling.c src/pio_digitalsampling.c 
    test/test_dma_digitalsampling.c src/dma_digitalsampling.c
    test/test_pio_program.c src/pio_program.c
    test/test_square_wave_generator.c src/square_wave_generator.c
    test/test_gpio_probe_event.c src/gpio_probe_event.c
    test/integration_test.c
        src/logic_probe.c
        src/frontend_commands.c
        src/digitalsampling.c
)

target_link_libraries(tests pico_stdlib hardware_pio hardware_dma hardware_gpio )

pico_add_extra_outputs(tests)
