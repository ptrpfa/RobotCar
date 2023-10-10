add_executable(mqtt
        mqtt.c
        )

target_compile_definitions(mqtt PRIVATE
        WIFI_SSID="Main Living Room"
        WIFI_PASSWORD="m72eetxn"
)

target_include_directories(mqtt PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..
)

# pull in common dependencies
target_link_libraries(mqtt 
        pico_cyw43_arch_lwip_poll
        pico_stdlib
        pico_lwip_mbedtls
        pico_mbedtls
        pico_lwip_mqtt
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(mqtt 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(mqtt)

