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

// Struct to represent a point in the maze
struct Point
{
    int x;
    int y;
};

// Function to print the path
void printPath(struct Point path[], int length)
{
    for (int i = 0; i < length; ++i)
    {
        printf("(%d, %d) ", path[i].x, path[i].y);
    }
    printf("\n");
}

// Recursive function to find the shortest path
void findShortestPath(int x, int y, int end_x, int end_y, struct Point path[], int pathLength, int *minPathLength, struct Point *minPath)
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
        if (pathLength < *minPathLength)
        {
            *minPathLength = pathLength;
            for (int i = 0; i <= pathLength; ++i)
            {
                minPath[i] = path[i];
            }
        }
        return;
    }

    // Try moving in all possible directions, checking for walls (NSEW)
    if (mazeGrid[x][y].northWall == 0 && mazeGrid[x][y].visited != 2)
    { // No north wall
        // Move north
        findShortestPath(x, y - 1, end_x, end_y, path, pathLength + 1, minPathLength, minPath);
    }
    if (mazeGrid[x][y].southWall == 0 && mazeGrid[x][y].visited != 2)
    { // No south wall
        // Move south
        findShortestPath(x, y + 1, end_x, end_y, path, pathLength + 1, minPathLength, minPath);
    }
    if (mazeGrid[x][y].eastWall == 0 && mazeGrid[x][y].visited != 2)
    { // No east wall
        // Move east
        findShortestPath(x + 1, y, end_x, end_y, path, pathLength + 1, minPathLength, minPath);
    }
    if (mazeGrid[x][y].westWall == 0 && mazeGrid[x][y].visited != 2)
    { // No west wall
        // Move west
        findShortestPath(x - 1, y, end_x, end_y, path, pathLength + 1, minPathLength, minPath);
    }

    // Backtrack: Mark the current point as unvisited when exploring other paths
    mazeGrid[x][y].nav_visited = 0;
}

int main()
{
    // Initialize path variables
    struct Point path[MAZE_WIDTH * MAZE_HEIGHT];
    int pathLength = 0;

    // Initialize variables to store the minimum path
    int minPathLength = MAZE_WIDTH * MAZE_HEIGHT + 1; // Initialize to a value greater than the maximum possible path length
    struct Point minPath[MAZE_WIDTH * MAZE_HEIGHT];

    // Find the shortest path using DFS
    findShortestPath(STARTING_X, STARTING_Y, ENDING_X, ENDING_Y, path, pathLength, &minPathLength, minPath);

    // Print the shortest path
    printf("Shortest Path: ");
    printPath(minPath, minPathLength);

    return 0;
}