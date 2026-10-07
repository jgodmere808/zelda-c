#pragma once

#include "../config.h"

#define ELIMINATION_MODE_SLOT_COUNT 3
#define ELIMINATION_MODE_NAME_LENGTH 8

typedef struct {
    char names[ELIMINATION_MODE_SLOT_COUNT][ELIMINATION_MODE_NAME_LENGTH + 1];
    bool markedForDeletion[ELIMINATION_MODE_SLOT_COUNT];
    int selectedEntry; /* 0-2 are files; 3 is ELIMINATION END. */
    bool saveFailed;
} EliminationModeScreen;

EliminationModeScreen initEliminationModeScreen(void);
void refreshEliminationModeScreen(EliminationModeScreen *screen);
void updateEliminationModeScreen(EliminationModeScreen *screen);
void drawEliminationModeScreen(const EliminationModeScreen *screen);

bool eliminationModeSlotMarked(const EliminationModeScreen *screen, int slot);
