#pragma once

#include "../config.h"

#define SELECT_WORLD_SLOT_COUNT 3
#define SELECT_WORLD_NAME_LENGTH 8

typedef enum {
    SELECT_WORLD_GREEN,
    SELECT_WORLD_BLUE,
    SELECT_WORLD_RED
} SelectWorldRingColor;

typedef struct {
    char names[SELECT_WORLD_SLOT_COUNT][SELECT_WORLD_NAME_LENGTH + 1];
    int selectedEntry; /* 0-2 are files, 3 is Register, 4 is Elimination. */
    int chosenSlot;    /* Last file started, or -1. */
    int heartContainers[SELECT_WORLD_SLOT_COUNT];
    int filledHalfHearts[SELECT_WORLD_SLOT_COUNT];
    SelectWorldRingColor ringColors[SELECT_WORLD_SLOT_COUNT];
} SelectWorldScreen;

SelectWorldScreen initSelectWorldScreen(void);
void refreshSelectWorldScreen(SelectWorldScreen *screen);
void updateSelectWorldScreen(SelectWorldScreen *screen);
void drawSelectWorldScreen(const SelectWorldScreen *screen);

const char *getSelectWorldName(const SelectWorldScreen *screen, int slot);
int getSelectedWorldSlot(const SelectWorldScreen *screen);
bool setSelectWorldSlotAppearance(SelectWorldScreen *screen, int slot,
                                  SelectWorldRingColor ringColor,
                                  int heartContainers, int filledHalfHearts);
