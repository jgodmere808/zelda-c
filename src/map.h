#pragma once

#include "maps/config.h"

typedef struct {
    MapLocation origin;
    MapLocation destination;
} MapChangeEvent;

bool initMap();
Vector2 loadMap(MapLocation location);

bool mapIsBlocked(Rectangle location);

MapChangeEvent getMapChangeEvent(Rectangle feet, Vector2 velocity);

void updateMap();
void drawMap();
