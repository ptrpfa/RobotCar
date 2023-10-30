# Define target
set(target wall_detection_driver)

# Add executable target for source file
add_executable(${target} detect_wall.c)

# Link libraries (common dependencies)
target_link_libraries(${target} pico_stdlib)

# Enable USB output
pico_enable_stdio_usb(${target} 1)

# Add extra build outputs (map/bin/hex file etc) to build target
pico_add_extra_outputs(${target})

# Add URL to target via pico_set_program_url
example_auto_set_url(${target})