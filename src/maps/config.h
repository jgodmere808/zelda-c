#pragma once

#include "../config.h"

#define MAP_TOP 56
#define MAP_VISIBLE_HEIGHT (GAME_HEIGHT - MAP_TOP)

#define MAP_SCREEN_ROWS 11
#define MAP_SCREEN_COLS 16
#define MAP_ATLAS_TILE_COUNT (18 * 8)

enum {
    TILE_OPEN = 0,
    TILE_BLOCK_NW = 1,
    TILE_BLOCK_NE = 2,
    TILE_BLOCK_SW = 4,
    TILE_BLOCK_SE = 8,
    TILE_BLOCK_LEFT = TILE_BLOCK_NW | TILE_BLOCK_SW,
    TILE_BLOCK_RIGHT = TILE_BLOCK_NE | TILE_BLOCK_SE,
    TILE_BLOCK_TOP = TILE_BLOCK_NW | TILE_BLOCK_NE,
    TILE_BLOCK_BOTTOM = TILE_BLOCK_SW | TILE_BLOCK_SE,
    TILE_SOLID = TILE_BLOCK_TOP | TILE_BLOCK_BOTTOM
};

typedef enum {
    // Overworld Locations
    MAP_LOCATION_OVERWORLD_SPAWN,
    MAP_LOCATION_OVERWORLD_CAVE_x07_y07,

    // Cave Locations
    MAP_LOCATION_CAVE_x07_y07,

    MAP_LOCATION_NONE
} MapLocation;

typedef unsigned char MapScreen[MAP_SCREEN_ROWS][MAP_SCREEN_COLS];

typedef struct {
    MapScreen *screen;
    Vector2 pos;
} MapLocationData;
