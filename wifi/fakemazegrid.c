#include "fakemazegrid.h"

const struct Cell mazeGrid[][6] = {
    // (0, 0) to (0, 5)
    {
        // N, S, E, W, Visited
        {1, 0, 1, 1, 1}, // Array index: (0, 0)
        {0, 0, 0, 1, 1}, // Array index: (0, 1)
        {0, 0, 1, 1, 3}, // Array index: (0, 2)
        {0, 0, 1, 1, 1}, // Array index: (0, 3)
        {0, 0, 0, 1, 1}, // Array index: (0, 4)
        {0, 1, 1, 1, 1}, // Array index: (0, 5)
    },
    // (1, 0) to (1, 5)
    {
        // N, S, E, W, Visited
        {0, 0, 0, 1, 1}, // Array index: (1, 0)
        {0, 1, 1, 0, 1}, // Array index: (1, 1)
        {1, 0, 1, 1, 1}, // Array index: (1, 2)
        {0, 0, 0, 1, 1}, // Array index: (1, 3)
        {0, 1, 1, 0, 1}, // Array index: (1, 4)
        {1, 1, 0, 1, 1}, // Array index: (1, 5)
    },
    // (2, 0) to (2, 5)
    {
        // N, S, E, W, Visited
        {1, 0, 1, 0, 1}, // Array index: (2, 0)
        {0, 0, 0, 1, 1}, // Array index: (2, 1)
        {0, 0, 1, 1, 1}, // Array index: (2, 2)
        {0, 1, 0, 0, 1}, // Array index: (2, 3)
        {1, 0, 0, 1, 1}, // Array index: (2, 4)
        {0, 0, 1, 0, 1}, // Array index: (2, 5)
    },
    // (3, 0) to (3, 5)
    {
        // N, S, E, W, Visited
        {1, 0, 1, 1, 1}, // Array index: (3, 0)
        {0, 0, 1, 0, 1}, // Array index: (3, 1)
        {0, 1, 1, 1, 1}, // Array index: (3, 2)
        {1, 0, 1, 0, 1}, // Array index: (3, 3)
        {0, 0, 1, 0, 1}, // Array index: (3, 4)
        {0, 1, 1, 1, 1}, // Array index: (3, 5)
    },

};
