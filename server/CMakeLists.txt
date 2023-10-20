cmake_minimum_required(VERSION 3.13)
set(PICO_SDK_PATH "/Users/jr/pico-sdk")

set(PROGRAM_NAME Server)
set(PICO_BOARD pico_w)


pico_sdk_init()

message("Running makefsdata python script")
execute_process(COMMAND
    py makefsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(${PROGRAM_NAME} main.c)

target_include_directories(${PROGRAM_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROGRAM_NAME}
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_stdlib
    hardware_adc
)

pico_enable_stdio_usb(${PROGRAM_NAME} TRUE)
pico_add_extra_outputs(${PROGRAM_NAME})
