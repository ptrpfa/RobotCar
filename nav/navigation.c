#include <stdio.h>
#include <stdlib.h>

// Define maze conditions
#define MAZE_WIDTH 4
#define MAZE_HEIGHT 6

// Start: (1, 0)
#define STARTING_X 1
#define STARTING_Y 0

// End: (1, 0)
#define ENDING_X 2
#define ENDING_Y 5

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

void navigateMaze(int x, int y, int end_x, int end_y)
{
    /* Base Condition */
    // Check if current coordinates is outside of map, or if current coordinates has already been visited
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT || mazeGrid[x][y].nav_visited == 1)
    {
        return;
    }

    // Mark the current cell as visited
    mazeGrid[x][y].nav_visited = 1;

    // Check if destination is reached
    if (x == end_x && y == end_y) 
    {
        printf("(%d, %d)!\n", x, y);
        return;
    }

    // If the current cell is part of the solution, print it
    printf("(%d, %d)\n", x, y);

    // Try moving in all possible directions, checking for walls (NSEW)
    if(mazeGrid[x][y].northWall == 0 && mazeGrid[x][y].visited != 2) { // No north wall
        // Move north
        navigateMaze(x, y - 1, end_x, end_y); 
    }
    if(mazeGrid[x][y].southWall == 0 && mazeGrid[x][y].visited != 2) { // No south wall
        // Move south
        navigateMaze(x, y + 1, end_x, end_y); 
    }
    if(mazeGrid[x][y].eastWall == 0 && mazeGrid[x][y].visited != 2) { // No east wall
        // Move east
        navigateMaze(x + 1, y, end_x, end_y); 
    }
    if(mazeGrid[x][y].westWall == 0 && mazeGrid[x][y].visited != 2) { // No west wall
        // Move west
        navigateMaze(x - 1, y, end_x, end_y); 
    }

}

int main() {

    // Solve maze and print shortest path
    navigateMaze(STARTING_X, STARTING_Y, ENDING_X, ENDING_Y);

    return 0;
}