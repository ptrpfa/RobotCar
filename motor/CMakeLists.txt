add_executable(motor motor.c)

# pull in common dependencies and additional pwm hardware support
target_link_libraries(motor 
        pico_stdlib 
        hardware_pwm)

# enable usb output, disable uart output
pico_enable_stdio_usb(motor 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(motor)
