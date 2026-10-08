#pragma once

#include "config.h"
#include "map.h"
#include "link.h"

typedef struct {
    char name[9];
} GameData;

extern GameData gameData;

void resetGameData();
void initGame();
void updateGame();
void drawGame();
