#pragma once

#include "config.h"
#include "texture_map.h"
#include "audio.h"

#include "maps/overworld.h"
#include "maps/cave_x07_y07.h"

typedef enum {
    SPAWN_GAME_START
} SpawnLocation;

bool initMap();
void loadMapScreen(SpawnLocation spawnLocation);

/* Four blocked 8 x 8 quadrants, ordered NW, NE, SW, SE (bits 0-3). */
unsigned char getMapTileCollisionMask(unsigned char tileId);
unsigned char getMapTileSwordCollisionMask(unsigned char tileId);

bool mapIsBlocked(Rectangle location, bool isSword);

bool beginMapTransition(MapTransition mapTransition, Rectangle destinationFeet);
void finishMapTransition();
void drawMapTransition(float progress, MapTransition mapTransition);

void updateMap();
void drawMap();
