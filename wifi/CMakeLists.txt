set(PROGRAM_NAME wifi)

execute_process(COMMAND
    py makefsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

add_executable(${PROGRAM_NAME} wifi.c cgi.c ssi.c)

target_include_directories(${PROGRAM_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../motor
    ${CMAKE_CURRENT_LIST_DIR}/../ultrasonic
    ${CMAKE_CURRENT_LIST_DIR}/../nav
    ${CMAKE_CURRENT_LIST_DIR}/../irline/barcode
    )

target_link_libraries(${PROGRAM_NAME}
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_stdlib
    hardware_adc
    motor_lib
    ultrasonic_lib
    barcode_lib
    )

pico_enable_stdio_usb(${PROGRAM_NAME} TRUE)
pico_add_extra_outputs(${PROGRAM_NAME})

