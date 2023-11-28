#include <stdio.h>
#include <stdlib.h>
#include "navigation.h"
#include "../motor/motor.h"
#include "../encoder/encoder.h"

// Start: (1, 0)
#define STARTING_X 1
#define STARTING_Y 0

// End: (1, 0)
#define ENDING_X 2
#define ENDING_Y 5

// Initialise cells (hardcoded for now, to be populated by mapping algorithm)
struct Cell mazeGrid[MAZE_WIDTH][MAZE_HEIGHT] = {
    // (0, 0) to (0, 5)
    {  // N, S, E, W, Visited
        { 1, 0, 1, 1, 1, 0},   // Array index: (0, 0) 
        { 0, 0, 0, 1, 1, 0},   // Array index: (0, 1) 
        { 0, 0, 1, 1, 1, 0},   // Array index: (0, 2) 
        { 0, 0, 1, 1, 1, 0},   // Array index: (0, 3) 
        { 0, 0, 0, 1, 1, 0},   // Array index: (0, 4) 
        { 0, 1, 1, 1, 1, 0},   // Array index: (0, 5) 
    },
    // (1, 0) to (1, 5)
    {  // N, S, E, W, Visited
        { 0, 0, 0, 1, 1, 0},   // Array index: (1, 0) 
        { 0, 1, 1, 0, 1, 0},   // Array index: (1, 1) 
        { 1, 0, 1, 1, 1, 0},   // Array index: (1, 2) 
        { 0, 0, 0, 1, 1, 0},   // Array index: (1, 3) 
        { 0, 1, 1, 0, 1, 0},   // Array index: (1, 4) 
        { 1, 1, 0, 1, 1, 0},   // Array index: (1, 5) 
    },
    // (2, 0) to (2, 5)
    {  // N, S, E, W, Visited
        { 1, 0, 1, 0, 1, 0},   // Array index: (2, 0) 
        { 0, 0, 0, 1, 1, 0},   // Array index: (2, 1) 
        { 0, 0, 1, 1, 1, 0},   // Array index: (2, 2) 
        { 0, 1, 0, 0, 1, 0},   // Array index: (2, 3) 
        { 1, 0, 0, 1, 1, 0},   // Array index: (2, 4) 
        { 0, 0, 1, 0, 1, 0},   // Array index: (2, 5) 
    },
    // (3, 0) to (3, 5)
    {  // N, S, E, W, Visited
        { 1, 0, 1, 1, 1, 0},   // Array index: (3, 0) 
        { 0, 0, 1, 0, 1, 0},   // Array index: (3, 1) 
        { 0, 1, 1, 1, 1, 0},   // Array index: (3, 2) 
        { 1, 0, 1, 0, 1, 0},   // Array index: (3, 3) 
        { 0, 0, 1, 0, 1, 0},   // Array index: (3, 4) 
        { 0, 1, 1, 1, 1, 0},   // Array index: (3, 5) 
    },
    
};


// Initialise cells (hardcoded for now, to be populated by mapping algorithm)
// struct Cell mazeGrid[MAZE_WIDTH][MAZE_HEIGHT] = {
//     // (0, 0) to (0, 5)
//     {  // N, S, E, W, Visited
//         { 1, 1, 0, 1, 1, 0},   // Array index: (0, 0) 
//         { 1, 0, 1, 1, 1, 0},   // Array index: (0, 1) 
//         { 0, 1, 0, 1, 1, 0},   // Array index: (0, 2) 
//         { 1, 1, 0, 1, 1, 0},   // Array index: (0, 3) 
//         { 1, 0, 0, 1, 1, 0},   // Array index: (0, 4) 
//         { 0, 1, 1, 1, 1, 0},   // Array index: (0, 5) 
//     },
//     // (1, 0) to (1, 5)
//     {  // N, S, E, W, Visited
//         { 0, 0, 0, 0, 1, 0},   // Array index: (1, 0) 
//         { 0, 1, 1, 1, 1, 0},   // Array index: (1, 1) 
//         { 1, 1, 0, 0, 1, 0},   // Array index: (1, 2) 
//         { 1, 1, 0, 0, 1, 0},   // Array index: (1, 3) 
//         { 1, 0, 1, 0, 1, 0},   // Array index: (1, 4) 
//         { 0, 1, 0, 1, 1, 0},   // Array index: (1, 5) 
//     },
//     // (2, 0) to (2, 5)
//     {  // N, S, E, W, Visited
//         { 1, 0, 0, 0, 1, 0},   // Array index: (2, 0) 
//         { 0, 0, 0, 1, 1, 0},   // Array index: (2, 1) 
//         { 0, 1, 1, 0, 1, 0},   // Array index: (2, 2) 
//         { 1, 1, 0, 0, 1, 0},   // Array index: (2, 3) 
//         { 1, 0, 1, 1, 1, 0},   // Array index: (2, 4) 
//         { 0, 0, 0, 0, 1, 0},   // Array index: (2, 5) 
//     },
//     // (3, 0) to (3, 5)
//     {  // N, S, E, W, Visited
//         { 1, 1, 1, 0, 1, 0},   // Array index: (3, 0) 
//         { 1, 0, 1, 0, 1, 0},   // Array index: (3, 1) 
//         { 0, 0, 1, 1, 1, 0},   // Array index: (3, 2) 
//         { 0, 0, 1, 0, 1, 0},   // Array index: (3, 3) 
//         { 0, 0, 1, 1, 1, 0},   // Array index: (3, 4) 
//         { 0, 1, 1, 0, 1, 0},   // Array index: (3, 5) 
//     },
    
// };

bool isMazeMapped = false;
int navigationPosition = 0; 
struct repeating_timer pidTimer;

// Recursive function to find the shortest path
void navigateMaze(int x, int y, int end_x, int end_y, struct Coordinates path[], int pathLength, int *shortest_path_length, struct Coordinates *shortest_path)
{
    // Base Condition
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT || mazeGrid[x][y].nav_visited == 1)
    {
        return;
    }

    // Mark the current cell as visited
    mazeGrid[x][y].nav_visited = 1;

    // Add the current point to the path
    path[pathLength].x = x;
    path[pathLength].y = y;

    // Check if destination is reached
    if (x == end_x && y == end_y)
    {
        // If the current path is shorter than the previously found minimum path, update the minimum path
        if (pathLength < *shortest_path_length)
        {
            *shortest_path_length = pathLength;
            for (int i = 0; i <= pathLength; ++i)
            {
                shortest_path[i] = path[i];
            }
        }

        // Backtrack: Mark the current point as unvisited when exploring other paths
        mazeGrid[x][y].nav_visited = 0;
        return;
    }

    // Try moving in all possible directions, checking for walls (NSEW)
    if (mazeGrid[x][y].northWall == 0 && mazeGrid[x][y].visited != 2)
    { // No north wall
        // Move north
        navigateMaze(x, y - 1, end_x, end_y, path, pathLength + 1, shortest_path_length, shortest_path);
    }
    if (mazeGrid[x][y].southWall == 0 && mazeGrid[x][y].visited != 2)
    { // No south wall
        // Move south
        navigateMaze(x, y + 1, end_x, end_y, path, pathLength + 1, shortest_path_length, shortest_path);
    }
    if (mazeGrid[x][y].eastWall == 0 && mazeGrid[x][y].visited != 2)
    { // No east wall
        // Move east
        navigateMaze(x + 1, y, end_x, end_y, path, pathLength + 1, shortest_path_length, shortest_path);
    }
    if (mazeGrid[x][y].westWall == 0 && mazeGrid[x][y].visited != 2)
    { // No west wall
        // Move west
        navigateMaze(x - 1, y, end_x, end_y, path, pathLength + 1, shortest_path_length, shortest_path);
    }

    // Backtrack: Mark the current point as unvisited when exploring other paths
    mazeGrid[x][y].nav_visited = 0;
}

bool pidUpdateCallback(struct repeating_timer *t)
{
    encoderCallback();
    update_motor_speed();
    return true;
}

// Solve 
void solveMaze(int start_x, int start_y, int end_x, int end_y) {
    // Initialize exploration path variables
    struct Coordinates path[MAZE_WIDTH * MAZE_HEIGHT];
    int pathLength = 0;

    // Initialize variables to store the minimum path
    int shortest_path_length = MAZE_WIDTH * MAZE_HEIGHT + 1; // Initialize to a value greater than the maximum possible path length
    struct Coordinates shortest_path[MAZE_WIDTH * MAZE_HEIGHT];

    // Find the shortest path using DFS
    navigateMaze(start_x, start_y, end_x, end_y, path, pathLength, &shortest_path_length, shortest_path);

    // int found_path[9][2] = {{1, 0}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {3, 3}, {3, 4}, {2, 4}, {2, 5}}; 
    // int found_path[9][2] = {{1, 0}, {2, 0}, {2, 1}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {2, 5}};
    
    // Initialise starting positions
    int currentX = STARTING_X;
    int currentY = STARTING_Y;

    for (int i = 0; i < shortest_path_length + 1; ++i) {
        if (navigationPosition != 0) {
            if (navigationPosition == -1) {
                turnMotor(1);
            }   
            else if (navigationPosition == 1) {
                turnMotor(0);
            }
            navigationPosition = 0;
        }

        int nextX = shortest_path[i].x;
        int nextY = shortest_path[i].y;

        int deltaX = nextX - currentX;
        int deltaY = nextY - currentY;


        add_repeating_timer_ms(100, pidUpdateCallback, NULL, &pidTimer);
        if (deltaX == 1) {
            // Move left
            turnMotor(0);
            moveGrids(1);
            navigationPosition -= 1;
        } else if (deltaX == -1) {
            // Move right
            turnMotor(1);
            moveGrids(1);
            navigationPosition += 1;
        } else if (deltaY == 1) {
            // Move down
            moveGrids(1);
        } else if (deltaY == -1) {
            // Move up
            reverseGrids(1);
        }

        currentX = nextX;
        currentY = nextY;
        sleep_ms(2000);
        cancel_repeating_timer(&pidTimer);
    }

    // Arrived at the destination
    if (navigationPosition != 0) {
        if (navigationPosition == -1) {
            turnMotor(1);
        }   
        else if (navigationPosition == 1) {
            turnMotor(0);
        }
        navigationPosition = 0;
    }
    stopMotor();
}

/*
int main() {
    // Find shortest path
    navigateMaze(STARTING_X, STARTING_Y, ENDING_X, ENDING_Y);
    
    // Solve the maze according to shortest path found
    solveMaze();

    return 0;
}
*/