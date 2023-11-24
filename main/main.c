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
#define MAZE_WIDTH 6
#define MAZE_HEIGHT 4
#define STARTING_X 0
#define STARTING_Y 2 
// Struct for maze cell
struct Cell {
    int northWall;  // -1 - unvisited, 0 - empty, 1 - wall
    int southWall;  // -1 - unvisited, 0 - empty, 1 - wall
    int eastWall;   // -1 - unvisited, 0 - empty, 1 - wall
    int westWall;   // -1 - unvisited, 0 - empty, 1 - wall
    int visited;    // 0 - unvisited, 1 - visited, 2 - obstacle, 3 - barcode
};

struct Cell mazeGrid[MAZE_WIDTH][MAZE_HEIGHT];
struct repeating_timer pid_timer;

// Global variables
const char WIFI_SSID[] = "SINGTEL-6CAN";           // Wifi credentials
const char WIFI_PASSWORD[] = "ser760172";    // Wifi credentials
int position = 0;                           // 0 - S, 1 - W, 2 - N, 3 - E
int startCar = 0;                           // From CGI to toggle car start / stop
int cellsLeft = MAZE_HEIGHT * MAZE_WIDTH;   // Total numbers of cells to map
bool wallDetected = false;                   // Wall detection variable
bool leftWallDetected = false;                   // Wall detection variable
bool rightWallDetected = false;                   // Wall detection variable
bool oneGrid = false;

// Function to check if x, y is within maze boudaries
int isValid(int x, int y) {
    return x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT;
}

void updateWall(int wall, int x, int y, int new_position) {
    switch(new_position) 
    {
        // South
        case 0:
            mazeGrid[x][y].southWall = wall;
            if (x < MAZE_WIDTH - 2) {
                mazeGrid[x+1][y].northWall = wall;
            }
            break;
        // West
        case 1:
            mazeGrid[x][y].westWall = wall;
            if (x < MAZE_HEIGHT - 2) {
                mazeGrid[x+1][y].eastWall = wall;
            }
            break;
        // North
        case 2:
            mazeGrid[x][y].northWall = wall;
            if (x > 0) {
                mazeGrid[x+1][y].southWall = wall;
            }
            break;
        // East
        case 3:
            mazeGrid[x][y].eastWall = wall;
            if (x > 0) {
                mazeGrid[x+1][y].westWall = wall;
            }
            break;
    }
}

bool pid_update_callback(struct repeating_timer *t) {
    encoderCallback();
    update_motor_speed();
    return true; 
}

void firstPathAlgo() {   	
    // Move one grid
    int current_x = 2;
    int current_y = 0;

    // Start facing south and move
    add_repeating_timer_ms(100, pid_update_callback, NULL, &pid_timer);
    moveMotor(pwmL, pwmR);
    while (!wallDetected && !obstacleDetected) {
        // Wait
    }
    // Encountered wall, stop
    cancel_repeating_timer(&pid_timer);
    stopMotor();

    uint32_t grids_moved = getGridsMoved();
    printf("GRIDS MOVED: %d\n", grids_moved);
    sleep_ms(2000);
    
    // Update mazegrid
    wallDetected = false;
    updateWall(1, current_x, current_y, position);

    // Turn right
    turnMotor(1);
    sleep_ms(2000);

    // Update position
    position++;
    
    if (mazeGrid[current_x][current_y].westWall == -1) {
        if (wallDetected) {
            updateWall(1, current_x, current_y, position);
        } else {
            updateWall(0, current_x, current_y, position);
        }
    }
    sleep_ms(2000);
    wallDetected = false;

    // Turn 180
    turnMotor(1);
    turnMotor(1);
    sleep_ms(2000);
    position += 2;

    if (mazeGrid[current_x][current_y].eastWall == -1) {
        if (wallDetected) {
            updateWall(1, current_x, current_y, position);
        } else {
            updateWall(0, current_x, current_y, position);
        }
    }

    sleep_ms(2000);

    printf("STATUS CHECK\n");
    printf("SOUTHWALL: %d\n", mazeGrid[current_x][current_y].southWall);
    printf("EASTWALL: %d\n", mazeGrid[current_x][current_y].eastWall);
    printf("WESTWALL: %d\n", mazeGrid[current_x][current_y].westWall);

    return;
}

void hard_coded(){
    turnMotor(1);
    sleep_ms(1500);
    
    // add_repeating_timer_ms(1000, pid_update_callback, NULL, &pid_timer);
    moveMotor(pwmL, pwmR);
    while (!wallDetected) {
        // Wait
    }

    // Encounterd wall, stop
    stopMotor();
    sleep_ms(1500);

    turnMotor(0);
    wallDetected = false;
    sleep_ms(1500);

    add_repeating_timer_ms(1000, pid_update_callback, NULL, &pid_timer);
    moveMotor(pwmL, pwmR);
    while (!wallDetected) {
        // Wait
    }

    // Encounterd wall, stop
    stopMotor();
    sleep_ms(1500);

    turnMotor(0);
    wallDetected = false;
    sleep_ms(1500);

    add_repeating_timer_ms(1000, pid_update_callback, NULL, &pid_timer);
    moveMotor(pwmL, pwmR);
    while (!wallDetected) {
        // Wait
    }

    // Encounterd wall, stop
    stopMotor();
    sleep_ms(1500);

    turnMotor(1);
    wallDetected = false;
    sleep_ms(1500);
}
// Function to find the path using a flood-fill algorithm
/*
* OLD ALGO
void firstPathAlgo() {   	
    if (cellsLeft == 0) {
        return;
    }

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
        int newX = mazeGrid + dx[i];
        int newY = mazeGrid + dy[i];

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
                    sleep_ms(2000);

                    // Recursively explore the next cell
                    firstPathAlgo(mazeGrid);

                    // To break loop once returned
                    action = true;
                } 
                else if (wallDetected) {
                    // Stop motor once wall detected
                    stopMotor();
                    sleep_ms(2000);

                    // Turn to the right to check next cell
                    moveMotor(pwmL,pwmR);
                    turnMotor(1);
                    sleep_ms(502);
                    stopMotor();

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
                    moveMotor(pwmL,pwmR);
                }
            }
        }   
        // New position is not valid or has been explored
        else {
            // Turn to the right
            turnMotor(1);
            sleep_ms(525);
            stopMotor();
            position += 1;
        }
    }
}
*/


// Function that is invoked upon a change in right IR sensor's input
void callbacks(uint gpio, uint32_t events) {
    switch(gpio) 
    {
        // Left wheel encoder callback
        case L_ENCODER_OUT:
            encoderPulse(L_ENCODER_OUT);
            break;
        // Right wheel encoder callback
        case R_ENCODER_OUT:
            encoderPulse(R_ENCODER_OUT);
            break;
        // Ultrasonic callback
        case ECHOPIN:
            get_echo_pulse(ECHOPIN, events);
            break;
        // Left wall sensor callback
        case LEFT_IR_PIN:
            printf("WALL DETECTED\n");
            leftWallDetected = true;
            break;
        // Right wall sensor callback
        case RIGHT_IR_PIN:
            printf("WALL DETECTED\n");
            rightWallDetected = true;
            break;
        // Barcode sensor callback
        case IR_SENSOR_PIN:
            barcode_callback(gpio);
            break;
        default:
            break;
    }
}

// Function to initialize maze grid as unexplored
void initializeMazeGrid() {
    // Set entire maze to unexplored
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            mazeGrid[x][y].northWall = -1;
            mazeGrid[x][y].southWall = -1;
            mazeGrid[x][y].eastWall = -1;
            mazeGrid[x][y].westWall = -1;
            mazeGrid[x][y].visited = 0;
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

void initInterrupts() {
    // Initialise interrupts for needed sensors
    gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(R_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(ECHOPIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);
    gpio_set_irq_enabled_with_callback(LEFT_IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(RIGHT_IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);
}


int main() {
    // Init all required
    initAll();
    initInterrupts();
    
    // Init PID
    // add_repeating_timer_ms(1000, pid_update_callback, NULL, &pid_timer);

    kalman_state *state = kalman_init(1, 100, 0, 0);
    // double cm;

    while(true) {
        if (startCar == 1) {
            // Call pathfinding algorithm
            // firstPathAlgo();
            // hard_coded();
            add_repeating_timer_ms(100, pid_update_callback, NULL, &pid_timer);
            moveGrids(4);
            cancel_repeating_timer(&pid_timer);
            sleep_ms(10000);
        }
    }

    return 0;    
}   

/*
* OLD MAIN
int main() {
    // Init all required
    initAll();

    // Initialise interrupts for needed sensors
    gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(R_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(ECHOPIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);
    gpio_set_irq_enabled_with_callback(LEFT_IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(RIGHT_IR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);

    // Get speed and distance every second
    // struct repeating_timer timer;
    // add_repeating_timer_ms(10000, encoderCallback, NULL, &timer);
    kalman_state *state = kalman_init(1, 100, 0, 0);
    double cm;
    mag_t mag;

    while (1) {
        // If car is set to start running from web server
        if (startCar == 1) {
            // Call pathfinding algorithm
            // firstPathAlgo(mazeGrid);
            
            // Move till touches a wall then stop for 2 seconds
            wallDetected = false;     
            moveMotor(pwmL,pwmR);
            while (!wallDetected || !obstacleDetected){
                // Wait
            }
            stopMotor();
            sleep_ms(2000);       

            // Get current angle
            // read_magnetometer(&mag);
            // int32_t angleTurned; 
            // int32_t initialAngle = get_angle(&mag); 
            // int32_t newAngle = initialAngle;
            // int32_t target_angle;
            
            // Turn right
            // moveMotor(pwmL,pwmR);
            turnMotor(1);
            // turnRight();
            // if (initialAngle < 90) {
            //     target_angle = 270 + initialAngle;
            //     printf("INITIAL ANGLE: %d, NEW ANGLE: %d, TARGET ANGLE: %d\n", initialAngle, newAngle, target_angle);
            //     moveMotor(pwmL,pwmR);
            //     while (newAngle <= initialAngle || newAngle > target_angle ) {
            //         read_magnetometer(&mag);
            //         newAngle = get_angle(&mag); 
            //         printf("initialAngle: %d, newAngle: %d\n", initialAngle, newAngle);
            //         turnMotor(1);
            //     }
            // } else {
            //     target_angle = initialAngle - 90;
            //     printf("INITIAL ANGLE: %d, NEW ANGLE: %d, TARGET ANGLE: %d\n", initialAngle, newAngle, target_angle);
            //     moveMotor(pwmL,pwmR);
            //     while (newAngle >= target_angle ) {
            //         read_magnetometer(&mag);
            //         newAngle = get_angle(&mag); 
            //         printf("initialAngle: %d, newAngle: %d\n", initialAngle, newAngle);
            //         turnMotor(1);
            //     }
            // }

            // stopMotor();

            // printf("Angle turned: %d\n", angleTurned);
            // sleep_ms(2000);       

            // Move till touches a wall then stop for 2 seconds
            wallDetected = false;     
            moveMotor(pwmL,pwmR);
            while (!wallDetected){
                // Wait
            }
            stopMotor();
            sleep_ms(2000); 

            turnMotor(0);


            // // Get current angle
            // read_magnetometer(&mag);
            // initialAngle = get_angle(&mag); 

            // // Turn left then stop for 2 seconds
            // moveMotor(pwmL,pwmR);
            // turnMotor(0);
            // sleep_ms(500);
            // stopMotor();

            // // Get new angle and calculate difference
            // read_magnetometer(&mag);
            // newAngle = get_angle(&mag); 
            // angleTurned = newAngle - initialAngle;
            // printf("Angle turned: %d\n", angleTurned);
            // sleep_ms(2000);   
        
            // bool obstacle = false;
            // // Move straight
            // moveMotor(pwmL,pwmR);

            // while (!obstacle) {
            //     // Get distance from ultrasonic sensor
            //     for (int i = 0; i < 10; i++) {
            //         cm = getCm(state);
            //     }
            //     printf("Distance: %.2lf\n", cm);
     

            //     // If there is an obstacle too close, stop motor and u-turn right
            //     if (cm < 5) {            
            //         stopMotor();
            //         sleep_ms(2000);
            //         turnMotor(1);
            //         sleep_ms(1200);
            //         stopMotor();
            //         obstacle = true;
            //     } 
            // }        
        }
    }

    struct repeating_timer pid_timer;
    add_repeating_timer_ms(10000,pid_update_callback,NULL,&pid_timer);

    while (1) {
        // If car is set to start running from web server
        if (startCar == 1) {
            // moveMotor(pwmL,pwmR));
            printf("PWML IN MAIN: LEFT: %f, RIGHT: %f\n",pwmL,pwmR);
            moveMotor(pwmL,pwmR);
            sleep_ms(1500);
        }
        // Car is set to stop
        else {
            stopMotor();
        }
    }
    return 0;
}
*/

