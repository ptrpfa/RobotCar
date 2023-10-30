#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Macros
#define LEFT_IR_PIN 9   // Digital
#define RIGHT_IR_PIN 22 // Digital

// Function that is invoked upon a change in right IR sensor's input
void wall_detected() {
    // Check if left IR pin's state is high
    if(gpio_get(LEFT_IR_PIN)) {
        printf("Left IR Sensor: Detected Wall!\n");
    } 
    // Check if right IR pin's state is high
    if(gpio_get(RIGHT_IR_PIN)) {
        printf("Right IR Sensor: Detected Wall!\n");
    } 
}

// Program entrypoint
int main() {
    // Initialise standard I/O
    stdio_init_all();

    // Initialise specified pins as input with a pull-up resistor 
    gpio_set_dir(LEFT_IR_PIN, GPIO_IN);       
    gpio_set_dir(RIGHT_IR_PIN, GPIO_IN);       
    gpio_pull_up(LEFT_IR_PIN);
    gpio_pull_up(RIGHT_IR_PIN);

    // Enable interrupt on rising or falling edge
    gpio_set_irq_enabled_with_callback(LEFT_IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &wall_detected);
    
    // Loop forever
    while(true) {
        tight_loop_contents();
    };
}