#pragma once

#include "config.h"
#include "texture_map.h"
#include "audio.h"

#define MAP_SHIFT_DOWN (56 * FACTOR)

#define MAP_BOTTOM_OVERHANG (8 * FACTOR)

typedef enum {
    MAP_TRANSITION_LEFT,
    MAP_TRANSITION_RIGHT,
    MAP_TRANSITION_UP,
    MAP_TRANSITION_DOWN
} MapTransition;

typedef enum {
    SPAWN_GAME_START
} SpawnLocation;

bool initMap();
void loadMapScreen(SpawnLocation spawnLocation);
void transitionMap(MapTransition mapTransition);
void updateMap();
void drawMap();