#pragma once

#include "config.h"

unsigned char getMapOverworldTileCollisionMask(unsigned char tileId);

bool initMapOverworld();
MapLocationData loadMapOverworld(MapLocation location);

void updateMapOverworld();
void drawMapOverworld(MapScreen *screen);