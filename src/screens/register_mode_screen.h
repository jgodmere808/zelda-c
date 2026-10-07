#pragma once

#include "../config.h"

#define REGISTER_MODE_SLOT_COUNT 3

typedef struct {
    int selectedSlot; /* 0-2 are files; 3 is REGISTER END. */
    int keyboardRow;
    int keyboardColumn;
    bool saveFailed;
} RegisterModeScreen;

RegisterModeScreen initRegisterModeScreen(void);
void updateRegisterModeScreen(RegisterModeScreen *screen);
void drawRegisterModeScreen(const RegisterModeScreen *screen);
