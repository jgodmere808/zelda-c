#pragma once

#include "config.h"

bool initMapCavex07y07();

/* Four blocked 8 x 8 quadrants, ordered NW, NE, SW, SE (bits 0-3). */
// unsigned char getMapTileCollisionMask(unsigned char tileId);
// unsigned char getMapTileSwordCollisionMask(unsigned char tileId);

bool isMapCavex07y07SolidAt();

void updateMapCavex07y07();
void drawMapCavex07y07();