#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "barcode.h"

/* Function Definitions */
// Function to setup barcode pin
void setup_barcode_pin() {
    // Initialise ADC on Raspberry Pi Pico
    adc_init();
    // Initialise GPIO pin for ADC operations, disabling all digital functions for that pin
    adc_gpio_init(IR_SENSOR_PIN);
    // Set ADC channel
    switch(IR_SENSOR_PIN) {
        case 26:
            adc_select_input(0);
            break;
        case 27:
            adc_select_input(1);
            break;
        case 28:
            adc_select_input(2);
            break;
        case 29:
            adc_select_input(3);
            break;
        default:
            break;
    }
}

// Function to read samples from the ADC
float __not_in_flash_func(get_adc_sample_average)() {
    // Initialise variable to store average of ADC sampling readings
    float sample_average = 0.0;

    // Capture ADC sample readings
    adc_fifo_setup(true, false, 0, false, false);
    adc_run(true);
    for (int i = 0; i < SAMPLING_SIZE; i++) {
        sample_average += adc_fifo_get_blocking();
    }
    adc_run(false);
    adc_fifo_drain();

    // Calculate sample average voltage
    sample_average = (sample_average / SAMPLING_SIZE) * conversion_factor;

    // Return sample average
    return sample_average;
}

// Function to reverse a string
void reverse_string(char* str) {
    // if the string is empty
    if (!str) {
        return;
    }
    // pointer to start and end at the string
    int i = 0;
    int j = strlen(str) - 1;
 
    // reversing string
    while (i < j) {
        char c = str[i];
        str[i] = str[j];
        str[j] = c;
        i++;
        j--;
    }
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


    // Ensure that the length of the barcode(9), number of black bars(5), number of white bars(4), and number of wide bars(3) is correct
    if((strlen(scanned_code) == CODE_LENGTH) && (black_bar[0] + black_bar[1] == 5) && (white_bar[0] + white_bar[1] == 4) && (white_bar[1] + black_bar[1] == 3)) {
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
        // Check for a match in the previous check
        if(!match) {
            // Reverse string
            // reverse_string(scanned_code);
            // Loop through all possible reverse binary representations for a matching lookup character
            for(int i = 0; i < TOTAL_CHAR; i++) {
                if(strcmp(scanned_code, array_reverse_code[i]) == 0) {
                    // Update lookup character and immediately break out of loop
                    lookup_char = array_char[i];
                    break;
                }
            }
            // Reverse string back to normal again
            // reverse_string(scanned_code);
        }
    }

    // Return lookup character obtained
    return lookup_char;
}

// Function to read from ADC
bool read_barcode(struct repeating_timer *t) {
    // Get current speed of the car in cm/s (to link with motor driver)
    current_speed = 2.5;

    // Calculate minimum amount of time that must be elapsed to scan the thinnest narrow bar and a wide bar
    min_time_narrow_bar = (THINNEST_BAR / current_speed) * 1000000;
    min_time_wide_bar = min_time_narrow_bar * WIDTH_THRESHOLD;     

    /* Read from IR sensor */
    // Keep a copy of the last sampling average
    last_sample_avg = current_sample_avg;

    // Capture ADC sample readings
    current_sample_avg = get_adc_sample_average();
    // current_sample_avg = adc_read() * conversion_factor;
    // printf("\ncurrent reading: %f", current_sample_avg);

    // Get current color scanned (0: White, 1: Black)
    current_color = (current_sample_avg >= MIN_BLACK_VOLTAGE) ? 1 : 0;

    // Calculate difference in voltage between current and last sample
    float voltage_difference = (fabs(current_sample_avg - last_sample_avg) / current_sample_avg) * 100;

    /* TO BE REMOVED? */
    // Get current time (from boot)
    uint64_t current_time = time_us_64();

    // Calculate difference between last state change time and current time
    uint64_t time_diff = current_time - last_state_change_time;

    // Check for a valid change in state (not first read, color has changed, voltage difference is valid and time difference is valid)
    if((current_color != last_scanned_color) && (voltage_difference >= MIN_VOLTAGE_DIFF) && (time_diff >= min_time_narrow_bar)) {
        /* Check current scan status */
        // IDLE
        if(start_scan == false) {
            printf("\nSCAN STATUS: IDLE");
            // Change scan status to start scanning in the next round
            start_scan = true;
        }
        // START SCAN or SCANNING
        else {
            // Get the type of the previous bar scanned (0: Narrow, 1: Wide)
            last_scanned_type = (time_diff >= min_time_wide_bar) ? 1 : 0;

            // Add binary representation of barcode and update white or black bar array
            // Check if the string is empty and update it accordingly
            if (strcmp(scanned_code, "\0") == 0) {
                strcpy(scanned_code, "");  // Clear the initial string
            }
            (last_scanned_type == 0) ? strcat(scanned_code, "0") : strcat(scanned_code, "1");
            (last_scanned_color == 0) ? white_bar[last_scanned_type]++ : black_bar[last_scanned_type]++;

            // Get current time in terms of hours, minutes, seconds, and milliseconds
            uint64_t milliseconds = current_time / 1000 % 1000;
            uint64_t seconds = (current_time / 1000000) % 60;
            uint64_t minutes = (current_time / 60000000) % 60;
            uint64_t hours = (current_time / 3600000000) % 24;

            // Print for debugging
            printf("\n\nSCAN STATUS: SCANNING");
            printf("\n~~~~~~PREVIOUS BAR SCANNED~~~~~~~~~\n");
            printf("\n%02lld:%02lld:%02lld:%03lld => Color: %s, Bar: %s", hours, minutes, seconds, milliseconds, (last_scanned_color == 0) ? "White" : "Black", (last_scanned_type == 0) ? "Thin" : "Thick");
            printf("\nTIME DIFFERENCE: %02lld microseconds", time_diff);
            printf("\nSCANNED CODE: %s", scanned_code);
            
            // Check for barcode
            if(strlen(scanned_code) == CODE_LENGTH) {

                // Sanity check and change to top 3 bars if wrong
                // something here

                printf("\n\nCode Received!\n%s: %s\n", scanned_code, get_barcode_char());

                // Reset
                strcpy(scanned_code, "\0");
                white_bar[0] = 0;
                white_bar[1] = 0;
                black_bar[0] = 0;
                black_bar[1] = 0;
                last_scanned_color = 2;
                last_scanned_type = 2; 
                // Reset scan status to IDLE state
                start_scan = false;
            }
        }
        // Update last state change time and last scanned color
        last_state_change_time = time_us_64();
        last_scanned_color = current_color;
    }
    else {
        // Update last scanned color
        last_scanned_color = current_color;
    }
    
    // Return to caller
    return true;
}

// Function that is invoked upon a button press
void button_callback()
{
    // Reset
    strcpy(scanned_code, "\0");
    white_bar[0] = 0;
    white_bar[1] = 0;
    black_bar[0] = 0;
    black_bar[1] = 0;
    last_scanned_color = 2;
    last_scanned_type = 2; 
    // Reset scan status to IDLE state
    start_scan = false;

    printf("\nRESET BARCODE!\n\n");
}

// Program entrypoint
int main() {
    // Initialise standard I/O
    stdio_init_all();
    
    // Setup barcode pin
    setup_barcode_pin();

    // Initialise repeating_timer struct for barcode
    struct repeating_timer barcode_timer; 

    // Start periodic timer to periodically read for barcodes
    add_repeating_timer_ms(-IR_SENSOR_PERIODIC_INTERVAL, read_barcode, NULL, &barcode_timer);

    /* TEMPORARY (For Maker Kit button reset)*/
    // Configure GPIO pin as input, with a pull-up resistor (Active-Low)
    gpio_init(20);
    gpio_set_dir(20, GPIO_IN);
    gpio_set_pulls(20, true, false);

    // Enable interrupt on specified pin upon a button press (rising or falling edge)
    gpio_set_irq_enabled_with_callback(20, GPIO_IRQ_EDGE_FALL, true, &button_callback);


    // Loop forever
    while(true) {
        // Perform no operations indefinitely
        tight_loop_contents();
    };
}