
#include "cave_x07_y07.h"

/* Cave atlas IDs: black floor, rock wall, lower rock wall, and fire. */
static const unsigned char tileCollisionMask[4] = {
    TILE_OPEN, TILE_SOLID, TILE_SOLID, TILE_SOLID
};

unsigned char getMapCavex07y07CollisionMask(unsigned char tileId)
{
    if (tileId >= sizeof tileCollisionMask / sizeof tileCollisionMask[0]) {
        return TILE_SOLID;
    }
    return tileCollisionMask[tileId];
}

bool initMapCavex07y07()
{
    return false;
}

bool isMapCavex07y07SolidAt()
{
    return false;
}

void updateMapCavex07y07()
{
    return;
}

void drawMapCavex07y07()
{
    return;
}
