// Header file for barcode configurations
#include <string.h>

// Macros
#define BARCODE_PIN 26                              // GPIO pin used for scanning barcode (ADC)
#define BARCODE_PERIODIC_INTERVAL -10               // Time interval to check barcode (in milliseconds)                  
#define DELIMIT_CHAR '*'                            // Delimiter used as a start/stop character before actual data reading
#define TOTAL_CHAR 44                               // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                               // Length of each barcode's binary representation
#define SAMPLING_SIZE 1000                          // Number of samples to read from each IR sensor capture
#define STATE_DIFFERENCE 0.3                        // Percentage difference between ADC voltage readings to differentiate between states
#define WIDTH_THRESHOLD 3.0                         // Minimum size difference to differentiate between a narrow and wide bar

// Global variables
const float conversion_factor = 3.3f / (1 << 12);   // ADC's resolution, 3.3 VREF for 12-bit ADC
uint16_t sample_buffer[SAMPLING_SIZE] = {0};        // Array to store buffer of ADC sampling readings (unconverted)
float current_sample_avg = 0.0;                     // Variable to store current average of ADC sample readings (converted)
float last_sample_avg = 0.0;                        // Variable to store average of last ADC sample readings (converted)
uint16_t current_color = -1;                        // Variable to store current colour scanned (-1: NA, 0: White, 1: Black)
uint16_t white_bar[2] = {0};                        // Array to store the number of narrow and wide white bars [narrow, wide]
uint16_t black_bar[2] = {0};                        // Array to store the number of narrow and wide black bars [narrow, wide]
char* scanned_code = "";                            // String to store scanned barcode

// Function Prototypes
void setup_barcode_pin();                           // Function to setup barcode pin
bool read_barcode(struct repeating_timer *t);       // Function to read IR sensor values
char* get_barcode_char();                           // Function to lookup scanned barcode to get its matching character