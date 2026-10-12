
#include "map.h"

#include "audio.h"
#include "texture_map.h"
#include "maps/overworld.h"

/*
 * Terrain collision metadata, indexed by the same ID as the tile atlas.
 * Each bit covers one 8 x 8 quadrant of a 16 x 16 tile. Movement and sword
 * attacks can use different masks. These are base terrain rules; individual
 * screens can later override them for entrances and changing terrain.
 *
 *   1 2
 *   4 8
 */

enum {
    TILE_SMALL_TREE_BROWN = 0x13,
    TILE_SMALL_TREE_GREEN = 0x19,
    TILE_SMALL_TREE_GRAY = 0x1F
};

static const unsigned char tileCollisionMask[MAP_ATLAS_TILE_COUNT] = {
    /* 00-11: cliffs and open ground in the three palette variants. */
    TILE_BLOCK_LEFT, TILE_SOLID, TILE_OPEN, TILE_BLOCK_RIGHT, TILE_SOLID, TILE_SOLID,
    TILE_BLOCK_LEFT, TILE_SOLID, TILE_SOLID, TILE_BLOCK_RIGHT, TILE_SOLID, TILE_SOLID,
    TILE_BLOCK_LEFT, TILE_SOLID, TILE_OPEN, TILE_BLOCK_RIGHT, TILE_SOLID, TILE_SOLID,

    /* 12, 18, 1E are passages; only the south half of cave tile 16 is open. */
    TILE_OPEN, TILE_SOLID, TILE_SOLID, TILE_BLOCK_RIGHT, TILE_BLOCK_TOP, TILE_SOLID,
    TILE_OPEN, TILE_SOLID, TILE_SOLID, TILE_BLOCK_RIGHT, TILE_SOLID, TILE_SOLID,
    TILE_OPEN, TILE_SOLID, TILE_SOLID, TILE_BLOCK_RIGHT, TILE_SOLID, TILE_SOLID,

    /* 24-35: rock and tree walls, including cave entrances. */
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_SOLID,

    /* 36-47: wall bases and banks; 3A and 46 are open ground. */
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_BLOCK_LEFT, TILE_OPEN, TILE_BLOCK_RIGHT,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_BLOCK_LEFT, TILE_SOLID, TILE_BLOCK_RIGHT,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_BLOCK_LEFT, TILE_OPEN, TILE_BLOCK_RIGHT,

    /* 48-59: water and neighboring ground. */
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,

    /* 5A-6B: water and neighboring ground. */
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,

    /* 6C-7D: water and neighboring ground. */
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,

    /* 7E-8F: shoreline corners and land; 83, 89, 8F are wooden bridges. */
    TILE_BLOCK_LEFT, TILE_OPEN, TILE_BLOCK_RIGHT, TILE_OPEN, TILE_SOLID, TILE_OPEN,
    TILE_BLOCK_LEFT, TILE_OPEN, TILE_BLOCK_RIGHT, TILE_OPEN, TILE_SOLID, TILE_OPEN,
    TILE_BLOCK_LEFT, TILE_OPEN, TILE_BLOCK_RIGHT, TILE_OPEN, TILE_SOLID, TILE_OPEN
};

/* Bits cleared from movement collision when checking a sword attack. */
static const unsigned char tileSwordPassThroughMask[MAP_ATLAS_TILE_COUNT] = {
    /* Small trees block Link but do not stop his sword. */
    [TILE_SMALL_TREE_BROWN] = TILE_SOLID,
    [TILE_SMALL_TREE_GREEN] = TILE_SOLID,
    [TILE_SMALL_TREE_GRAY] = TILE_SOLID,

    /* Water between the trees. */
    [0x40] = TILE_SOLID,

    /* The water tiles and their banks in each palette variant. */
    [0x48] = TILE_SOLID, [0x49] = TILE_SOLID, [0x4A] = TILE_SOLID,
    [0x4E] = TILE_SOLID, [0x4F] = TILE_SOLID, [0x50] = TILE_SOLID,
    [0x54] = TILE_SOLID, [0x55] = TILE_SOLID, [0x56] = TILE_SOLID,
    [0x5A] = TILE_SOLID, [0x5B] = TILE_SOLID, [0x5C] = TILE_SOLID,
    [0x60] = TILE_SOLID, [0x61] = TILE_SOLID, [0x62] = TILE_SOLID,
    [0x66] = TILE_SOLID, [0x67] = TILE_SOLID, [0x68] = TILE_SOLID,
    [0x6C] = TILE_SOLID, [0x6D] = TILE_SOLID, [0x6E] = TILE_SOLID,
    [0x72] = TILE_SOLID, [0x73] = TILE_SOLID, [0x74] = TILE_SOLID,
    [0x78] = TILE_SOLID, [0x79] = TILE_SOLID, [0x7A] = TILE_SOLID,

    /* At a shoreline, only the water side is transparent to the sword. */
    [0x7E] = TILE_BLOCK_LEFT,  [0x80] = TILE_BLOCK_RIGHT,
    [0x82] = TILE_BLOCK_LEFT,  [0x83] = TILE_BLOCK_LEFT,
    [0x84] = TILE_BLOCK_LEFT,
    [0x86] = TILE_BLOCK_RIGHT, [0x8A] = TILE_BLOCK_LEFT,
    [0x8C] = TILE_BLOCK_RIGHT
};

unsigned char getMapTileCollisionMask(unsigned char tileId)
{
    if (tileId >= MAP_ATLAS_TILE_COUNT) return TILE_SOLID;
    return tileCollisionMask[tileId];
}

unsigned char getMapTileSwordCollisionMask(unsigned char tileId)
{
    if (tileId >= MAP_ATLAS_TILE_COUNT) return TILE_SOLID;
    return tileCollisionMask[tileId] & ~tileSwordPassThroughMask[tileId];
}

// Distinguish different maps
typedef enum {
    MAP_REGION_OVERWORLD,
    MAP_REGION_CAVE_x07_y07
} MapRegion;

typedef struct {
    MapScreen *currentScreen;
    MapRegion mapRegion;
} Map;

static Map map;

bool initMap()
{
    return initMapOverworld();
}

Vector2 loadMapScreen(MapLocation location)
{
    MapLocationData data = loadMapOverworld(location);
    map.currentScreen = data.screen;
    map.mapRegion = MAP_REGION_OVERWORLD;
    return data.pos;
}

bool mapIsBlocked(Rectangle location, bool isSword)
{
    const float mapBottom = MAP_TOP + MAP_VISIBLE_HEIGHT;
    const float left = fmaxf(location.x, 0);
    const float top = fmaxf(location.y, MAP_TOP);
    const float right = fminf(location.x + location.width, GAME_WIDTH);
    const float bottom = fminf(location.y + location.height, mapBottom);

    /* Only visible map tiles contribute terrain collision. */
    if (!(right > left && bottom > top)) return false;

    for (int row = (int)(top - MAP_TOP) / 16;
         row < MAP_SCREEN_ROWS && MAP_TOP + row * 16 < bottom;
         row++) {
        for (int col = (int)left / 16;
             col < MAP_SCREEN_COLS && col * 16 < right;
             col++) {
            unsigned char tileId = (*map.currentScreen)[row][col];
            unsigned char mask = isSword
                ? getMapTileSwordCollisionMask(tileId)
                : getMapTileCollisionMask(tileId);

            for (int quadrant = 0; quadrant < 4; quadrant++) {
                if (!(mask & (1 << quadrant))) continue;

                float cellX = col * 16 + (quadrant % 2) * 8;
                float cellY = MAP_TOP + row * 16 + (quadrant / 2) * 8;

                if (left < cellX + 8 && right > cellX &&
                    top < cellY + 8 && bottom > cellY) {
                    return true;
                }
            }
        }
    }

    return false;
}

void updateMap()
{
    switch (map.mapRegion) {
        case MAP_REGION_OVERWORLD:
            changeMusic(MUSIC_OVERWORLD);
            break;
        default:
            break;
    }
}

void drawMap()
{
    for (int row = 0; row < MAP_SCREEN_ROWS; row++) {
        for (int col = 0; col < MAP_SCREEN_COLS; col++) {
            unsigned char tileId = (*map.currentScreen)[row][col];

            int atlasCol = tileId % 18;
            int atlasRow = tileId / 18;
            int tileHeight = row == MAP_SCREEN_ROWS - 1 ? 8 : 16;

            Rectangle source = {
                atlasCol * 16,
                atlasRow * 16,
                16,
                tileHeight
            };

            Rectangle destination = {
                col * 16 * FACTOR,
                (MAP_TOP + row * 16) * FACTOR,
                16 * FACTOR,
                tileHeight * FACTOR
            };

            DrawTexturePro(
                textures[TEXTURE_OVERWORLD_TILES],
                source,
                destination,
                (Vector2){0, 0},
                0,
                WHITE
            );
        }
    }
}
