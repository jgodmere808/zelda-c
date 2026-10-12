#pragma once

#include "maps/config.h"

typedef enum {
    MAP_STATE_NONE,

    MAP_STATE_ENTERING,
    MAP_STATE_EXITING,

    MAP_STATE_TRANSITIONING_LEFT,
    MAP_STATE_TRANSITIONING_RIGHT,
    MAP_STATE_TRANSITIONING_UP,
    MAP_STATE_TRANSITIONING_DOWN
} MapState;

typedef struct {
    MapState state;
    float enteringProgress;
    float exitingProgress;
    Vector2 pos;
} MapData;

extern MapData mapData;

bool initMap();
Vector2 loadMap(MapLocation location);

bool mapIsBlocked(Rectangle location);

MapLocation getMapChangeEvent(Rectangle feet, Vector2 velocity);

void updateMap();
void drawMap();
