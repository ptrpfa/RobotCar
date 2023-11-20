#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Macros
#define IR_PIN 26 // Analogue

// Global Variables
const float conversion_factor = 3.3f / (1 << 12);

// Function to read from ADC
void read_analogue() {
    uint32_t result = adc_read();
    printf("\nAnalogue: 0x%03x -> %f V\n", result, result * conversion_factor); 
}

// Program entrypoint
int main() {

    // Initialise standard I/O
    stdio_init_all();
    
    /* Analogue */
    // Initialise ADC on Raspberry Pi Pico
    adc_init();

    // Initialise GPIO pin for ADC operations, disabling all digital functions for that pin
    adc_gpio_init(IR_PIN);

    // Set ADC channel (Channel 0 for pin 26)
    adc_select_input(0);

    // Loop forever
    while(true) {
        read_analogue();
    };
    
}