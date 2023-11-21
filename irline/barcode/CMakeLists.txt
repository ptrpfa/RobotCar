# Define target
set(target barcode_driver)

# Add executable target for source file
add_executable(${target} barcode.c)

# Link libraries (common dependencies)
target_link_libraries(${target} pico_stdlib)

# Enable USB output
pico_enable_stdio_usb(${target} 1)

# Add extra build outputs (map/bin/hex file etc) to build target
pico_add_extra_outputs(${target})

add_library(barcode_lib
        barcode.c
        barcode.h
        )
    
# pull in common dependencies
target_link_libraries(barcode_lib
    pico_stdlib 
    )