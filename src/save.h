#pragma once

#include "config.h"

typedef enum {
    SAVE_REGISTER_1,
    SAVE_REGISTER_2,
    SAVE_REGISTER_3
} SaveRegister;

bool loadSave(SaveRegister saveRegister, GameData *gameData);
bool storeSave(SaveRegister saveRegister, GameData *gameData);
bool deleteSave(SaveRegister saveRegister);
