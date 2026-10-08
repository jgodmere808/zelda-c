#pragma once

#include "config.h"
#include "texture_map.h"
#include "audio.h"

typedef enum {
    SPAWN_GAME_START
} SpawnLocation;

bool initMap();
void loadMapScreen(SpawnLocation spawnLocation);
void transitionMap();
void updateMap();
void drawMap();