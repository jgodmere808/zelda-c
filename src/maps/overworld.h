#pragma once

#include "config.h"

unsigned char getMapOverworldTileCollisionMask(unsigned char tileId);

bool initMapOverworld();
MapLocationData loadMapOverworld(MapLocation location);

void updateMapOverworld();
void updateMapOverworldEntering(float *progress);
void updateMapOverworldExiting(float *progress);

void drawMapOverworld(MapScreen *screen);
void drawMapOverworldEntering(MapScreen *screen, float *progress);
void drawMapOverworldExiting(MapScreen *screen, float *progress);