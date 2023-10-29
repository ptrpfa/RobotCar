// Header file for IR sensor configurations

/* Macros */
// IR Sensor
#define IR_SENSOR_PIN 26                                            // GPIO pin used for IR sensor (ADC)
#define IR_SENSOR_PERIODIC_INTERVAL -100                            // Time interval to check IR sensor (in milliseconds)     
#define SAMPLING_SIZE 100                                           // Number of samples to read from each IR sensor capture
#define BLACK_VOLTAGE 1.3                                           // Voltage to detect a black bar
// Thresholds
#define SLOWEST_SPEED 10                                            // Slowest speed for car to move (cm/s)
#define THINNEST_BAR 0.5                                            // Width of thinnest barcode bar (cm)
#define MIN_TIME_DIFF (THINNEST_BAR / SLOWEST_SPEED) * 1000000      // Minimum amount of time (microseconds) that must be elapsed for a change in state
#define MIN_VOLTAGE_DIFF 10                                         // Minimum percentage difference of ADC voltage readings for a change in state
// Barcode 39
#define TOTAL_CHAR 44                                               // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                                               // Length of each barcode's binary representation
#define WIDTH_THRESHOLD 3.0                                         // Minimum size difference to differentiate between a narrow and wide bar
#define DELIMIT_CHAR '*'                                            // Delimiter used as a start/stop character before actual data reading

// Global variables
const float conversion_factor = 3.3f / (1 << 12);                   // ADC's resolution, 3.3 VREF for 12-bit ADC
float current_sample_avg = 0.0;                                     // Variable to store current average of ADC sample readings (voltage)
float last_sample_avg = 0.0;                                        // Variable to store average of last ADC sample readings (voltage)
uint64_t last_state_change_time = 0;                                // Variable to store the last time where the state changed (microseconds)
uint64_t current_color = 0;                                         // Variable to store the current colour detected (0: White, 1: Black)
uint16_t last_scanned_color = -1;                                   // Variable to store current colour scanned (-1: NA, 0: White, 1: Black)
uint16_t white_bar[2] = {0};                                        // Array to store the number of narrow and wide white bars [narrow, wide]
uint16_t black_bar[2] = {0};                                        // Array to store the number of narrow and wide black bars [narrow, wide]
char* scanned_code = "";                                            // String to store scanned barcode binary representation

/* Function Prototypes */
void setup_barcode_pin();                                           // Function to setup barcode pin
float __not_in_flash_func(get_adc_sample_average)();                // Function to read samples from the ADC
char* get_barcode_char();                                           // Function to lookup scanned barcode to get its matching character
bool read_barcode(struct repeating_timer *t);                       // Function to read IR sensor values