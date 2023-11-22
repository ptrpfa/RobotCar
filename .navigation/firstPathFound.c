// Traverse to map out the entire maze into a grid

#include <string.h>
#include "database.h"

// Function declarations
void initAlgorithim(Database *d);
void recurToDest(Grid *start, Grid *end, Grid *comeFrom);
bool pushArray(Grid *array[], Grid *grid);
void popArray(Grid *array[]);

Database *database;

// Init an array of grid struct to all NULL
void initAlgorithim(Database *d) {
    database = d;
}

// Function to get an array of first found path
void recurToDest(Grid *start, Grid *end, Grid *comeFrom) {
    // If current grid is not end, continue searching
    if (start != end) {
        // Check which is the non-null neighbouringGrids, max amount of linked grid is 4 (NSEW)
        for (int i = 0; i < NSEW; i++) {
            // If either N, S, E or W has a linked grid AND the linked grid is not a grid we just come from
            if (start->neighbouringGrids[i] != NULL && start->neighbouringGrids[i] != comeFrom) {
                // If grid has been visited before or is the destination
                if (start->neighbouringGrids[i]->visited == true || start->neighbouringGrids[i] == end) {
                    // Add grid to path array
                    if(!pushArray(database->Path, start)) {
                        return;
                    }

                    // Call recursive function with start as the new current
                    recurToDest(start->neighbouringGrids[i], end, start);

                    // If path found, exit recursion
                    if(database->isPathfinding) {
                        return;
                    }

                    // Else, remove path as its now visited
                    else {
                        popArray(database->Path);
                    }
                }
            }
        }
        return;
    }
    // Else, end grid found
    else {
        // Add grid to path array
        pushArray(database->Path, end);
        database->isPathfinding = true;

        // Return to previous recurToDest
        return;
    }
}

// Function append grid addresses to an array
bool pushArray(Grid *array[], Grid *grid) {
    int counter = 0;

    while (array[counter] != NULL) {
        // If grid is already in array, return false
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