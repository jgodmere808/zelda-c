#pragma once

#include "config.h"
#include "texture_map.h"
#include "audio.h"

#define MAP_TOP 56
#define MAP_VISIBLE_HEIGHT (GAME_HEIGHT - MAP_TOP)
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

/* Four blocked 8 x 8 quadrants, ordered NW, NE, SW, SE (bits 0-3). */
unsigned char getMapTileCollisionMask(unsigned char tileId);
unsigned char getMapTileSwordCollisionMask(unsigned char tileId);

bool mapIsBlocked(Rectangle location, bool isSword);

bool beginMapTransition(MapTransition mapTransition, Rectangle destinationFeet);
void finishMapTransition();
void drawMapTransition(float progress, MapTransition mapTransition);

void updateMap();
void drawMap();
