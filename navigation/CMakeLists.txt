add_executable(navigation navigation.c)

# pull in common dependencies
target_link_libraries(navigation pico_stdlib)

pico_enable_stdio_usb(navigation 1)
pico_enable_stdio_uart(navigation 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(navigation)

