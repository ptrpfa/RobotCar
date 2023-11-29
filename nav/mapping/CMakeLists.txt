add_executable(mapping mapping.c)

target_include_directories(mapping PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../motor
    ${CMAKE_CURRENT_LIST_DIR}/../../encoder
    ${CMAKE_CURRENT_LIST_DIR}/../../ultrasonic
    ${CMAKE_CURRENT_LIST_DIR}/../../irline/wallsensor
    ${CMAKE_CURRENT_LIST_DIR}/../../irline/barcode 
    ${CMAKE_CURRENT_LIST_DIR}/../navigation
    )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(mapping 
        pico_stdlib 
        hardware_pwm
        hardware_adc
        motor_lib
        encoder_lib
        ultrasonic_lib
        wallsensor_lib
        barcode_lib
        navigation_lib
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(mapping 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(mapping)

add_library(mapping_lib
        mapping.c
        mapping.h
        )

# pull in common dependencies
target_link_libraries(mapping_lib
    pico_stdlib 
    hardware_pwm
    hardware_adc
    motor_lib
    encoder_lib
    ultrasonic_lib
    wallsensor_lib
    barcode_lib
    navigation_lib
    )

# create map/bin/hex file etc.
pico_enable_stdio_usb(mapping_lib 1)
