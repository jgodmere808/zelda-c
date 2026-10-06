
#include "menu.h"

typedef enum {
    TITLE_SCREEN,
    SELECT_WORLD,
    REGISTER_MODE,
    ELIMINATION_MODE
} MenuScreen;

typedef struct {
    MenuScreen currentScreen;
    TitleScreen titleScreen;
} Menu;

static Menu menu;

void initMenu()
{
    menu.currentScreen = TITLE_SCREEN;
    menu.titleScreen = initTitleScreen();
}

void updateAndDrawMenu()
{
    switch (menu.currentScreen) {
        case TITLE_SCREEN:
            updateTitleScreen(&menu.titleScreen);
            drawTitleScreen(&menu.titleScreen);
            break;
    }
}