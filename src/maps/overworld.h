#pragma once

#include "config.h"

bool initMapOverworld();
MapLocationData loadMapOverworld(MapLocation location);

/* Four blocked 8 x 8 quadrants, ordered NW, NE, SW, SE (bits 0-3). */
// unsigned char getMapTileCollisionMask(unsigned char tileId);
// unsigned char getMapTileSwordCollisionMask(unsigned char tileId);

// bool isMapSolidAt();

// void updateMapOverworld();
// void drawMapOverworld();