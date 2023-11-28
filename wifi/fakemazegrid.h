/* file: my_constants.h */

#ifndef FAKEMAZEGRID_H
#define FAKEMAZEGRID_H

// Struct for maze cell
struct Cell
{
    int northWall; // -1 - unvisited, 0 - empty, 1 - wall
    int southWall; // -1 - unvisited, 0 - empty, 1 - wall
    int eastWall;  // -1 - unvisited, 0 - empty, 1 - wall
    int westWall;  // -1 - unvisited, 0 - empty, 1 - wall
    int visited;   // 0 - unvisited, 1 - visited, 2 - obstacle, 3 - barcode
};

// Declare the external constant array of structs
extern const struct Cell mazeGrid[][6];

#endif
