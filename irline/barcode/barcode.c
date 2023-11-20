#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "barcode.h"

/* Function Definitions */
// Function to setup barcode pin to digital
void setup_barcode_pin() {
    // Configure GPIO pin as input, with a pull-up resistor (Active-Low)
    gpio_init(IR_SENSOR_PIN);
    gpio_set_dir(IR_SENSOR_PIN, GPIO_IN);
    gpio_set_pulls(IR_SENSOR_PIN, true, false);
}

// Function to reset barcode
void reset_barcode() {
    // Reset number of bars scanned
    count_scanned_bar = 0;

    // Reset scanned code
    strcpy(scanned_code, ""); 

    // Reset array of scanned timings
    for(uint16_t i = 0; i < CODE_LENGTH; i++) {
        scanned_timings[i] = 0;
    }

    // Reset scan status
    start_scan = false;
}

// Function to parse scanned bars
char* parse_scanned_bars() {
    // Initialise array of indexes
    uint16_t indexes[CODE_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // Bubble sort the indexes array based on the values in scanned_timings
    for (uint16_t i = 0; i < CODE_LENGTH - 1; ++i) {
        for (uint16_t j = 0; j < CODE_LENGTH - i - 1; ++j) {
            if (scanned_timings[indexes[j]] < scanned_timings[indexes[j + 1]]) {
                // Swap indexes if the value at j is less than the value at j + 1
                uint16_t temp = indexes[j];
                indexes[j] = indexes[j + 1];
                indexes[j + 1] = temp;
            }
        }
    }

    // Generate the final binary representation string (initialise all characters to 0, narrow bars)
    for (uint16_t i = 0; i < CODE_LENGTH; ++i) {
        scanned_code[i] = '0';
    }
    // Null-terminate the string
    scanned_code[CODE_LENGTH] = '\0';

    // Set the top 3 indexes (top 3 timings) to 1, wide bars
    for (uint16_t i = 0; i < 3; ++i) {
        scanned_code[indexes[i]] = '1';
    }

    // Initialise the decoded character
    char* decoded_char = ERROR_CHAR;

    // Initialise variable to check for matches
    bool match = false;

    // Check scan direction
    if(!reverse_scan) {
        // Loop through all possible binary representations for a matching lookup character
        for(int i = 0; i < TOTAL_CHAR; i++) {
            if(strcmp(scanned_code, array_code[i]) == 0) {
                // Update lookup character and immediately break out of loop
                decoded_char = array_char[i];
                match = true;
                break;
            }
        }
    }
    // If there are no matches, try reverse lookup
    if(!match || reverse_scan) {
        // Loop through all possible reverse binary representations for a matching lookup character
        for(int i = 0; i < TOTAL_CHAR; i++) {
            if(strcmp(scanned_code, array_reverse_code[i]) == 0) {
                // Update lookup character and immediately break out of loop
                decoded_char = array_char[i];
                // Update scan direction
                reverse_scan = true;
                break;
            }
        }
    }

    // Return decoded character to caller
    return decoded_char;
}

// Function to read from ADC
void read_barcode() {
    /*  
        Color: voltage or use edge rise/fall
        Width: select top 3 timings for thick barcode, can use moving average of timings then compare which one take longest
        --> need to ensure that car speed is CONSTANT 
    */

    // Perform some basic denoising or checks to detect when a barcode is encountered
    // something here
    // Ensure that car is moving at a constant speed, to slow down the car
    // current_speed = 2.5;

    // Check whether to start scan
    if(!start_scan) { // Ignore initial change of state
        // Change scan status to start scanning in the next round
        start_scan = true;
    }
    // Start scanning
    else {
        // Store time difference between state change in array
        scanned_timings[count_scanned_bar] = time_us_64() - last_state_change_time;
        
        // Update number of bars scanned
        ++count_scanned_bar;

        // Print for debugging
        printf("\n\nTime difference [%d]: %lld", count_scanned_bar, scanned_timings[count_scanned_bar - 1]);

        // Start decoding when number of bars scanned reaches required code length
        if(count_scanned_bar == CODE_LENGTH) {
            // Parse scanned bars
            char* scanned_char = parse_scanned_bars();
            
            // Reset barcode after reading a character
            reset_barcode();

            // Update number of characters scanned
            ++count_scanned_char;

            // Check validity of scanned character
            bool valid_char = strcmp(scanned_char, ERROR_CHAR) ? true : false;

            // Check if scanned character is valid
            if(valid_char) {
                // Check number of characters scanned
                switch(count_scanned_char){
                    // Check for a delimiter character
                    case 1:
                        // Check if the scanned character matches the delimiter character
                        if(strcmp(scanned_char, DELIMIT_CHAR)) { 
                            printf("No starting delimiter character found! Backup car and reset all characters scanned so far..\n");
                            // Reset number of characters scanned 
                            count_scanned_char = 0;
                            // TODO: Backup car..
                        }
                        break;
                    // Check for a valid character
                    case 2:
                        // Check if the scanned character matches the delimiter character
                        if(strcmp(scanned_char, DELIMIT_CHAR)) { 
                            printf("Delimiter character found instead of a valid character! Backup car and reset all characters scanned so far..\n");
                            // Reset number of characters scanned 
                            count_scanned_char = 0;
                            // TODO: Backup car..
                        }
                        else {
                            // Update barcode character scanned
                            strcpy(barcode_char, scanned_char);
                            // Print for debugging
                            printf("\n\nBarcode Character (\n%s): %s\n", scanned_code, barcode_char);
                            // TODO: Transmit scanned code..
                        }
                        break;
                    case 3:
                        // Check if the scanned character matches the delimiter character
                        if(strcmp(scanned_char, DELIMIT_CHAR)) { 
                            printf("No ending delimiter character found! Backup car and reset all characters scanned so far..\n");
                            // Reset number of characters scanned 
                            count_scanned_char = 0;
                            // TODO: Backup car..
                        }
                        else {
                            // Reset scan direction
                            reverse_scan = false;
                            // Reset barcode character scanned
                            strcpy(barcode_char, ""); 
                        }
                        break;
                    default:
                        break;
                }
            }
            else { 
                // Invalid character scanned
                printf("Invalid barcode character scanned! Backup car and reset all characters scanned so far..");
                // Reset number of characters scanned 
                count_scanned_char = 0;
                // TODO: Backup car..
            }
        }

    }
    
    // Update last state change time after all computations are completed
    last_state_change_time = time_us_64();
}

// Interrupt callback function
void interrupt_callback() {
    // Check if button has been pressed
    if(!gpio_get(BTN_PIN)) {
        printf("\nRESET BARCODE!\n\n");
        // Reset barcode
        reset_barcode();
    }
    else {
        // Ensure that the time difference between current time and last button press is not within the debounce delay threshold
        if((time_us_64() - last_state_change_time) > DEBOUNCE_DELAY_MICROSECONDS) {
            // Read barcode
            read_barcode();
        }
    }
}

// Program entrypoint
int main() {
    // Initialise standard I/O
    stdio_init_all();

    // Setup barcode pin
    setup_barcode_pin();

    // Enable interrupt on specified pin upon a button press (rising or falling edge)
    gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_callback);
    
    /* TEMPORARY (For Maker Kit button reset)*/
    // Configure GPIO pin as input, with a pull-up resistor (Active-Low)
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_set_pulls(BTN_PIN, true, false);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &interrupt_callback);

    // Loop forever
    while(true) {
        // Perform no operations indefinitely
        tight_loop_contents();
    };
}