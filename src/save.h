#pragma once

#include "config.h"

typedef enum {
    SAVE_REGISTER_1,
    SAVE_REGISTER_2,
    SAVE_REGISTER_3
} SaveRegister;

bool loadSave(SaveRegister saveRegister, GameState *gameState);
bool storeSave(SaveRegister saveRegister, GameState *gameState);
bool deleteSave(SaveRegister saveRegister);