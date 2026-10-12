
#include "map.h"

#include "audio.h"
#include "texture_map.h"
#include "maps/overworld.h"

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

static unsigned char getMapTileCollisionMask(unsigned char tileId)
{
    switch (map.mapRegion) {
        case MAP_REGION_OVERWORLD:
            return getMapOverworldTileCollisionMask(tileId);
            break;
        case MAP_REGION_CAVE_x07_y07:
            // return getMapCavex07y07CollisionMask();
            break;
        default:
            return 0;
    }
}

bool mapIsBlocked(Rectangle location)
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
            unsigned char mask = getMapTileCollisionMask(tileId);

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
