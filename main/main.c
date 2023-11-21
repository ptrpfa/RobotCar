// Main program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "magnetometer.h"
#include "ultrasonic.h"
#include "wallsensor.h"
#include "barcode.h"
#include "encoder.h"
#include "motor.h"
#include "ssi.h"
#include "cgi.h"

// Define maze conditions
#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10

// Struct for maze cell
struct Cell {
    int x;
    int y;
    bool northWall;
    bool southWall;
    bool eastWall;
    bool westWall;
    int wall[4];
    int visited; // 0 - unvisited, 1 - visited, 2 - obstacle, 3 - barcode
};

struct Cell mazeGrid[MAZE_WIDTH][MAZE_HEIGHT];

// Global variables
const char WIFI_SSID[] = "dinie";           // Wifi credentials
const char WIFI_PASSWORD[] = "testest1";    // Wifi credentials
int position = 0;                           // 0 - S, 1 - W, 2 - N, 3 - E
int startCar = 0;                           // From CGI to toggle car start / stop
bool oneGrid = false;

// Function to initialize maze grid as unexplored
void initializeMazeGrid() {
    // Set entire maze to unexplored
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            mazeGrid[x][y].x = x;
            mazeGrid[x][y].y = y;
            mazeGrid[x][y].northWall = false;
            mazeGrid[x][y].southWall = false;
            mazeGrid[x][y].eastWall = false;
            mazeGrid[x][y].westWall = false;
        }
    }
}

// Function to check if x, y is within maze boudaries
int isValid(int x, int y) {
    return x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT;
}

// Function to find the path using a flood-fill algorithm
void firstPathAlgo(struct Cell mazeGrid[MAZE_WIDTH][MAZE_HEIGHT], struct Cell start, struct Cell end) {
    // If destination reached, return
    if (start.x == end.x && start.y == end.y) {
        return;
    }
    sleep_ms(2000);
   	
	// Reposition car to face S
   	if (position != 0) {
		for(int i = position; i != 0; i--) {
			turnMotor(0);
            sleep_ms(525);
            stopMotor();
            sleep_ms(2000);
		}

		position = 0;
   	}

    // Define the possible moves (S, W, N, E)
    int dx[] = {0, -1, 0, 1};
    int dy[] = {1, 0, -1, 0};

    // Try all possible moves
    for (int i = 0; i < 4; i++) {
        int newX = start.x + dx[i];
        int newY = start.y + dy[i];

        // Check if the new position is valid
        if (isValid(newX, newY)) {
            wallDetected = false;
            oneGrid = false;
            bool action = false;      

            time_t startTime, currentTime;
            double elapsedTime = 0;
            startTime = time(NULL);

            // Keep moving until a wall is detected or 1 grid space has past
            while (!action) {
                currentTime = time(NULL);
                elapsedTime = difftime(currentTime, startTime);

                // If past 1 sec without wall detected means successfully moved 1 grid
                if (elapsedTime >= 0.85) {
                    // Stop motor after moving 1 grid
                    stopMotor();

                    // Recursively explore the next cell
                    firstPathAlgo(mazeGrid, (struct Cell){newX, newY}, end);

                    // To break loop once returned
                    action = true;
                } 
                else if (wallDetected) {
                    // Stop motor once wall detected
                    stopMotor();
                    sleep_ms(2000);

                    // Turn to the right to check next cell
                    moveMotor(1900);
                    turnMotor(1);
                    sleep_ms(502);
                    stopMotor();
                    sleep_ms(2000);

                    // Identify the direction tried to move and mark cell wall as blocked
                    if (i == 0) {       // Tried to moved south
                        mazeGrid[start.x][start.y].southWall = true;
                    } 
                    else if (i == 1) {  // Tried to move west
                        mazeGrid[start.x][start.y].westWall = true;
                    } 
                    else if (i == 2) {  // Tried to move north
                        mazeGrid[start.x][start.y].northWall = true;
                    } 
                    else if (i == 3) {  // Tried to move east
                        mazeGrid[start.x][start.y].eastWall = true;
                    }

                    // Set position to know how many turn rights were taken
                    position += 1;

                    // To break loop
                    action = true;
                }
                // Else, not 1 sec yet and no wall detected, continue moving
                else {
                    moveMotor(1900);
                }
            }
        }   
        // New position is not valid or has been explored
        else {
            // Turn to the right
            moveMotor(1900);
            turnMotor(1);
            sleep_ms(502);
            stopMotor();
            position += 1;
        }
    }
}

// Function to init all sensors and motors
void initAll () {
    // Initialise standard I/O
    stdio_init_all();
    sleep_ms(1000);

    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    sleep_ms(1000);

    // Loop until connected to WiFI network
    while(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0){
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected!\n");

    // Initialise web server
    httpd_init();
    printf("1/9 - Http server initialised\n");
    sleep_ms(1000);

    // Initialise SSI and CGI handler
    ssi_init(); 
    cgi_init();
    printf("2/9 - SSI and CGI Handler initialised\n");
    sleep_ms(1000);

    // Initialise motor pins and PWM
    initMotorSetup();
    initMotorPWM();
    printf("3/9 - Motor pins and PWM initialised\n");
    sleep_ms(1000);

    // Initialise encoder pins and setup timer to generate interrupts every second to update speed and distance
    initEncoderSetup();
    printf("4/9 - Wheel encoder pins initialised\n");
    sleep_ms(1000);

    // Initialise ultrasonic sensor
    setupUltrasonicPins();
    kalman_state *state = kalman_init(1, 100, 0, 0);
    printf("5/9 - Ultrasonic pins initialised\n");
    sleep_ms(1000);

    // Initialise wall sensors
    init_wallsensors();
    printf("6/9 - Wall sensor pins initialised\n");
    sleep_ms(1000);

    // Initialise barcode sensor pin
    init_barcode();
    printf("7/9 - Barcode sensor pin initialised\n");
    sleep_ms(1000);

    init_i2c_default();
    magnetometer_init();
    printf("8/9 - Magnetometer pins initialised\n");
    sleep_ms(1000);

    initializeMazeGrid();
    printf("9/9 - Maze grids initialised\n");
}

int main() {
    initAll();

    // Get speed and distance every second
    // struct repeating_timer timer;
    // add_repeating_timer_ms(1000, encoderCallback, NULL, &timer);
    // double cm;
    mag_t mag;

    while (1) {
        // If car is set to start running from web server
        if (startCar == 1) {

            /* Barcode scan */
            // Move straight for 2 seconds
            moveMotor(1900);
            // sleep_ms(2000);
            // stopMotor();

            /* Other stuff */
            // // Set the start cell and end cell
            // struct Cell start = {0, 1, false, false, false, false};
            // struct Cell end = {MAZE_WIDTH - 2, MAZE_HEIGHT - 1, false, false, false, false};

            // // Call pathfinding algorithm
            // // firstPathAlgo(mazeGrid, start, end);

            // // Move till touches a wall then stop for 2 seconds
            // wallDetected = false;     
            // while (!wallDetected){
            //     moveMotor(1900);
            // }
            // stopMotor();
            // sleep_ms(2000);       

            // // Get current angle
            // read_magnetometer(&mag);
            // int32_t initialAngle = get_angle(&mag); 

            // // Turn right then stop for 2 seconds
            // moveMotor(1900);
            // turnMotor(1);
            // sleep_ms(502);
            // stopMotor();

            // // Get new angle and calculate difference
            // read_magnetometer(&mag);
            // int32_t newAngle = get_angle(&mag); 
            // int32_t angleTurned = newAngle - initialAngle;
            // printf("\nAngle turned: %d", angleTurned);
            // sleep_ms(2000);       

            // // Move till touches a wall then stop for 2 seconds
            // wallDetected = false;     
            // while (!wallDetected){
            //     moveMotor(1900);
            // }
            // stopMotor();
            // sleep_ms(2000); 

            // // Get current angle
            // read_magnetometer(&mag);
            // initialAngle = get_angle(&mag); 

            // // Turn left then stop for 2 seconds
            // moveMotor(1900);
            // turnMotor(0);
            // sleep_ms(502);
            // stopMotor();

            // // Get new angle and calculate difference
            // read_magnetometer(&mag);
            // newAngle = get_angle(&mag); 
            // angleTurned = newAngle - initialAngle;
            // printf("\nAngle turned: %d", angleTurned);
            // sleep_ms(2000);   

            // // Move straight for 2 seconds
            // moveMotor(1900);
            // sleep_ms(2000);
            // stopMotor();

            // sleep_ms(10000);

            /*
            // Get distance from ultrasonic sensor
            for (int i = 0; i < 10; i++) {
                cm = getCm(state);
            }
            printf("Distance: %.2lf cm\n", cm);

            // If there is an obstacle too close, stop motor and turn right
            if (cm < 22) {            
                stopMotor();
                sleep_ms(500);
                turnMotor(1);
            } 
            else {
                // Run at half duty cycle
                moveMotor(1563);
                // Run at 32% duty cycle
                moveMotor(1000);
                sleep_ms(10000);
            }
            sleep_ms(250);
            */
        }
        // If car is set to stop
        else {
            stopMotor();
        }
    }

    return 0;
}

/*
    +---+   +---+---+
    |     S |       |
    +   +---+   +   +
    |   |       |   |
    +   +   +---+   +
    |   |   |   |   |
    +   +   +   +   +
    |       | E     |
    +---+---+   +---+
*/