
#include "menu.h"

typedef struct {
    MenuScreen currentScreen;
    TitleScreen titleScreen;
    SelectWorldScreen selectWorldScreen;
    RegisterModeScreen registerModeScreen;
    EliminationModeScreen eliminationModeScreen;
} Menu;

static Menu menu;

void initMenu()
{
    menu.currentScreen = TITLE_SCREEN;
    menu.titleScreen = initTitleScreen();
    menu.selectWorldScreen = initSelectWorldScreen();
    menu.registerModeScreen = initRegisterModeScreen();
    menu.eliminationModeScreen = initEliminationModeScreen();
}

MenuScreen getMenu()
{
    return menu.currentScreen;
}

void changeMenu(MenuScreen nextScreen)
{
    if (nextScreen == REGISTER_MODE_SCREEN && menu.currentScreen != REGISTER_MODE_SCREEN) {
        menu.registerModeScreen = initRegisterModeScreen();
    }
    menu.currentScreen = nextScreen;
}

void updateAndDrawMenu()
{
    switch (menu.currentScreen) {
        case TITLE_SCREEN:
            updateTitleScreen(&menu.titleScreen);
            drawTitleScreen(&menu.titleScreen);
            break;
        case SELECT_WORLD_SCREEN:
            updateSelectWorldScreen(&menu.selectWorldScreen);
            drawSelectWorldScreen(&menu.selectWorldScreen);
            break;
        case REGISTER_MODE_SCREEN:
            updateRegisterModeScreen(&menu.registerModeScreen);
            drawRegisterModeScreen(&menu.registerModeScreen);
            break;
        case ELIMINATION_MODE_SCREEN:
            updateEliminationModeScreen(&menu.eliminationModeScreen);
            drawEliminationModeScreen(&menu.eliminationModeScreen);
            break;
        default:
            break;
    }
}
