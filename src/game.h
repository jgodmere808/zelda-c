#pragma once

#include "config.h"

typedef struct {
    char name[9];
} GameState;

extern GameState gameState;

void resetGameState();
void initGame();
void updateGame();
void drawGame();
