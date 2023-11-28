#ifndef barcode_H
#define barcode_H
/* Macros */
// Barcode 39
#define TOTAL_CHAR 44                     // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                     // Length of each barcode's binary representation
#define DELIMIT_CHAR '*'                  // Delimiter used as a start/stop character before actual data reading
#define DELIMIT_CODE "010010100"          // Binary representation of delimit character
#define DELIMIT_REVERSED_CODE "001010010" // Reversed binary representation of delimit character
#define ERROR_CHAR '#'                    // Error character

// Sensors
#define BTN_PIN 20                       // Maker kit button pin
#define IR_SENSOR_PIN 18                 // IR sensor pin
#define DEBOUNCE_DELAY_MICROSECONDS 5000 // Debounce delay in microseconds (us)

/* Function Prototypes */
void setup_barcode_pin();                          // Function to setup barcode pin to digital
void reset_barcode();                              // Function to reset barcode
char parse_scanned_bars();                         // Function to parse scanned bars
void read_barcode();                               // Function to read from ADC
void barcode_callback(uint gpio, uint32_t events); // Interrupt callback function
void init_barcode();

extern char barcode_char;              // Character variable to store scanned and parsed barcode character
extern volatile bool scanning_allowed; // Boolean to indicate when scanning is allowed
extern bool start_scan;                // Boolean to indicate barcode is scanning

#endif