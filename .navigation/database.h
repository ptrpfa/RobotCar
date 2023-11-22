#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

#define MAXGRID    30   // Max number of grids
#define MAXQUEUE    5   // Max number of multi move list elements
#define NSEW        4   // Max number of directions

// Direction definitions
#define FRONT    0
#define LEFT    -1
#define RIGHT    1
#define BEHIND   2

// Enum to keep track of directions
typedef enum DIRECTION {
    NORTH = 0,
    EAST,
    SOUTH,
    WEST
} DIRECTION;

// Struct for Grid
typedef struct Grid Grid;

struct Grid {
    // Coordinates of grid
    short x;
    short y;

    // Addresses of the neighbouringGrids in array. 0 = NORTH grid, 1 = EAST grid, 2 = SOUTH grid, 3 = WEST grid
    Grid *neighbouringGrids[NSEW];

    // Boolean to track visited grids
    bool visited;
};

typedef struct {
    // List of grids
    Grid gridList[MAXGRID];

    // List of grids to return to when there are 2 or more paths to take
    Grid *MMList[MAXQUEUE];
    
    // Path of grids that the path finding algo will generate
    Grid *Path[MAXGRID];

    // Toggling of pathfinding
    bool isPathfinding;

    // Direction car is facing
    DIRECTION isFacing;
    
    // Grid that the car is currently on
    Grid *currentGrid; 
    
    // Count total number of grids discovered
    int gridCounter;
} Database;

// Integer wrapper to access neighbouringGrids.
int WrapNSEW(int result) {
    while (result < 0) {
        result += NSEW;
    }

    if (result >= NSEW) {
        result %= NSEW;
    }

    return result;
}

#endif