add_executable(main
        main.c
        )

target_include_directories(main PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${CMAKE_CURRENT_LIST_DIR}/../mqtt
        ${CMAKE_CURRENT_LIST_DIR}/../ultrasonic)

# pull in common dependencies
target_link_libraries(main 
        hardware_adc
        mqtt_lib
        ultrasonic_lib)

# create map/bin/hex file etc.
pico_add_extra_outputs(main)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)