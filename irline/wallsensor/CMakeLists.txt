# Define target
set(target wallsensor)

# Add executable target for source file
add_executable(${target} wallsensor.c)

# Link libraries (common dependencies)
target_link_libraries(${target} pico_stdlib)

# Enable USB output
pico_enable_stdio_usb(${target} 1)

# Add extra build outputs (map/bin/hex file etc) to build target
pico_add_extra_outputs(${target})

add_library(wallsensor_lib
        wallsensor.c
        wallsensor.h
        )

# pull in common dependencies
target_link_libraries(wallsensor_lib
    pico_stdlib 
    )

# create map/bin/hex file etc.
pico_enable_stdio_usb(wallsensor 1)
