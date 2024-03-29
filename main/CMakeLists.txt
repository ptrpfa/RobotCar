add_executable(main
        main.c
        ${CMAKE_CURRENT_LIST_DIR}/../wifi/cgi.c 
        ${CMAKE_CURRENT_LIST_DIR}/../wifi/ssi.c
        )

target_include_directories(main PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${CMAKE_CURRENT_LIST_DIR}/../ultrasonic
        ${CMAKE_CURRENT_LIST_DIR}/../motor
        ${CMAKE_CURRENT_LIST_DIR}/../encoder
        ${CMAKE_CURRENT_LIST_DIR}/../wifi
        ${CMAKE_CURRENT_LIST_DIR}/../irline/wallsensor
        ${CMAKE_CURRENT_LIST_DIR}/../irline/barcode 
        ${CMAKE_CURRENT_LIST_DIR}/../magnetometer
        ${CMAKE_CURRENT_LIST_DIR}/../nav/mapping
        ${CMAKE_CURRENT_LIST_DIR}/../nav/navigation
        )

target_compile_definitions(main PRIVATE
        PICO_MAX_SHARED_IRQ_HANDLERS=5
        )

# pull in common dependencies
target_link_libraries(main 
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_http
        hardware_adc
        ultrasonic_lib
        motor_lib
        encoder_lib
        wallsensor_lib
        barcode_lib
        magnetometer_lib
        mapping_lib
        navigation_lib
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(main)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)