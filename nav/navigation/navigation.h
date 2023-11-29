#ifndef navigation_h
#define navigation_h

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

// Struct to represent a point in the maze
struct Coordinates
{
    int x;
    int y;
};

// External variables
extern bool isMazeMapped;

// Functions for motors
void navigateMaze(int x, int y, int end_x, int end_y, struct Coordinates path[], int pathLength, int *shortest_path_length, struct Coordinates *shortest_path);
bool pidUpdateCallback(struct repeating_timer *t);
void solveMaze(int start_x, int start_y, int end_x, int end_y);

#endif