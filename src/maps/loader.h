#pragma once

#include "config.h"

bool loaderLoadScreen(FILE *file, MapScreen *mapScreen, Item *items,
                      int *itemCount, int mapRow, int mapCol);
