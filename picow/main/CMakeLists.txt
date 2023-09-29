add_executable(main
        main.c
        )

# pull in common dependencies
target_link_libraries(main 
        pico_stdlib              # for core functionality
)

# create map/bin/hex file etc.
pico_add_extra_outputs(main)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)