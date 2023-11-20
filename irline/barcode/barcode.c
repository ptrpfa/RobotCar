#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
// #include "barcode.h"

// Barcode 39
#define TOTAL_CHAR 44                                               // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                                               // Length of each barcode's binary representation
#define DELIMIT_CHAR '*'                                            // Delimiter used as a start/stop character before actual data reading
#define DEBOUNCE_DELAY_MICROSECONDS 1000      // Debounce delay in microseconds (us)

// Sensors
#define BTN_PIN 20
#define IR_SENSOR_PIN 26                                            // GPIO pin used for IR sensor (Digital)

// Global variables
bool start_scan = false;                                             // Boolean to store current scan status (False: Idle, True: Scanning)
// something to check end of scan (3 chars scanned)
uint64_t last_state_change_time = 0;                                // Variable to store the last time where the state changed (microseconds), used for measuring the time it takes to scan each bar
uint64_t scanned_timings[CODE_LENGTH] = {0};                        // Array to store the time it took to scan each bar
uint16_t scanned_bars = 0;                                          // Count of number of bars scanned
char scanned_code[CODE_LENGTH + 1] = "";                          // String to store scanned barcode binary representation

/*  
    Color: voltage or use edge rise/fall
    Width: select top 3 timings for thick barcode, can use moving average of timings then compare which one take longest
    --> need to ensure that car speed is CONSTANT 
*/


/* Function Definitions */
// Function to setup barcode pin to digital
void setup_barcode_pin() {
    // Configure GPIO pin as input, with a pull-up resistor (Active-Low)
    gpio_init(IR_SENSOR_PIN);
    gpio_set_dir(IR_SENSOR_PIN, GPIO_IN);
    gpio_set_pulls(IR_SENSOR_PIN, true, false);
}


// Function to lookup barcode character
char* get_barcode_char() {
    /*   
       NOTE: Each character in Barcode 39 is encoded using 5 black bars, 4 white bars, and 3 wide bars. To represent each of the 
       44 unique characters, a binary representation is used, whereby 1 indicates a wide bar, and 0 indicates a narrow bar.
       The binary representation does not capture any information on the colour of the bar (whether it is black or white).
    */

    // Initialise lookup character
    char* lookup_char = "ERROR";

    // Initialise array used to store each barcode character
    char* array_char[TOTAL_CHAR] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", 
                                    "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", 
                                    "Y", "Z", "_", ".", "$", "/", "+", "%%", " ", "*"};       

    // Initialise array used to store binary representation of each character
    char* array_code[TOTAL_CHAR] = {"000110100", "100100001", "001100001", "101100000", "000110001", "100110000", "001110000", 
                                    "000100101", "100100100", "001100100", "100001001", "001001001", "101001000", "000011001", 
                                    "100011000", "001011000", "000001101", "100001100", "001001100", "000011100", "100000011", 
                                    "001000011", "101000010", "000010011", "100010010", "001010010", "000000111", "100000110", 
                                    "001000110", "000010110", "110000001", "011000001", "111000000", "010010001", "110010000", 
                                    "011010000", "010000101", "110000100", "010101000", "010100010", "010001010", "000101010", 
                                    "011000100", "010010100"};
                                    
    // Initialise array used to store the reversed binary representation of each character
    char* array_reverse_code[TOTAL_CHAR] = {"001011000", "100001001", "100001100", "000001101", "100011000", "000011001", 
                                            "000011100", "101001000", "001001001", "001001100", "100100001", "100100100", 
                                            "000100101", "100110000", "000110001", "000110100", "101100000", "001100001", 
                                            "001100100", "001110000", "110000001", "110000100", "010000101", "110010000", 
                                            "010010001", "010010100", "111000000", "011000001", "011000100", "011010000", 
                                            "100000011", "100000110", "000000111", "100010010", "000010011", "000010110", 
                                            "101000010", "001000011", "000101010", "010001010", "010100010", "010101000", 
                                            "001000110", "001010010"};

    // Initialise variable to check for matches
    bool match = false;

    // Loop through all possible binary representations for a matching lookup character
    for(int i = 0; i < TOTAL_CHAR; i++) {
        if(strcmp(scanned_code, array_code[i]) == 0) {
            // Update lookup character and immediately break out of loop
            lookup_char = array_char[i];
            match = true;
            break;
        }
    }
    // If there are no matches, try reverse lookup
    if(!match) {
        // Loop through all possible reverse binary representations for a matching lookup character
        for(int i = 0; i < TOTAL_CHAR; i++) {
            if(strcmp(scanned_code, array_reverse_code[i]) == 0) {
                // Update lookup character and immediately break out of loop
                lookup_char = array_char[i];
                break;
            }
        }
    }

    // Return lookup character obtained
    return lookup_char;
}

// Function to reset barcode
void reset() {
    // Reset number of bars scanned
    scanned_bars = 0;

    // Reset scanned code
    // strcpy(scanned_code, ""); 

    // Reset array of scanned timings
    for(uint16_t i = 0; i < CODE_LENGTH; i++) {
        scanned_timings[i] = 0;
    }

    // TO CHANGE!!!!!
    // Reset scan status to IDLE state (temporary, to link with number of characters scanned)
    start_scan = false;
}

// Function to parse bars scanned
void parse_bars() {

    // Create an array of indices and initialize it with values from 0 to CODE_LENGTH-1
    int indices[CODE_LENGTH];
    for (int i = 0; i < CODE_LENGTH; ++i) {
        indices[i] = i;
    }

    // Bubble sort the indices array based on the values in scanned_timings
    for (int i = 0; i < CODE_LENGTH - 1; ++i) {
        for (int j = 0; j < CODE_LENGTH - i - 1; ++j) {
            if (scanned_timings[indices[j]] < scanned_timings[indices[j + 1]]) {
                // Swap indices if the value at j is less than the value at j + 1
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }

    // Generate the final string
    for (int i = 0; i < CODE_LENGTH; ++i) {
        scanned_code[i] = '0';
    }
    // Null-terminate the string
    scanned_code[CODE_LENGTH] = '\0';

    // Set the top 3 indices to '1'
    for (int i = 0; i < 3; ++i) {
        scanned_code[indices[i]] = '1';
    }

    printf("\n\nCode Received!\n%s: %s\n", scanned_code, get_barcode_char());

}


// Function to read from ADC
void read_barcode() {
    // Perform some basic denoising or checks to detect when a barcode is encountered
    // something here
    // Ensure that car is moving at a constant speed, to slow down the car
    // current_speed = 2.5;

    // IDLE
    if(start_scan == false) {
        printf("\nSCAN STATUS: IDLE");
        // Change scan status to start scanning in the next round
        start_scan = true;
    }
    // START SCAN or SCANNING
    else {
        // Store time difference in array
        scanned_timings[scanned_bars] = time_us_64() - last_state_change_time;
        
        // Increment number of bars scanned
        ++scanned_bars;

        // Print for debugging
        printf("\n\nTime difference [%d]: %lld", scanned_bars, scanned_timings[scanned_bars - 1]);
        

        // Decode when number of bars scanned reaches code length
        if(scanned_bars == CODE_LENGTH) {
            // Parse bars scanned
            parse_bars();

            // Reset
            reset();
        }

    }
    
    // Update last state change time after all computations are completed
    last_state_change_time = time_us_64();
}

// Interrupt callback function
void interrupt_callback()
{
    // Check if button has been pressed
    if(!gpio_get(BTN_PIN)) {
        printf("\nRESET BARCODE!\n\n");
        // Reset
        reset();
    }
    else {
        // Ensure that the time difference between current time and last button press is not within the debounce delay threshold
        if((time_us_64() - last_state_change_time) > DEBOUNCE_DELAY_MICROSECONDS) {
            // Read barcode
            read_barcode();
        }

        // // Update scanned colour
        // scanned_color = gpio_get(IR_SENSOR_PIN);

        // // Check type of change (high to low or low to high)
        // if(scanned_color) { // Black bar detected
        //     printf("IR HIGH\n");
        // }
        // else {  // White bar
        //     printf("IR Low state\n"); 
        // }
    }
    

}

// Program entrypoint
int main() {
    // Initialise standard I/O
    stdio_init_all();

    // Setup barcode pin
    setup_barcode_pin();

    /* TEMPORARY (For Maker Kit button reset)*/
    // Configure GPIO pin as input, with a pull-up resistor (Active-Low)
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_set_pulls(BTN_PIN, true, false);

    // Enable interrupt on specified pin upon a button press (rising or falling edge)
    gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_callback);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &interrupt_callback);

    // Loop forever
    while(true) {
        // Perform no operations indefinitely
        tight_loop_contents();
    };
}