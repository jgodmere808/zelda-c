#pragma once

#include "../config.h"
#include "../texture_map.h"
#include "../menu.h"

typedef struct {
    Color glowColors[8];
    int waveY[3];
    int frame;
    int glowPhase;
    int glowTimer;
    float frameAccumulator;
} TitleScreen;

TitleScreen initTitleScreen();
void updateTitleScreen(TitleScreen *titleScreen);
void drawTitleScreen(TitleScreen *titleScreen);