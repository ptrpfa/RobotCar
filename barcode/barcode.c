#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Macros
#define R_ENCODER_PIN 8
#define L_ENCODER_PIN 9

// Function that is invoked upon a change in IR sensor's input
void callback() {
    // Check if pin's state is high
    if(gpio_get(L_ENCODER_PIN)) {
        printf("High\n");
    } 
    // Pin's state is low
    else {
        printf("Low\n");
    }

}

// Program entrypoint
int main() {

    // Initialise standard I/O
    stdio_init_all();

    // Initialise specified pin as input with a pull-up resistor 
    gpio_set_dir(L_ENCODER_PIN, GPIO_IN);       
    gpio_pull_up(L_ENCODER_PIN);

    // Enable interrupt on rising or falling edge
    gpio_set_irq_enabled_with_callback(L_ENCODER_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &callback);

    // Loop forever
    while(true) {
        // Perform no operations indefinitely
        tight_loop_contents();
    };
    
}
