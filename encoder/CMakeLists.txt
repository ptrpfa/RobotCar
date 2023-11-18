add_executable(encoder encoder.c)

target_include_directories(encoder PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../motor
    )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(encoder 
        pico_stdlib 
        hardware_pwm
        motor_lib
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(encoder 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(encoder)

add_library(encoder_lib
        encoder.c
        encoder.h
        )

# pull in common dependencies
target_link_libraries(encoder_lib
    pico_stdlib 
    hardware_pwm
    motor_lib
    )

# create map/bin/hex file etc.
pico_enable_stdio_usb(encoder_lib 1)
