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
#include "navigation.h"
#include "mapping.h"

const char WIFI_SSID[] = "P";            // Wifi credentials
const char WIFI_PASSWORD[] = "embedded"; // Wifi credentials
int position = SOUTH;                    // 0 - S, 1 - W, 2 - N, 3 - E

int startCar = 0;                         // From CGI to toggle car start / stop
int cellsLeft = MAZE_HEIGHT * MAZE_WIDTH; // Total numbers of cells to map
kalman_state *state;                      // kalman state for ultrasonic

// Function to check if x, y is within maze boudaries
int isValid(int x, int y)
{
    return x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_HEIGHT;
}

//  Mark a cell as visited if all walls are marked
void checkIfVisited(int x, int y)
{
    if (mazeGrid[x][y].visited == 0 &&
        mazeGrid[x][y].northWall != -1 &&
        mazeGrid[x][y].southWall != -1 &&
        mazeGrid[x][y].eastWall != -1 &&
        mazeGrid[x][y].westWall != -1)
    {
        mazeGrid[x][y].visited = 1;
        cellsLeft -= 1;
    }
}

// Returns visit status of cell in front of car
int getVisitStatus(int current_x, int current_y)
{
    switch (position)
    {
    case SOUTH:
        if (isValid(current_x, current_y + 1))
        {
            return mazeGrid[current_x][current_y + 1].visited;
        }
        break;
    case WEST:
        if (isValid(current_x - 1, current_y))
        {
            return mazeGrid[current_x - 1][current_y].visited;
        }
        break;
    case NORTH:
        if (isValid(current_x, current_y - 1))
        {
            return mazeGrid[current_x][current_y - 1].visited;
        }
        break;
    case EAST:
        if (isValid(current_x + 1, current_y))
        {
            return mazeGrid[current_x + 1][current_y].visited;
        }
        break;
    }
    // Cell is invalid, return 1 so car won't go there
    return 1;
}

// Return wall status
// Number of turns in CLOCKWISE!
int getWallStatus(int current_x, int current_y, int number_of_turns)
{
    int new_position = number_of_turns + position;

    if (new_position > 3)
    {
        new_position -= 4;
    }

    switch (new_position)
    {
    case SOUTH:
        // South
        return mazeGrid[current_x][current_y].southWall;
        break;
    case WEST:
        // West
        return mazeGrid[current_x][current_y].westWall;
        break;
    case NORTH:
        // North
        return mazeGrid[current_x][current_y].northWall;
        break;
    case EAST:
        // East
        return mazeGrid[current_x][current_y].eastWall;
        break;
    }
}

// Update wall status of coordinate and adjacent cell
void updateWall(int wall, int x, int y)
{
    switch (position)
    {
    // South
    case SOUTH:
        mazeGrid[x][y].southWall = wall;
        if (y < MAZE_HEIGHT - 1)
        {
            mazeGrid[x][y + 1].northWall = wall;
            checkIfVisited(x, y + 1);
        }
        break;
    // West
    case WEST:
        mazeGrid[x][y].westWall = wall;
        if (x > 0)
        {
            mazeGrid[x - 1][y].eastWall = wall;
            checkIfVisited(x - 1, y);
        }
        break;
    // North
    case NORTH:
        mazeGrid[x][y].northWall = wall;
        if (y > 0)
        {
            mazeGrid[x][y - 1].southWall = wall;
            checkIfVisited(x, y - 1);
        }
        break;
    // East
    case EAST:
        mazeGrid[x][y].eastWall = wall;
        if (x < MAZE_WIDTH - 1)
        {
            mazeGrid[x + 1][y].westWall = wall;
            checkIfVisited(x + 1, y);
        }
        break;
    }
    checkIfVisited(x, y);
    return;
}

// Update coordinate if cell in front of car is an obstacle
void updateObstacle(int x, int y)
{
    switch (position)
    {
    case SOUTH: // South
        mazeGrid[x][y + 1].visited = 2;
        cellsLeft -= 1;
        break;
    case WEST: // West
        mazeGrid[x - 1][y].visited = 2;
        cellsLeft -= 1;
        break;
    case NORTH: // North
        mazeGrid[x][y - 1].visited = 2;
        cellsLeft -= 1;
        break;
    case EAST: // East
        mazeGrid[x + 1][y].visited = 2;
        cellsLeft -= 1;
        break;
    }
    return;
}

// Function for PID timer callback
bool pid_update_callback(struct repeating_timer *t)
{
    encoderCallback();
    update_motor_speed();
    return true;
}

// Change in x-coordinate based on the direction
int getDeltaX()
{
    if (position == WEST)
    {
        return -1;
    }

    if (position == EAST)
    {
        return 1;
    }

    return 0;
}

// Change in y-coordinate based on the direction
int getDeltaY()
{
    if (position == NORTH)
    {
        return -1;
    }

    if (position == SOUTH)
    {
        return 1;
    }

    return 0;
}

// Recursive function to map out the entire maze
void mapMaze(int current_x, int current_y)
{
    double cm; // To check for obstacle

    // Update current cell as visited
    mazeGrid[current_x][current_y].visited = 1;

    // Check if cell behind car is valid
    // if (isValid(current_x - getDeltaX(), current_y - getDeltaX()))
    // {
    //     // If cell behind is barcode cell, this is barcode cell
    //     // as barcode spans between 2 cells
    //     if (mazeGrid[current_x - getDeltaX()][current_y - getDeltaY()].visited == 3)
    //     {
    //         mazeGrid[current_x][current_y].visited = 3;
    //     }
    // }

    // Don't allow barcode scanning if cell already marked as barcode cell
    scanning_allowed = !(mazeGrid[current_x][current_y].visited == 3);

    // Check if theres an obstacle
    for (int i = 0; i < 20; i++)
    {
        cm = getCm(state);
    }

    // If theres an obstacle, update the obstacle cell
    if (cm < 10)
    {
        printf("Obstacle Detected %.2lf away!\n", cm);
        updateObstacle(current_x, current_y);
        obstacleDetected = false; // Reset obstacle detected flag
    }

    /* If barcode is scanning,
     * move one more grid to finish scanning,
     * mark as barcode cell and reverse one grid to return to current cell
     */
    if (start_scan && barcode_char == '#')
    {
        printf("Barcode scanning\n");

        moveGrids(1);
        sleep_ms(1500);
        mazeGrid[current_x][current_y].visited = 3;
        reverseGrids(1);
        sleep_ms(1500);
    }
    
    /***** Start the recursive journey *****/
    // Check front wall first
    if (isWallDetected())
    {
        printf("Wall at the front\n");
        // Theres a wall in front!
        updateWall(1, current_x, current_y);
    }
    else if (getVisitStatus(current_x, current_y) == 0)
    {
        // No wall in front and cell not visited!
        // Update wall status
        updateWall(0, current_x, current_y);

        // Recursive
        moveGrids(1);
        sleep_ms(1500);
        mapMaze(current_x + getDeltaX(), current_y + getDeltaY());
    }

    /***** Front side settled, now check right *****/
    // Check if wall on the right is checked
    if (getWallStatus(current_x, current_y, 1) == -1)
    {
        printf("Checking right\n");
        // Car does not know about the wall on the right, turn
        turnMotor(1);
        position = (position + 1) % 4;
        sleep_ms(1500);

        // Check if theres an obstacle
        for (int i = 0; i < 20; i++)
        {
            cm = getCm(state);
        }

        // If theres an obstacle, update the obstacle cell
        if (cm < 10)
        {
            printf("Obstacle on the right!\n");
            updateObstacle(current_x, current_y);
            obstacleDetected = false; // Reset obstacle detected flag
        }
        else
        {
            // No obstacles, check if there's a wall
            if (isWallDetected())
            {
                // There's a wall!
                printf("Wall on the right!\n");
                updateWall(1, current_x, current_y);
            }
            else
            {
                // There's no wall, recursive if cell is not visited yet
                updateWall(0, current_x, current_y);
                if (getVisitStatus(current_x, current_y) == 0)
                {
                    moveGrids(1);
                    sleep_ms(1500);

                    mapMaze(current_x + getDeltaX(), current_y + getDeltaY());
                }
            }
        }
        // Reset position
        turnMotor(0);
        position = (position + 3) % 4;
        sleep_ms(1500);
    }

    /***** Lastly, left *****/
    // Check if wall on the left is checked
    if (getWallStatus(current_x, current_y, 3) == -1)
    {
        // Car does not know about the wall on the left, turn
        turnMotor(0);
        position = (position + 3) % 4;

        sleep_ms(1500);

        // Check if theres an obstacle
        for (int i = 0; i < 20; i++)
        {
            cm = getCm(state);
        }

        // If theres an obstacle, update the obstacle cell
        if (cm < 10)
        {
            updateObstacle(current_x, current_y);
            obstacleDetected = false; // Reset obstacle detected flag
        }
        else
        {
            // No obstacles, check if there's a wall
            if (isWallDetected())
            {
                // There's a wall!
                updateWall(1, current_x, current_y);
            }
            else
            {
                // There's no wall, recursive
                updateWall(0, current_x, current_y);
                if (getVisitStatus(current_x, current_y) == 0)
                {
                    moveGrids(1);
                    sleep_ms(1500);

                    mapMaze(current_x + getDeltaX(), current_y + getDeltaY());
                }
            }
        }

        // Reset position
        turnMotor(1);
        position = (position + 1) % 4;
        sleep_ms(1500);
    }

    // Go back if not starting grid
    if (current_x != STARTING_X || current_y != STARTING_Y)
    {
        reverseGrids(1);
        sleep_ms(1500);
    }

    return;
}

// Function that is invoked upon a change in right IR sensor's input
void callbacks(uint gpio, uint32_t events)
{
    switch (gpio)
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
    // Barcode sensor callback
    case IR_SENSOR_PIN:
        barcode_callback(gpio, events);
        break;
    default:
        break;
    }
}

// Function to init all sensors and motors
void initAll()
{
    // Initialise standard I/O
    stdio_init_all();
    sleep_ms(1000);

    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    sleep_ms(1000);

    // Loop until connected to WiFI network
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0)
    {
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected!\n");

    // Initialise web server
    httpd_init();
    printf("1/9 - Http server initialised\n");
    sleep_ms(500);

    // Initialise SSI and CGI handler
    ssi_init();
    cgi_init();
    printf("2/9 - SSI and CGI Handler initialised\n");
    sleep_ms(500);

    // Initialise motor pins and PWM
    initMotorSetup();
    initMotorPWM();
    printf("3/9 - Motor pins and PWM initialised\n");
    sleep_ms(500);

    // Initialise encoder pins and setup timer to generate interrupts every second to update speed and distance
    initEncoderSetup();
    printf("4/9 - Wheel encoder pins initialised\n");
    sleep_ms(500);

    // Initialise ultrasonic sensor
    setupUltrasonicPins();
    printf("5/9 - Ultrasonic pins initialised\n");
    sleep_ms(500);

    // Initialise wall sensors
    init_wallsensors();
    printf("6/9 - Wall sensor pins initialised\n");
    sleep_ms(500);

    // Initialise barcode sensor pin
    init_barcode();
    printf("7/9 - Barcode sensor pin initialised\n");
    sleep_ms(500);

    init_i2c_default();
    magnetometer_init();
    printf("8/9 - Magnetometer pins initialised\n");
    sleep_ms(500);

    initializeMazeGrid();
    printf("9/9 - Maze grids initialised\n");
}

// Function to init all interrupts
void initInterrupts()
{
    // Initialise interrupts for needed sensors
    gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(R_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(ECHOPIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);
    gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);
}

int main()
{
    // Init all required
    initAll();
    initInterrupts();

    // Init PID
    add_repeating_timer_ms(50, pid_update_callback, NULL, &pid_timer);

    double cm;

    while (startCar == 0)
    {
        // Wait
    }

    // Maybe change below into a function for cgi

    mapMaze(STARTING_X, STARTING_Y);

    printf("** MAZE MAPPING COMPLETED **\n");
    printf("** MAP DETAILS **\n");

    for (int x = 0; x < MAZE_WIDTH; x++)
    {
        for (int y = 0; y < MAZE_HEIGHT; y++)
        {
            printf("X: %d, Y: %d, SOUTHWALL: %d, WESTWALL: %d, NORTHWALL: %d, EASTWALL: %d, VISITED: %d\n\n", x, y, mazeGrid[x][y].southWall, mazeGrid[x][y].westWall, mazeGrid[x][y].northWall, mazeGrid[x][y].eastWall, mazeGrid[x][y].visited);
        }
    }
    isMazeMapped = true;
    printf("TIME TO NAVIGATE\n");
    sleep_ms(5000);

    //  For reference
    // Solve maze
    solveMaze(STARTING_X, STARTING_Y, ENDING_X, ENDING_Y);
    // cancel_repeating_timer(&pid_timer);
    // sleep_ms(10000);
    startCar = 0;

    return 0;
}
