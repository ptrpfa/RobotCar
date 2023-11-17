// Header file for IR sensor configurations

/* 
    Key Parameters:
    IR_SENSOR_PERIODIC_INTERVAL:    Tweak to modify how fast/slow (interval) for IR sensor to be activated to take readings

    MIN_BLACK_VOLTAGE:              Tweak to set the minimum voltage of a black bar (setting too low would result in more false positives, 
                                    setting too high would result in lesser detections)

    MIN_VOLTAGE_DIFF:               Tweak to modify the minimum percentage difference between the last voltage and current voltage reading
                                    for a change in state/color to be detected
 */

/* Macros */
// Barcode 39
#define TOTAL_CHAR 44                                               // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                                               // Length of each barcode's binary representation
#define DELIMIT_CHAR '*'                                            // Delimiter used as a start/stop character before actual data reading
// IR Sensor
#define IR_SENSOR_PIN 26                                            // GPIO pin used for IR sensor (ADC)
#define SAMPLING_SIZE 100                                           // Number of samples to read from each IR sensor capture
#define IR_SENSOR_PERIODIC_INTERVAL 100                             // Time interval to sample IR sensor (milliseconds)    
#define MIN_BLACK_VOLTAGE 1.3                                       // Minimum voltage to detect a black bar
// Thresholds
#define MIN_VOLTAGE_DIFF 10                                         // Minimum percentage difference of ADC voltage readings for a change in state
#define WIDTH_THRESHOLD 3.0                                         // Minimum threshold difference between the time it takes to scan a wide bar over a narrow bar (ratio between time to scan a wide: time to scan a narrow)
#define THINNEST_BAR 0.5                                            // Width of thinnest barcode bar (cm) [DEPENDENT ON THICKNESS OF BARCODES!!, to change to use MOVING AVERAGE]

// Global variables
const float conversion_factor = 3.3f / (1 << 12);                   // ADC's resolution, 3.3 VREF for 12-bit ADC
bool start_scan = true;                                             // Boolean to store current scan status (False: Idle, True: Scanning)
float current_sample_avg = 0.0;                                     // Variable to store current average of ADC sample readings (voltage)
float last_sample_avg = 0.0;                                        // Variable to store average of last ADC sample readings (voltage)
uint16_t current_color = 0;                                         // Variable to store the current colour scanned (0: White, 1: Black)
uint16_t last_scanned_color = 2;                                    // Variable to store last colour scanned (2: NA, 0: White, 1: Black)
uint16_t last_scanned_type = 2;                                     // Variable to store the type of bar scanned (2: NA, 0: Narrow, 1: Wide)
uint64_t last_state_change_time = 0;                                // Variable to store the last time where the state changed (microseconds)
uint16_t white_bar[2] = {0};                                        // Array to store the number of narrow and wide white bars [narrow, wide]
uint16_t black_bar[2] = {0};                                        // Array to store the number of narrow and wide black bars [narrow, wide]
char scanned_code[CODE_LENGTH + 1] = "\0";                          // String to store scanned barcode binary representation
double current_speed = 0;                                           // Variable to store the current speed for car to move (cm/s)
double min_time_narrow_bar = 0;                                     // Variable to store the minimum amount of time that must be elapsed to scan the thinnest narrow bar
double min_time_wide_bar = 0;                                       // Variable to store the minimum amount of time that must be elapsed to scan a wide bar
uint64_t scanned_timings[CODE_LENGTH] = {0};                        // Array to store the time it took to scan each bar

/* Function Prototypes */
void setup_barcode_pin();                                           // Function to setup barcode pin
float __not_in_flash_func(get_adc_sample_average)();                // Function to read samples from the ADC
void reverse_string(char* str);                                     // Function to reverse a string
char* get_barcode_char();                                           // Function to lookup scanned barcode to get its matching character
bool read_barcode(struct repeating_timer *t);                       // Function to read IR sensor values