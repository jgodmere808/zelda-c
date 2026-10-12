#pragma once

#include "maps/config.h"

bool initMap();
Vector2 loadMapScreen(MapLocation location);

bool mapIsBlocked(Rectangle location);

void updateMap();
void drawMap();
