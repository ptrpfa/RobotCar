#ifndef mapping_h
#define mapping_h

#include <stdio.h>
#include "pico/stdlib.h"

// Define maze conditions, starting and ending grid
#define MAZE_WIDTH 4
#define MAZE_HEIGHT 6

// Start: (1, 0)
#define STARTING_X 1
#define STARTING_Y 0

// End: (2, 5)
#define ENDING_X 2
#define ENDING_Y 5

struct repeating_timer pid_timer;

typedef enum
{
    SOUTH = 0,
    WEST = 1,
    NORTH = 2,
    EAST = 3
} Direction;

// Global variables
int position = SOUTH;                       // 0 - S, 1 - W, 2 - N, 3 - E
int startCar = 0;                           // From CGI to toggle car start / stop
int cellsLeft = MAZE_HEIGHT * MAZE_WIDTH;   // Total numbers of cells to map

// Functions for motors
int isValid(int x, int y);
void checkIfVisited(int x, int y);
int getVisitStatus(int current_x, int current_y);
int getWallStatus(int current_x, int current_y, int number_of_turns);
void updateWall(int wall, int x, int y);
void updateObstacle(int x, int y);
int getDeltaX();
int getDeltaY();
void mapMaze(int current_x, int current_y);
void initializeMazeGrid();
void initializeKalmanState();

#endif