// Mapping and Navigation
// switch pathfining algo with the following define
// define "SHORTEST_PATH" to run the shortest path algo
// remove the define to run the first path found algo

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "database.h"

#define SHORTEST_PATH

#ifdef SHORTEST_PATH
#include "shortestPath.c"
#else
#include "firstPathFound.c"
#endif

// Finite state machine states for maping
typedef enum FINITE_STATE
{
    FS_CHECKING_GRID = 0,   // performs checks on current grid and decides next course of action
    FS_GOING_STRAIGHT,      // when the car is moving straight
    FS_TURN_RIGHT,          // when the car is turing right
    FS_TURN_LEFT,           // when the car is turing left
    FS_TURN_AROUND,         // when the car is doing a 180 degree turn
    FS_PATHFINDING,         // state where the car navigates to a selected grid
    FS_FINISH_MAPPING       // ending state when the entire map is maped out

} FINITE_STATE;

// corresponding string name for the DIRECTION enum found in database
const char *direction_str[NSEW] = {"North", "East", "South", "West"};

// Function Prototypes
void CheckPosibleMoves(int L, int R, int F, Database *database);
void LinkCurrentGrid(int facingOffset, Database *database);
Grid *GetGrid(int x, int y, Database *database);
Grid *GetNextAvailableGrid(int nextX, int nextY, Database *database);
void SetCoordinate(int *x, int *y, DIRECTION facing);
int FindTurningIndex(Grid *grid, Database *database);
void UpdateMMList(int facingOffset, Database *database);
void RemoveFromMMList(Grid *MMgrid, Database *database);

unsigned char mygetchar() { 
     int c; 
     while ( (c = getchar_timeout_us(0)) < 0);  
     return (unsigned char)c; 
};

int main() {
    // Init all standard I/O
    stdio_init_all();

    // // bink led lights to signify start of excecution
    // PicoStartUpTest();

    // FSM current state
    FINITE_STATE curState = FS_CHECKING_GRID;
    // flag to toggle when finish mapping
    bool finishMapping = false;

    // shared database for all modules to use
    Database database;
    memset(&database, 0, sizeof(Database));

    // pass database address to path finding algo
    initAlgorithim(&database);

    // set the current grid to slot 0 of the grid list
    database.currentGrid = &(database.gridList[0]);
    database.currentGrid->visited = true;
    
    mygetchar();
    
    // Main loop, will keep running
    while (1)
    {
        // Final State Machine (FSM)
        switch (curState)
        {

        case FS_GOING_STRAIGHT:
        {
            printf("\n<GOING_STRAIGHT State>\n");
            unsigned char userInput = 0;
            do
            {
                printf("Finish moving straight? y/n: ");
                userInput = mygetchar();
                printf("%c\n", userInput);
            } while (userInput != 'y');

            // update current grid to next grid based on the direction of car
            database.currentGrid = database.currentGrid->neighbouringGrids[database.isFacing];
            database.currentGrid->visited = true;
            curState = FS_CHECKING_GRID;
        }
        break;

        case FS_TURN_LEFT:
        {
            printf("\n<TURN_LEFT State>\n");
            database.isFacing = WrapNSEW(database.isFacing + LEFT);

            unsigned char userInput = 0;
            do
            {
                printf("Finish turning left? y/n: ");
                userInput = mygetchar();
                printf("%c\n", userInput);
            } while (userInput != 'y');

            curState = FS_GOING_STRAIGHT;
        }
        break;

        case FS_TURN_RIGHT:
        {
            printf("\n<TURN_RIGHT State>\n");
            database.isFacing = WrapNSEW(database.isFacing + RIGHT);

            unsigned char userInput = 0;
            do
            {
                printf("Finish turning right? y/n: ");
                userInput = mygetchar();
                printf("%c\n", userInput);
            } while (userInput != 'y');

            curState = FS_GOING_STRAIGHT;
        }
        break;

        case FS_TURN_AROUND:
        {
            printf("\n<TURN_AROUND State>\n");
            database.isFacing = WrapNSEW(database.isFacing + BEHIND);

            unsigned char userInput = 0;
            do
            {
                printf("Finish 180 turn? y/n: ");
                userInput = mygetchar();
                printf("%c\n", userInput);
            } while (userInput != 'y');

            if (database.isPathfinding == true)
                curState = FS_GOING_STRAIGHT;
            else
                curState = FS_CHECKING_GRID;
        }
        break;

        case FS_CHECKING_GRID:
        {
            printf("\n<CHECKING_GRID State>\n");
            
            printf("\nCurrent gridList array slot: %d\n", database.gridCounter);
            printf("Current position x: %d, y: %d\n", database.currentGrid->x, database.currentGrid->y);
            printf("Currently facing: %s\n", direction_str[database.isFacing]);

            if (database.isPathfinding)
            {
                curState = FS_PATHFINDING;
                continue;
            }
            else if (finishMapping)
            {
                curState = FS_FINISH_MAPPING;
                continue;
            }

            // Get all 3 Ultrasonic's Current Distance (L,R,F)

            // Simulate Ultrasonic Sensors by geting input with uart
            unsigned char userInputF = 0, userInputL = 0, userInputR = 0;
            do
            {
                printf("Can move %s? y/n: ", direction_str[database.isFacing]);
                userInputF = mygetchar();
                printf("%c\n", userInputF);
            } while (userInputF != 'y' && userInputF != 'n');

            do
            {
                printf("Can move %s? y/n: ", direction_str[WrapNSEW(database.isFacing + LEFT)]);
                userInputL = mygetchar();
                printf("%c\n", userInputL);
            } while (userInputL != 'y' && userInputL != 'n');

            do
            {
                printf("Can move %s? y/n: ", direction_str[WrapNSEW(database.isFacing + RIGHT)]);
                userInputR = mygetchar();
                printf("%c\n", userInputR);
            } while (userInputR != 'y' && userInputR != 'n');
            
            // based on ultrasonic input, update the current grid's details and other maping infomation
            CheckPosibleMoves(
                userInputF == 'y' ? 200 : 0,
                userInputL == 'y' ? 200 : 0,
                userInputR == 'y' ? 200 : 0,
                &database);
            
            // after updating data above
            // check which direction is possible. Check Right, Left then Front.
            // check current grid's right side neighbour (isfacing+1) is not NULL (has a grid address) and that it is not yet visited
            if (database.currentGrid->neighbouringGrids[WrapNSEW(database.isFacing + RIGHT)] != NULL && !database.currentGrid->neighbouringGrids[WrapNSEW(database.isFacing + RIGHT)]->visited)
            {
                UpdateMMList(RIGHT, &database);
                curState = FS_TURN_RIGHT;
            }
            // check current grid's left side neighbour (isfacing-1) is not NULL (has a grid address) and that it is not yet visited
            else if (database.currentGrid->neighbouringGrids[WrapNSEW(database.isFacing + LEFT)] != NULL && !database.currentGrid->neighbouringGrids[WrapNSEW(database.isFacing + LEFT)]->visited)
            {
                UpdateMMList(LEFT, &database);
                curState = FS_TURN_LEFT;
            }
            // check current grid's front neighbour is not NULL (has a grid address) and that it is not yet visited
            else if (database.currentGrid->neighbouringGrids[(database.isFacing)] != NULL && !database.currentGrid->neighbouringGrids[(database.isFacing)]->visited)
            {
                curState = FS_GOING_STRAIGHT;
            }
            else // Cannot move forward. Left, Right and Front no possible move.
            {
                // if able back is of current grid is null, turn 180 on the spot to perform ultrasonic check the time it enter this state
                if (database.currentGrid->neighbouringGrids[WrapNSEW(database.isFacing + BEHIND)] == NULL)
                {
                    curState = FS_TURN_AROUND;
                    continue;
                }

                // When neighbors are already visited, start Dijkstra to navigate Multi-Move
                // Currently is only recursive path finding.

                // look for a multi move grid to go to
                Grid *toMoveto = NULL;
                for (int i = 0; i < MAXQUEUE; i++)
                {
                    if (database.MMList[i] != NULL && database.MMList[i] != database.currentGrid)
                    {
                        toMoveto = database.MMList[i];
                        break;
                    }
                }

                // if there is no multi move grid left, maping is complete
                if (toMoveto == NULL)
                {
                    printf("\nMM List is empty. Completed Mapping.\n");
                    finishMapping = true;
                    continue;
                }

                // perform path finding search to the multi move grid
                #ifdef SHORTEST_PATH
                recurToDest(database.currentGrid, toMoveto, database.currentGrid, 0);
                #else
                recurToDest(database.currentGrid, toMoveto, database.currentGrid);
                #endif

            }
        }
        break;

        case FS_PATHFINDING:
        {
            printf("\n<PATHFINDING State>\n");

            // counter to track the next grid to move to in path array
            // starts from 1 because index 0 is the current grid
            static int i = 1;
            
            if (database.Path[i] == NULL)
            {
                // We have travelled at the end of the path.
                
                // clean up / reset data and go back to check grid state
                RemoveFromMMList(database.Path[i - 1], &database);
                database.isPathfinding = false;
                memset(&database.Path, 0, sizeof(database.Path));
                i = 1;

                curState = FS_CHECKING_GRID;
            }
            else
            {
                // print path
                printf("\nRemainding path to take: ");
                for(int j = i; j < MAXGRID; ++j)
                {   
                    if(database.Path[j] == NULL)
                        break;
                    printf("[x:%d, y:%d]->", database.Path[j]->x, database.Path[j]->y);
                }
                printf("DONE\n");

                // Based on the car's current direction, find out the direction to turn to move to the next grid
                switch (FindTurningIndex(database.Path[i++], &database))
                {
                case FRONT:
                    curState = FS_GOING_STRAIGHT;
                    break;
                case RIGHT:
                    curState = FS_TURN_RIGHT;
                    break;
                case LEFT:
                    curState = FS_TURN_LEFT;
                    break;
                case BEHIND:
                    curState = FS_TURN_AROUND;
                    break;
                }
            }
        }
        break;

        case FS_FINISH_MAPPING:
        {
            printf("\n<FINISH_MAPPING State>\n");
            printf("Enter navigation coordinates\n");

            int pathFindingX = 1, pathFindingY = 1;
            unsigned char userInput = 0;

            // finish maping, get x, y input for going to another grid
            printf("Enter coordinates for X: ");
            userInput = mygetchar();
            if(userInput == '-')
            {
                printf("%c", userInput);
                pathFindingX = -1;
                userInput = mygetchar();
            }
            printf("%c", userInput);
            pathFindingX *= (userInput - '0');

            printf("\nEnter coordinates for Y: ");
            userInput = mygetchar();
            if(userInput == '-')
            {
                printf("%c", userInput);
                userInput = mygetchar();
                pathFindingY = -1;
            }
            printf("%c", userInput);
            pathFindingY *= (userInput - '0');

            // get grid based on input 
            Grid *pFinder = GetGrid(pathFindingX, pathFindingY, &database);
            // if grid exist and is not current grid, start path fining
            if (pFinder != NULL && database.currentGrid != pFinder)
            {
                // perform path finding search selected grid
                #ifdef SHORTEST_PATH
                recurToDest(database.currentGrid, pFinder, database.currentGrid, 0);
                #else
                recurToDest(database.currentGrid, pFinder, database.currentGrid);
                #endif

                curState = FS_CHECKING_GRID;
            }
            else
            {
                printf("\nCannot move to location\n");
            }
        }
        break;

        }
    }
}


// Based on ultra sensors inputs, update the current grid and map acordingly. F, L, R are the front left and right sensor values
void CheckPosibleMoves(int F, int L, int R, Database *database) {
    // for each of the sensors value's are above a treshold, it means that there is a posible path in that direction. link the current grid with it
    if (F > 100) {
        LinkCurrentGrid(FRONT, database);
    }
    if (L > 100) {
        LinkCurrentGrid(LEFT, database);
    }
    if (R > 100) {
        LinkCurrentGrid(RIGHT, database);
    }
}

// Link current grid and its neighbour together, also set other maping info such as adding to multi move array facing
// Offset offset of the neighbouring grid based on the current facing direction. -1 for left, 0 for front, 1 for right
void LinkCurrentGrid(int facingOffset, Database *database) {
    // Get current car facing direction and shift it to direction of "empty" grid
    // Modulus 4(NSEW) is to determine new direction (enum roll over).
    DIRECTION tempFacing = WrapNSEW(database->isFacing + facingOffset);

    // if grid is already linked, return
    if (database->currentGrid->neighbouringGrids[tempFacing] != NULL) {
        return;
    }

    // Get position of the current grid
    int tempX = database->currentGrid->x;
    int tempY = database->currentGrid->y;

    // Offset the position based on which ever direction of the "empty" grid
    SetCoordinate(&tempX, &tempY, tempFacing);

    // GRID CHECKER
    // Assuming that there is only a maximum of 5 in the MM List.
    for (int i = 0; i < MAXQUEUE; i++) {
        // Check if this current grid is part of the MM list.
        if (database->MMList[i] != NULL && database->MMList[i]->x == tempX && database->MMList[i]->y == tempY) {
            // The grid has already been linked as it is a MM grid
            // Put neighbour grid's address to current grid
            database->currentGrid->neighbouringGrids[tempFacing] = database->MMList[i];
            database->MMList[i]->neighbouringGrids[WrapNSEW(tempFacing + BEHIND)] = database->currentGrid;

            database->MMList[i] = 0;

            return;
        }
    }

    // Change counter to the next neighbouring grid
    Grid *nextGrid = GetNextAvailableGrid(tempX, tempY, database);
    // Put neighbour grid's address to current grid
    database->currentGrid->neighbouringGrids[tempFacing] = nextGrid;
    // Put the neighbouring grid to the current grid
    nextGrid->neighbouringGrids[WrapNSEW(tempFacing + BEHIND)] = database->currentGrid;

    // Set the x and y of the neighbouring grid
    nextGrid->x = tempX;
    nextGrid->y = tempY;
}

// offset x, y position based on given direction. north/south will ofset y axis. east west will offset x axis
void SetCoordinate(int *x, int *y, DIRECTION facing) {
    if (facing == NORTH) {
        (*y) += 1;
    }
    else if (facing == SOUTH) {
        (*y) -= 1;
    }
    else if (facing == EAST) {
        (*x) += 1;
    }
    else if (facing == WEST) {
        (*x) -= 1;
    }
}

// get address of an existing grid by x and y coordinates 
Grid *GetGrid(int x, int y, Database *database) {
    for (int i = 0; i <= database->gridCounter; ++i) {
        if (x == database->gridList[i].x && y == database->gridList[i].y)
            return &database->gridList[i];
    }
    return NULL;
}

// get address of an existing grid by x and y coordinates. If not found, return a new grid
Grid *GetNextAvailableGrid(int nextX, int nextY, Database *database) {
    for (int i = 0; i <= database->gridCounter; ++i) {
        if (nextX == database->gridList[i].x && nextY == database->gridList[i].y)
            return &database->gridList[i];
    }

    database->gridCounter += 1;

    if (database->gridCounter >= MAXGRID) {
        printf("\n< ERROR! RAN OUT OF GRIDS >\n");
        exit(404);
    }

    return &database->gridList[database->gridCounter];
}

// based on the car's current grid and direction, find out the offset direction towards the selected grid the grid to compare with
int FindTurningIndex(Grid *grid, Database *database) {
    int turningIndex = 0;

    if (grid->x < database->currentGrid->x)
        turningIndex = (WEST - database->isFacing);
    else if (grid->x > database->currentGrid->x)
        turningIndex = (EAST - database->isFacing);
    else if (grid->y < database->currentGrid->y)
        turningIndex = (SOUTH - database->isFacing);
    else
        turningIndex = (NORTH - database->isFacing);

    if (turningIndex == 3)
        turningIndex = LEFT;
    else if (turningIndex == -2)
        turningIndex = BEHIND;
    else if (turningIndex == -3)
        turningIndex = RIGHT;

    return turningIndex;
}

// Called before car is about to move to next grid. adds any other posible neighbour grids that has not been visited to multi move list.
// facingOffset offset of the grid the car is turning towards based on the current facing direction. -1 for left, 0 for front, 1 for right
void UpdateMMList(int facingOffset, Database *database) {
    // for each neighbour grid
    for (int i = 0; i < NSEW; i++)
    {
        // if neighbour grid is not null
        if (database->currentGrid->neighbouringGrids[i] != NULL
        // and that neighbour grid is not the grid that the car is going to travel to
        && database->currentGrid->neighbouringGrids[i] != database->currentGrid->neighbouringGrids[WrapNSEW(database->isFacing + facingOffset)]
        // and that neighbour grid is not visited
        && !database->currentGrid->neighbouringGrids[i]->visited)
        {
            // push this grid to the MM List
            for (int j = 0; j < MAXQUEUE; j++)
            {
                if (database->MMList[j] == NULL)
                {
                    database->MMList[j] = database->currentGrid->neighbouringGrids[i];
                    break;
                }
            }
        }
    }
}

// Removes a grid from the multi move list
void RemoveFromMMList(Grid *MMgrid, Database *database) {
    for (int i = 0; i < MAXQUEUE; ++i)
    {
        // After it has moved to the location of the MM List, remove it.
        if (MMgrid == database->MMList[i])
        {
            database->MMList[i] = NULL;
        }
    }
}