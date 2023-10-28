// Header file for barcode configurations

// Imports
#include <string.h>

// Configuration options
#define DELIMIT_CHAR '*'                            // Delimiter used as a start/stop character before actual data reading
#define WIDTH_THRESHOLD 3.0                         // Minimum size difference to differentiate between a narrow and wide bar
#define TOTAL_CHAR 44                               // Total number of characters encoded by Barcode 39 representation
#define CODE_LENGTH 9                               // Length of each barcode's binary representation

// Global variables
const float conversion_factor = 3.3f / (1 << 12);   // ADC's resolution, 3.3 VREF for 12-bit ADC
int white_bar[2] = {0};                             // Array to store the number of narrow and wide white bars [narrow, wide]
int black_bar[2] = {0};                             // Array to store the number of narrow and wide black bars [narrow, wide]
char* scanned_code = "";                            // String to store scanned barcode

// Function Prototypes
char* get_barcode_char();

// Function Definitions
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
                                    "Y", "Z", "_", ".", "$", "/", "+", "%", " ", "*"};       

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
    if( (strlen(scanned_code) == CODE_LENGTH) && (black_bar[0] + black_bar[1] == 5) && (white_bar[0] + white_bar[1] == 4) && (white_bar[1] + black_bar[1] == 3)) {
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