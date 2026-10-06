
#include "menu.h"

typedef enum {
    TITLE_SCREEN,
    SELECT_WORLD,
    REGISTER_MODE,
    ELIMINATION_MODE
} MenuScreen;

typedef struct {
    MenuScreen currentScreen;
} Menu;

static Menu menu;