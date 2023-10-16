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
