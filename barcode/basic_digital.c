#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Macros
#define IR_PIN 22 // Digital

// Function that is invoked upon a change in IR sensor's input
void read_digital() {
    // Check if pin's state is high
    if(gpio_get(IR_PIN)) {
        printf("Current pin status: High\n");
    } 
    // Pin's state is low
    else {
        printf("Current pin status: Low\n");
    }
}

// Program entrypoint
int main() {

    // Initialise standard I/O
    stdio_init_all();

    /* Digital */
    // Initialise specified pin as input with a pull-up resistor 
    gpio_set_dir(IR_PIN, GPIO_IN);       
    gpio_pull_up(IR_PIN);

    // Enable interrupt on rising or falling edge
    gpio_set_irq_enabled_with_callback(IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &read_digital);
    
    // Loop forever
    while(true) {
        tight_loop_contents();
    };
    
}