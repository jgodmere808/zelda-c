#pragma once

#include "config.h"
#include "map.h"
#include "link.h"

typedef struct {
    char name[9];
    unsigned char totalHearts; // each half heart is 1 health
    int currentHealth; // divide by 2 to get health
    
    // for cave_x07_y07 sword
    bool collectedSword1;
} GameData;

extern GameData gameData;

void resetGameData();
void initGame();
void updateGame();
void drawGame();
