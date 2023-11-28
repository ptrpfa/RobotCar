#ifndef navigation_h
#define navigation_h

#include <stdio.h>
#include "pico/stdlib.h"

// Define maze conditions
#define MAZE_WIDTH 4
#define MAZE_HEIGHT 6

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

// Struct to represent a point in the maze
struct Coordinates
{
    int x;
    int y;
};

// External variables
extern struct Cell mazeGrid[MAZE_WIDTH][MAZE_HEIGHT];
extern bool isMazeMapped;

// Functions for motors
void navigateMaze(int x, int y, int end_x, int end_y, struct Coordinates path[], int pathLength, int *shortest_path_length, struct Coordinates *shortest_path);
bool pid_update_callback(struct repeating_timer *t);
void solveMaze();

#endif