// Main program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mapping.h"
#include "../../motor/motor.h"
#include "../../encoder/encoder.h"
#include "../../ultrasonic/ultrasonic.h"
#include "../../irline/wallsensor/wallsensor.h"
#include "../../irline/barcode/barcode.h"
#include "../navigation/navigation.h"

kalman_state *state; // kalman state for ultrasonic

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
    if (isValid(current_x - getDeltaX(), current_y - getDeltaX()))
    {
        // If cell behind is barcode cell, this is barcode cell
        // as barcode spans between 2 cells
        if (mazeGrid[current_x - getDeltaX()][current_y - getDeltaY()].visited == 3)
        {
            mazeGrid[current_x][current_y].visited = 3;
        }
    }

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
    if (start_scan)
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

// Function to initialize maze grid as unexplored
void initializeMazeGrid()
{
    // Set entire maze to unexplored
    for (int x = 0; x < MAZE_WIDTH; x++)
    {
        for (int y = 0; y < MAZE_HEIGHT; y++)
        {
            mazeGrid[x][y].westWall = (x == 0 ? 1 : -1);
            mazeGrid[x][y].eastWall = (x == (MAZE_WIDTH - 1) ? 1 : -1);
            mazeGrid[x][y].northWall = (y == 0 ? 1 : -1);
            mazeGrid[x][y].southWall = (y == (MAZE_HEIGHT - 1) ? 1 : -1);

            mazeGrid[x][y].visited = 0;
        }
    }
}

// Function to initialize kalman states
void initializeKalmanState()
{
    state = kalman_init(1, 100, 0, 0);
}
/*
int main()
{
    // Init all required
    initAll();
    initInterrupts();

    // Init PID
    add_repeating_timer_ms(75, pid_update_callback, NULL, &pid_timer);

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

    //  For reference
    // // Solve maze
    // solveMaze(STARTING_X, STARTING_Y, ENDING_X, ENDING_Y);
    // // cancel_repeating_timer(&pid_timer);
    // // sleep_ms(10000);
    // startCar = 0;

    return 0;
}
*/