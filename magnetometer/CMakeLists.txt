add_executable(magnetometer 
    magnetometer.c
    )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(magnetometer 
        pico_stdlib 
        hardware_i2c)

# enable usb output, disable uart output
pico_enable_stdio_usb(magnetometer 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(magnetometer)

# create map/bin/hex file etc.
pico_enable_stdio_usb(magnetometer 1)
