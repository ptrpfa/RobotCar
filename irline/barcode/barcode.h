/* Macros */
// Barcode 39
#define TOTAL_CHAR 44                         // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                         // Length of each barcode's binary representation
#define DELIMIT_CHAR '*'                      // Delimiter used as a start/stop character before actual data reading
#define DELIMIT_CODE "010010100"              // Binary representation of delimit character
#define DELIMIT_REVERSED_CODE "001010010"     // Reversed binary representation of delimit character
#define ERROR_CHAR '#'                        // Error character

// Sensors
#define BTN_PIN 20                            // Maker kit button pin
#define IR_SENSOR_PIN 26                      // IR sensor pin
#define DEBOUNCE_DELAY_MICROSECONDS 5000      // Debounce delay in microseconds (us)

/* Global Variables */
bool reverse_scan = false;                    // Boolean to check whether current scan direction is reversed or not
bool start_scan = false;                      // Boolean to store current scan status, used to ignore initial change in state
uint64_t last_state_change_time = 0;          // Variable to store the last time where the state changed (microseconds), used for measuring the time it takes to scan each bar
uint64_t scanned_timings[CODE_LENGTH] = {0};  // Array to store the time it took to scan each bar
uint16_t count_scanned_bar = 0;               // Count of number of bars scanned
uint16_t count_scanned_char = 0;              // Count of number of characters scanned, used to get target character between delimiters
char scanned_code[CODE_LENGTH + 1] = "";      // String to store scanned barcode binary representation
char barcode_char = ERROR_CHAR;               // Character variable to store scanned and parsed barcode character

/* Function Prototypes */
void setup_barcode_pin();   // Function to setup barcode pin to digital
void reset_barcode();       // Function to reset barcode
char parse_scanned_bars(); // Function to parse scanned bars
void read_barcode();        // Function to read from ADC
void interrupt_callback();  // Interrupt callback function