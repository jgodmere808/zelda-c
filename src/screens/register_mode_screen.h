#pragma once

#include "../config.h"

#define REGISTER_MODE_SLOT_COUNT 3
#define REGISTER_MODE_NAME_LENGTH 8

typedef struct {
    char names[REGISTER_MODE_SLOT_COUNT][REGISTER_MODE_NAME_LENGTH + 1];
    int nameLengths[REGISTER_MODE_SLOT_COUNT];
    int namePositions[REGISTER_MODE_SLOT_COUNT];
    int selectedSlot; /* 0-2 are files; 3 is REGISTER END. */
    int keyboardRow;
    int keyboardColumn;
    bool saveFailed;
    Texture2D background;
    Texture2D font;
    Texture2D link;
    Texture2D redHeart;
    Texture2D redSelection;
} RegisterModeScreen;

RegisterModeScreen initRegisterModeScreen(void);
void updateRegisterModeScreen(RegisterModeScreen *screen);
void drawRegisterModeScreen(const RegisterModeScreen *screen);
void unloadRegisterModeScreen(RegisterModeScreen *screen);

const char *getRegisteredName(const RegisterModeScreen *screen, int slot);
bool setRegisteredName(RegisterModeScreen *screen, int slot, const char *name);
bool saveRegisteredNames(const RegisterModeScreen *screen);
bool registeredNameStartsSecondQuest(const RegisterModeScreen *screen, int slot);
