#pragma once

#include "maps/config.h"

bool initMap();
Vector2 loadMapScreen(MapLocation location);

/* Four blocked 8 x 8 quadrants, ordered NW, NE, SW, SE (bits 0-3). */
unsigned char getMapTileCollisionMask(unsigned char tileId);
unsigned char getMapTileSwordCollisionMask(unsigned char tileId);

bool mapIsBlocked(Rectangle location, bool isSword);

void updateMap();
void drawMap();
