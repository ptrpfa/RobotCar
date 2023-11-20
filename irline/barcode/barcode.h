/* Macros */
// Barcode 39
#define TOTAL_CHAR 44                         // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                         // Length of each barcode's binary representation
#define DELIMIT_CHAR "#"                      // Delimiter used as a start/stop character before actual data reading
#define ERROR_CHAR "#"                        // Error character

// Sensors
#define BTN_PIN 20                            // Maker kit button pin
#define IR_SENSOR_PIN 26                      // IR sensor pin
#define DEBOUNCE_DELAY_MICROSECONDS 1000      // Debounce delay in microseconds (us)

/* Global Variables */
bool reverse_scan = false;                    // Boolean to check whether current scan direction is reversed or not
bool start_scan = false;                      // Boolean to store current scan status, used to ignore initial change in state
uint64_t last_state_change_time = 0;          // Variable to store the last time where the state changed (microseconds), used for measuring the time it takes to scan each bar
uint64_t scanned_timings[CODE_LENGTH] = {0};  // Array to store the time it took to scan each bar
uint16_t count_scanned_bar = 0;               // Count of number of bars scanned
uint16_t count_scanned_char = 0;              // Count of number of characters scanned, used to get target character between delimiters
char scanned_code[CODE_LENGTH + 1] = "";      // String to store scanned barcode binary representation
char* barcode_char = ""                       // String to store scanned and parsed barcode character

/*   
    NOTE: Each character in Barcode 39 is encoded using 5 black bars, 4 white bars, and 3 wide bars. To represent each of the 
    44 unique characters, a binary representation is used, whereby 1 indicates a wide bar, and 0 indicates a narrow bar.
    The binary representation does not capture any information on the colour of the bar (whether it is black or white).
*/
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

/* Function Prototypes */
void setup_barcode_pin();   // Function to setup barcode pin to digital
void reset_barcode();       // Function to reset barcode
char* parse_scanned_bars(); // Function to parse scanned bars
void read_barcode();        // Function to read from ADC
void interrupt_callback();  // Interrupt callback function