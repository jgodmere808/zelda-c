#pragma once

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#define FACTOR 3

#define GAME_WIDTH  256
#define GAME_HEIGHT 224

#define SCREEN_WIDTH  (GAME_WIDTH * FACTOR)
#define SCREEN_HEIGHT (GAME_HEIGHT * FACTOR)

typedef struct {
    char name[9];
    unsigned char totalHearts; // each half heart is 1 health
    int currentHealth; // divide by 2 to get health
    
    // for cave_x07_y07 sword
    bool collectedSword1;
} GameData;

extern GameData gameData;