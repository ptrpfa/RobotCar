#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mqtt.h"

int main()
{
    stdio_init_all();
    // Run functions here

    mqtt_setup();
}