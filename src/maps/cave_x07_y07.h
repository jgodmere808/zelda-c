#pragma once

#include "config.h"

unsigned char getMapCavex07y07CollisionMask(unsigned char tileId);

bool initMapCavex07y07(bool *collectedSword1);
MapLocationData loadMapCavex07y07(MapLocation location);

bool isMapCavex07y07SolidAt();

void updateMapCavex07y07();
void updateMapCavex07y07Entering(float *progress);
void updateMapCavex07y07Exiting(float *progress);

void drawMapCavex07y07(MapScreen *screen, bool isEnteringOrExiting);
void drawMapCavex07y07Entering(MapScreen *screen, float *progress);
void drawMapCavex07y07Exiting(MapScreen *screen, float *progress);