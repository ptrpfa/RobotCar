#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Macros
#define RIGHT_IR_PIN 27 // Digital
#define LEFT_IR_PIN 26  // Digital

// Global variable to keep track of wall detection
volatile bool wallDetected = false;

// Function to initialise ir line sensors
void init_wallsensors() {
    // Initialize standard I/O
    // stdio_init_all();

    // Initialize specified pins as input with a pull-up resistor
    gpio_init(LEFT_IR_PIN);
    gpio_init(RIGHT_IR_PIN);
    gpio_set_dir(LEFT_IR_PIN, GPIO_IN);
    gpio_set_dir(RIGHT_IR_PIN, GPIO_IN);
    gpio_pull_up(LEFT_IR_PIN);
    gpio_pull_up(RIGHT_IR_PIN);

    // Enable interrupt on rising or falling edge for left IR sensor
    // gpio_set_irq_enabled_with_callback(LEFT_IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &wall_detected);
    // gpio_set_irq_enabled_with_callback(RIGHT_IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &wall_detected);
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
        gpio_get(LEFT_IR_PIN);
        gpio_get(RIGHT_IR_PIN);
    };
}
*/