cmake_minimum_required(VERSION 3.13)

set(APPNAME raceofficersfriend)
set(PROGNAME "Race Officers Friend")
set(VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_SDK_PATH "$ENV{HOME}/pico/pico-sdk")
include("$ENV{HOME}/pico/pico-sdk/external/pico_sdk_import.cmake")
set(PICO-LIB_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}/../Pico-Lib" )

project(${APPNAME} C CXX ASM)

pico_sdk_init()

#
#     build the application image
#

add_executable(app)

pico_set_program_name(app "${PROGNAME}")
pico_set_program_version(app "${VERSION}")
target_compile_definitions(app PUBLIC VERSION="${VERSION}" )

pico_enable_stdio_uart(app 1)
pico_enable_stdio_usb(app 0)

pico_generate_pio_header(app ${PICO-LIB_CURRENT_LIST_DIR}/src/events/debounce.pio)

include_directories(
    ${PICO-LIB_CURRENT_LIST_DIR}/src/events/
)

target_sources(app PUBLIC
    src/main.c
    src/buttonactions.c
    src/timeractions.c
    src/statemachine.c

    src/screen.c
    src/graphiczoneUI.c
    src/statuszoneUI.c
    src/timezoneUI.c
    src/miscUI.c
    src/zonelayout.c
    src/racingscreenUI.c

    ${PICO-LIB_CURRENT_LIST_DIR}/src/events/gpio_events.c
    ${PICO-LIB_CURRENT_LIST_DIR}/src/events/timer_events.c
    ${PICO-LIB_CURRENT_LIST_DIR}/src/events/eventhandler.c
)

target_link_libraries(app pico_stdlib hardware_pio hardware_gpio )

pico_add_extra_outputs(app)
