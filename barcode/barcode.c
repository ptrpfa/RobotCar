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

// Function to lookup barcode character
char* get_barcode_char() {
    /*   
       NOTE: Each character in Barcode 39 is encoded using 5 black bars, 4 white bars, and 3 wide bars. To represent each of the 
       44 unique characters, a binary representation is used, whereby 1 indicates a wide bar, and 0 indicates a narrow bar.
       The binary representation does not capture any information on the colour of the bar (whether it is black or white).
    */

    // Initialise lookup character
    char* lookup_char = NULL;

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
        // Loop through all possible binary and reverse binary representations for a matching lookup character
        for(int i = 0; i < TOTAL_CHAR; i++) {
            if((strcmp(scanned_code, array_code[i]) == 0) || (strcmp(scanned_code, array_reverse_code[i]) == 0)) {
                // Update lookup character and immediately break out of loop
                lookup_char = array_char[i];
                break;
            }
        }
    }

    // Return lookup character obtained
    return lookup_char;
}

// Function to read from ADC
bool read_barcode(struct repeating_timer *t) {
    /* Read from IR sensor */
    // Keep a copy of the last sampling average
    last_sample_avg = current_sample_avg;

    // Capture ADC sample readings
    current_sample_avg = get_adc_sample_average();

    // Get current time (from boot) and calculate hours, minutes, seconds, and milliseconds
    uint64_t current_time = time_us_64();
    uint64_t milliseconds = current_time / 1000 % 1000;
    uint64_t seconds = (current_time / 1000000) % 60;
    uint64_t minutes = (current_time / 60000000) % 60;
    uint64_t hours = (current_time / 3600000000) % 24;

    // Check current color scanned
    current_color = (current_sample_avg >= BLACK_VOLTAGE) ? 1 : 0; // 0: White, 1: Black

    /* Check for change in color/state */
    // Calculate difference in voltage between current and last sample
    float voltage_difference = (fabs(current_sample_avg - last_sample_avg) / current_sample_avg) * 100;
    bool color_change = voltage_difference >= MIN_VOLTAGE_DIFF;

    // Calculate difference between last state change time and current time
    uint64_t time_diff = current_time - last_state_change_time;
    bool valid_time_diff = time_diff >= MIN_TIME_DIFF;

    // Compute boolean check
    bool change_state = (current_color != last_scanned_color) && color_change && valid_time_diff;

    printf("\n\n%02lld:%02lld:%02lld:%03lld => Color: %d", hours, minutes, seconds, milliseconds, last_scanned_color);
    printf("\nCurrent sampling average: %f V\n", current_sample_avg); 
    printf("\nLast sampling average: %f V\n", last_sample_avg); 
    printf("\nTIME DIFFERENCE: %02lld microseconds", time_diff);
    printf("\nVOLTAGE DIFFERENCE: %f %%", voltage_difference);

    
    if(change_state) {
        printf("\n~~~~~~COLOR CHANGE~~~~~~~~~\n");
        // Update last state change time
        last_state_change_time = current_time;
        last_scanned_color = current_color;
    }


    // Check for state changes
    // if(check_state_change()) {
    //     // Check if nothing has been scanned yet
    //     if(current_color == -1) {
    //         // Set current color to black
    //         current_color = 1;
    //         printf("Current color is black!");
    //         printf("\nCurrent sampling average: %f V\n", current_sample_avg); 
    //     } 
    //     else if(current_color == 0) {
    //         // Set current color to black
    //         current_color = 1;
    //         printf("Current color is black!");
    //         printf("\nCurrent sampling average: %f V\n", current_sample_avg); 
    //     }
    //     else {
    //         // Set current color to white
    //         current_color = 0;
    //         printf("Current color is white!");
    //         printf("\nCurrent sampling average: %f V\n", current_sample_avg); 
    //     }
    //     // scanned_code = "000101010";
    //     // white_bar[0] = 1;
    //     // white_bar[1] = 3;
    //     // black_bar[0] = 5;
    //     // black_bar[1] = 0;
    //     // printf("%s: %s\n", "001010010", get_barcode_char());
    // }
    // else {
    //     printf("No state change!");

    // }

    
    

    return true;

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
    add_repeating_timer_ms(IR_SENSOR_PERIODIC_INTERVAL, read_barcode, NULL, &barcode_timer);

    // Loop forever
    while(true) {
        // Perform no operations indefinitely
        tight_loop_contents();
    };
    
}