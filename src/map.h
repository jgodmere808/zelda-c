#pragma once

#include "maps/config.h"

typedef struct {
    MapLocation origin;
    MapLocation destination;
} MapChangeEvent;

bool initMap();
Vector2 loadMap(MapLocation location);

bool mapIsBlocked(Rectangle location);
bool mapTryPickupItem(Rectangle linkFeet);

MapChangeEvent getMapChangeEvent(Rectangle feet, Vector2 velocity);

void updateMap();
void updateMapEntering(MapChangeEvent *event, float *progress);
void updateMapExiting(MapChangeEvent *event, float *progress);

void drawMap();
void drawMapEntering(MapChangeEvent *event, float *progress);
void drawMapExiting(MapChangeEvent *event, float *progress);
