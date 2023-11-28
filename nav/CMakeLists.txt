add_executable(navigation navigation.c)

target_include_directories(navigation PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../motor
    )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(navigation 
        pico_stdlib 
        hardware_pwm
        motor_lib
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(navigation 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(navigation)

add_library(navigation_lib
        navigation.c
        navigation.h
        )

# pull in common dependencies
target_link_libraries(navigation_lib
    pico_stdlib 
    hardware_pwm
    motor_lib
    )

# create map/bin/hex file etc.
pico_enable_stdio_usb(navigation_lib 1)
