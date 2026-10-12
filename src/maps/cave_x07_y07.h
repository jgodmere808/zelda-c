#pragma once

#include "config.h"

unsigned char getMapCavex07y07CollisionMask(unsigned char tileId);

bool initMapCavex07y07();

bool isMapCavex07y07SolidAt();

void updateMapCavex07y07();
void drawMapCavex07y07();