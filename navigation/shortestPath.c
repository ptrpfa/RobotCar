#include <string.h>
#include "database.h"

// Function declarations
void initAlgorithim(Database *d);
void recurToDest(Grid *start, Grid *end, Grid *comeFrom, int recursiveCall);
bool pushArray(Grid *array[], Grid *grid);
void popArray(Grid *array[]);
bool isLargerthan(Grid *array1[], Grid *array2[]);

Grid *tempPath[MAXGRID] = { NULL };
Database *database;

// Init an array of grid struct to all NULL
void initAlgorithim(Database *d) {
    database = d;
}

// Function to get an array of shortest path
void recurToDest(Grid *start, Grid *end, Grid *comeFrom, int recursiveCall) {
    // If current grid is not end, continue searching
    if (start != end) {
        // Check non-null neighbour grids, max amount of linked grid is 4 (since NSEW)
        for (int i = 0; i < NSEW; i++) {
            // If either N,S,E or W is has a linked grid AND the linked grid is not a grid we just come from (this can happen because it is dependent on the order of NSEW)
            if (start->neighbouringGrids[i] != NULL && start->neighbouringGrids[i] != comeFrom) {
                // If grid has been visited before or it is the destination
                if (start->neighbouringGrids[i]->visited == true || start->neighbouringGrids[i] == end) {
                    // Append this grid to the tempPath array, if unable to append exit from this cycle
                    if(!pushArray(tempPath, start)) {
                        return; 
                    }

                    // Call recursive function with start as the new curr
                    recurToDest(start->neighbouringGrids[i], end, start, recursiveCall+1);
                    
                    popArray(tempPath);
                }
            }
        }
        
        // If recursiveCall is 0, all paths traversed
        if(recursiveCall == 0) {
            // If path exist set pathfinding to true
            if(database->Path[0] != NULL) {
                database->isPathfinding = true;
            }

            // Reset tempPath path array
            memset(tempPath, 0, sizeof(tempPath));
        }

        return;
    }
    // Else if path found
    else {
        // Append the ending grid to the tempPath array
        pushArray(tempPath, end);

        // If path is empty or tempPath is shorter then path
        if(database->Path[0] == NULL || isLargerthan(database->Path, tempPath)) {
            // Replace path with the shorter one  
            memcpy(database->Path, tempPath, sizeof(tempPath));
        }

        popArray(tempPath);
        
        // Return to previous reurToDest function call
        return;
    }
}

// Function append grid addresses to an array
bool pushArray(Grid *array[], Grid *grid) {
    int counter = 0;

    while (array[counter] != NULL) {
        if(array[counter] == grid)
            return false;
        counter++;
    }

    array[counter] = grid;
    
    return true;
}

// Function to remove grid address from the top of grid address array
void popArray(Grid *array[]) {
    int counter = 0;

    do {
        counter++;
    }
    while (array[counter] != NULL);

    array[counter-1] = NULL;

    return;
}

// Function to compare two array lengths
bool isLargerthan(Grid *array1[], Grid *array2[]) {
    int length1 = 0;
    int length2 = 0;

    while(array1[length1] != NULL) {
        length1++;
    }

    while(array2[length2] != NULL) {
        length2++;
    }

    return length1 > length2;
}