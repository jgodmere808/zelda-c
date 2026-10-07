#pragma once

#include "config.h"

#include "screens/title_screen.h"
#include "screens/select_world_screen.h"
#include "screens/register_mode_screen.h"
#include "screens/elimination_mode_screen.h"

typedef enum {
    TITLE_SCREEN,
    SELECT_WORLD_SCREEN,
    REGISTER_MODE_SCREEN,
    ELIMINATION_MODE_SCREEN,
    GAME_SCREEN
} MenuScreen;

void initMenu();
MenuScreen getMenu();
void changeMenu(MenuScreen nextScreen);
void updateAndDrawMenu();