add_executable(ultrasonic
        ultrasonic.c
        )

# pull in common dependencies
target_link_libraries(ultrasonic
    pico_stdlib 
    hardware_gpio 
    hardware_timer)

# create map/bin/hex file etc.
pico_add_extra_outputs(ultrasonic)

pico_enable_stdio_usb(ultrasonic 1)
pico_enable_stdio_uart(ultrasonic 0)

add_library(ultrasonic_lib
        ultrasonic.c
        ultrasonic.h
        )

# pull in common dependencies
target_link_libraries(ultrasonic_lib
    pico_stdlib 
    hardware_gpio 
    hardware_timer)

# create map/bin/hex file etc.
pico_enable_stdio_usb(ultrasonic_lib 1)
