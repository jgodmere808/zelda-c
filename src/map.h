#pragma once

#include "maps/config.h"

bool initMap();
Vector2 loadMap(MapLocation location);

bool mapIsBlocked(Rectangle location);

MapLocation getMapChangeEvent(Rectangle feet, Vector2 velocity);

void updateMap();
void drawMap();
