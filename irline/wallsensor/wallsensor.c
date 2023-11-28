#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "wallsensor.h"

// Function to initialise ir line sensors
void init_wallsensors()
{
    // Initialize specified pins as input with a pull-up resistor
    gpio_init(LEFT_IR_PIN);
    gpio_init(RIGHT_IR_PIN);
    gpio_set_dir(LEFT_IR_PIN, GPIO_IN);
    gpio_set_dir(RIGHT_IR_PIN, GPIO_IN);
    gpio_pull_up(LEFT_IR_PIN);
    gpio_pull_up(RIGHT_IR_PIN);
}

bool isWallDetected()
{
    if (gpio_get(LEFT_IR_PIN) && gpio_get(RIGHT_IR_PIN))
    {
        printf("WALL DETECTED\n");
    }
    else
    {
        printf("NO WALL\n");
    }
    return gpio_get(LEFT_IR_PIN) && gpio_get(RIGHT_IR_PIN);
}

/*
// Program entrypoint
int main() {
    // Initialise standard I/O
    stdio_init_all();

    sleep_ms(3000);

    init_wallsensors();
    printf("Wall sensor GPIOs initialised\n");

    // Loop forever
    while(true) {
        if (gpio_get(LEFT_IR_PIN))
        {
            printf("LEFT WALL DETECTED\n");
        }
        if (gpio_get(RIGHT_IR_PIN))
        {
            printf("RIGHT WALL DETECTED\n");
        }
    }
}
*/