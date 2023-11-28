#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "barcode.h"

// Global variable to keep track of barcode scan
volatile bool barcode_scan = false;

/* Global Variables */
extern volatile bool barcode_scan;
bool reverse_scan = false;                   // Boolean to check whether current scan direction is reversed or not
bool start_scan = false;                     // Boolean to store current scan status, used to ignore initial change in state
uint64_t last_state_change_time = 0;         // Variable to store the last time where the state changed (microseconds), used for measuring the time it takes to scan each bar
uint64_t scanned_timings[CODE_LENGTH] = {0}; // Array to store the time it took to scan each bar
uint16_t count_scanned_bar = 0;              // Count of number of bars scanned
uint16_t count_scanned_char = 0;             // Count of number of characters scanned, used to get target character between delimiters
char scanned_code[CODE_LENGTH + 1] = "";     // String to store scanned barcode binary representation
// char barcode_char = ERROR_CHAR;
char barcode_char = 'k';

/* Function Definitions */
// Function to setup barcode pin to digital
void setup_barcode_pin()
{
    // Configure GPIO pin as input, with a pull-up resistor (Active-Low)
    gpio_init(IR_SENSOR_PIN);
    gpio_set_dir(IR_SENSOR_PIN, GPIO_IN);
    gpio_set_pulls(IR_SENSOR_PIN, true, false);
}

// Function to reset barcode
void reset_barcode()
{
    // Reset number of bars scanned
    count_scanned_bar = 0;

    // Reset scanned code
    strcpy(scanned_code, "");

    // Reset array of scanned timings
    for (uint16_t i = 0; i < CODE_LENGTH; i++)
    {
        scanned_timings[i] = 0;
    }

    // Reset scan status
    start_scan = false;
}

// Function to parse scanned bars
char parse_scanned_bars()
{
    // Initialise array of indexes
    uint16_t indexes[CODE_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // Bubble sort the indexes array based on the values in scanned_timings
    for (uint16_t i = 0; i < CODE_LENGTH - 1; ++i)
    {
        for (uint16_t j = 0; j < CODE_LENGTH - i - 1; ++j)
        {
            if (scanned_timings[indexes[j]] < scanned_timings[indexes[j + 1]])
            {
                // Swap indexes if the value at j is less than the value at j + 1
                uint16_t temp = indexes[j];
                indexes[j] = indexes[j + 1];
                indexes[j + 1] = temp;
            }
        }
    }

    // Generate the final binary representation string (initialise all characters to 0, narrow bars)
    for (uint16_t i = 0; i < CODE_LENGTH; ++i)
    {
        scanned_code[i] = '0';
    }
    // Null-terminate the string
    scanned_code[CODE_LENGTH] = '\0';

    // Set the top 3 indexes (top 3 timings) to 1, wide bars
    for (uint16_t i = 0; i < 3; ++i)
    {
        scanned_code[indexes[i]] = '1';
    }

    // Initialise the decoded character
    char decoded_char = ERROR_CHAR;

    // Initialise variable to check for matches
    bool match = false;

    /*
        NOTE: Each character in Barcode 39 is encoded using 5 black bars, 4 white bars, and 3 wide bars. To represent each of the
        44 unique characters, a binary representation is used, whereby 1 indicates a wide bar, and 0 indicates a narrow bar.
        The binary representation does not capture any information on the colour of the bar (whether it is black or white).
    */
    // Initialise array used to store each barcode character
    char array_char[TOTAL_CHAR] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
                                   'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                   'Y', 'Z', '_', '.', '$', '/', '+', '%', ' '};

    // Initialise array used to store binary representation of each character
    char *array_code[TOTAL_CHAR] = {"000110100", "100100001", "001100001", "101100000", "000110001", "100110000", "001110000",
                                    "000100101", "100100100", "001100100", "100001001", "001001001", "101001000", "000011001",
                                    "100011000", "001011000", "000001101", "100001100", "001001100", "000011100", "100000011",
                                    "001000011", "101000010", "000010011", "100010010", "001010010", "000000111", "100000110",
                                    "001000110", "000010110", "110000001", "011000001", "111000000", "010010001", "110010000",
                                    "011010000", "010000101", "110000100", "010101000", "010100010", "010001010", "000101010",
                                    "011000100"};

    // Initialise array used to store the reversed binary representation of each character
    char *array_reverse_code[TOTAL_CHAR] = {"001011000", "100001001", "100001100", "000001101", "100011000", "000011001",
                                            "000011100", "101001000", "001001001", "001001100", "100100001", "100100100",
                                            "000100101", "100110000", "000110001", "000110100", "101100000", "001100001",
                                            "001100100", "001110000", "110000001", "110000100", "010000101", "110010000",
                                            "010010001", "010010100", "111000000", "011000001", "011000100", "011010000",
                                            "100000011", "100000110", "000000111", "100010010", "000010011", "000010110",
                                            "101000010", "001000011", "000101010", "010001010", "010100010", "010101000",
                                            "001000110"};

    // Check if parsing for delimit character
    if (count_scanned_char == 1 || count_scanned_char == 3)
    {
        // Check for a matching delimit character
        if (strcmp(scanned_code, DELIMIT_CODE) == 0)
        {
            // Update decoded character
            decoded_char = DELIMIT_CHAR;
            match = true;
        }
        else if (strcmp(scanned_code, DELIMIT_REVERSED_CODE) == 0)
        {
            // Update decoded character
            decoded_char = DELIMIT_CHAR;
            match = true;
            // Update scan direction
            reverse_scan = true;
        }
    }
    else
    { // Parsing for character
        // Check scan direction
        if (!reverse_scan)
        {
            // Loop through all possible binary representations for a matching decoded character
            for (int i = 0; i < TOTAL_CHAR; i++)
            {
                if (strcmp(scanned_code, array_code[i]) == 0)
                {
                    // Update decoded character and immediately break out of loop
                    decoded_char = array_char[i];
                    match = true;
                    break;
                }
            }
        }
        // Reversed scan direction
        else
        {
            // Loop through all possible reverse binary representations for a matching decoded character
            for (int i = 0; i < TOTAL_CHAR; i++)
            {
                if (strcmp(scanned_code, array_reverse_code[i]) == 0)
                {
                    // Update decoded character and immediately break out of loop
                    decoded_char = array_char[i];
                    match = true;
                    break;
                }
            }
        }
    }

    // Return decoded character to caller
    return decoded_char;
}

// Function to read from ADC
void read_barcode()
{
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
    if (!start_scan)
    { // Ignore initial change of state
        // Change scan status to start scanning in the next round
        start_scan = true;
    }
    // Start scanning
    else
    {
        // Store time difference between state change in array
        scanned_timings[count_scanned_bar] = time_us_64() - last_state_change_time;

        // Update number of bars scanned
        ++count_scanned_bar;

        // Print for debugging
        printf("\n\nTime difference [%d]: %lld", count_scanned_bar, scanned_timings[count_scanned_bar - 1]);

        // Start decoding when number of bars scanned reaches required code length
        if (count_scanned_bar == CODE_LENGTH)
        {
            // Update number of characters scanned
            ++count_scanned_char;

            // Parse scanned bars
            char scanned_char = parse_scanned_bars();

            // Check validity of scanned character
            bool valid_char = (scanned_char != ERROR_CHAR) ? true : false;

            // Check if scanned character is valid
            if (valid_char)
            {
                // Check number of characters scanned
                switch (count_scanned_char)
                {
                // Check for a delimiter character
                case 1:
                    // Check if the scanned character matches the delimiter character
                    if (scanned_char != DELIMIT_CHAR)
                    {
                        printf("\nNo starting delimiter character found! Backup car and reset all characters scanned so far..\n");
                        /* Prepare for next scan */
                        // Reset scan direction
                        reverse_scan = false;
                        // Reset barcode character scanned
                        barcode_char = ERROR_CHAR;
                        // Reset number of characters scanned
                        count_scanned_char = 0;
                        // TODO: Backup car..
                        // Disable readings..
                    }
                    break;
                // Check for a valid character
                case 2:
                    // Update barcode character scanned
                    barcode_char = scanned_char;
                    break;
                case 3:
                    // Check if the scanned character matches the delimiter character
                    if (scanned_char != DELIMIT_CHAR)
                    {
                        printf("\nNo ending delimiter character found! Backup car and reset all characters scanned so far..\n");
                        /* Prepare for next scan */
                        // Reset scan direction
                        reverse_scan = false;
                        // Reset barcode character scanned
                        barcode_char = ERROR_CHAR;
                        // Reset number of characters scanned
                        count_scanned_char = 0;
                        // TODO: Backup car..
                        // Disable readings..
                    }
                    else
                    {
                        // Print for debugging
                        printf("\n\nBarcode Character: %c\n", barcode_char);
                        // TODO: Transmit scanned code..

                        /* Prepare for next scan */
                        // Reset scan direction
                        reverse_scan = false;
                        // Reset barcode character scanned
                        // barcode_char = ERROR_CHAR;
                        // Reset number of characters scanned
                        count_scanned_char = 0;
                    }
                    break;
                default:
                    break;
                }
            }
            else
            {
                // Invalid character scanned
                printf("\nInvalid barcode character scanned! Backup car and reset all characters scanned so far..\n");
                /* Prepare for next scan */
                // Reset scan direction
                reverse_scan = false;
                // Reset barcode character scanned
                barcode_char = ERROR_CHAR;
                // Reset number of characters scanned
                count_scanned_char = 0;
                // TODO: Backup car..
                // Disable readings..
            }

            // Reset barcode after reading a character
            reset_barcode();
        }
    }

    // Update last state change time after all computations are completed
    last_state_change_time = time_us_64();
}

// Interrupt callback function
void barcode_callback(uint gpio, uint32_t events)
{
    // Ensure that the time difference between current time and last button press is not within the debounce delay threshold
    if ((time_us_64() - last_state_change_time) > DEBOUNCE_DELAY_MICROSECONDS && gpio == 18)
    {
        barcode_scan = true;
        // Read barcode
        read_barcode();
    }
}

// Interrupt callback function
void interrupt_callback_test()
{
    // Check if button has been pressed
    if (!gpio_get(BTN_PIN))
    {
        printf("\nRESET BARCODE!\n\n");
        // Reset barcode
        reset_barcode();
    }
    else
    {
        // Ensure that the time difference between current time and last button press is not within the debounce delay threshold
        if ((time_us_64() - last_state_change_time) > DEBOUNCE_DELAY_MICROSECONDS)
        {
            barcode_scan = true;
            // Read barcode
            read_barcode();
        }
    }
}

// Function to initialise barcode sensor
void init_barcode()
{
    // Initialise standard I/O
    // stdio_init_all();

    // Setup barcode pin
    setup_barcode_pin();

    // Enable interrupt on specified pin upon a rising or falling edge
    // gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_callback);
}

// Program entrypoint
// int main() {
//     // Initialise standard I/O
//     stdio_init_all();

//     // Setup barcode pin
//     setup_barcode_pin();

//     // Enable interrupt on specified pin upon a button press (rising or falling edge)
//     gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_callback_test);

//     /* TEMPORARY (For Maker Kit button reset)*/
//     // Configure GPIO pin as input, with a pull-up resistor (Active-Low)
//     gpio_init(BTN_PIN);
//     gpio_set_dir(BTN_PIN, GPIO_IN);
//     gpio_set_pulls(BTN_PIN, true, false);
//     gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &interrupt_callback_test);

//     // Loop forever
//     while(true) {
//         // Perform no operations indefinitely
//         tight_loop_contents();
//     };
// }