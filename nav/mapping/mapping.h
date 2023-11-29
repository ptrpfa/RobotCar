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

// Struct for maze cell
struct Cell
{
    int northWall; // -1 - unvisited, 0 - empty, 1 - wall
    int southWall; // -1 - unvisited, 0 - empty, 1 - wall
    int eastWall;  // -1 - unvisited, 0 - empty, 1 - wall
    int westWall;  // -1 - unvisited, 0 - empty, 1 - wall

    // Unused
    int visited;   // 0 - unvisited, 1 - visited, 2 - obstacle, 3 - barcode
    
    // Navigation
    int nav_visited; // 0 - unvisted, 1 - visited
};

typedef enum
{
    SOUTH = 0,
    WEST = 1,
    NORTH = 2,
    EAST = 3
} Direction;

// External variables
extern struct Cell mazeGrid[MAZE_WIDTH][MAZE_HEIGHT];

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