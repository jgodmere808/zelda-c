
#include "cave_x07_y07.h"

#include "loader.h"
#include "../texture_map.h"

#define CAVE_TILE_COUNT 4

#define CAVE_SCREEN_PATH "map/cave_x07_y07/screen_x00_y00.txt"

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

typedef struct {
    MapScreen screen;
} MapCavex07y07;

static MapCavex07y07 mapCavex07y07;

bool initMapCavex07y07()
{
    FILE *file = fopen(CAVE_SCREEN_PATH, "r");
    if (file == NULL) {
        perror(CAVE_SCREEN_PATH);
        return false;
    }

    bool loaded = loaderLoadScreen(file, &mapCavex07y07.screen, 0, 0);
    fclose(file);

    if (!loaded) {
        fprintf(stderr, "Invalid cave screen: %s\n", CAVE_SCREEN_PATH);
        return false;
    }

    /* The shared loader permits overworld IDs; this atlas has only four. */
    for (int row = 0; row < MAP_SCREEN_ROWS; row++) {
        for (int col = 0; col < MAP_SCREEN_COLS; col++) {
            if (mapCavex07y07.screen[row][col] >= CAVE_TILE_COUNT) {
                fprintf(stderr, "Invalid cave tile at row %d, col %d\n",
                        row, col);
                return false;
            }
        }
    }

    return true;
}

MapLocationData loadMapCavex07y07()
{
    return (MapLocationData) {
        .screen = &mapCavex07y07.screen,
        .pos = { 120, 198 }
    };
}

void updateMapCavex07y07()
{
    return;
}

void drawMapCavex07y07(MapScreen *screen)
{
    for (int row = 0; row < MAP_SCREEN_ROWS; row++) {
        for (int col = 0; col < MAP_SCREEN_COLS; col++) {
            unsigned char tileId = (*screen)[row][col];
            int tileHeight = row == MAP_SCREEN_ROWS - 1 ? 8 : 16;

            Rectangle source = {
                tileId * 16, 0, 16, tileHeight
            };
            Rectangle destination = {
                col * 16 * FACTOR,
                (MAP_TOP + row * 16) * FACTOR,
                16 * FACTOR,
                tileHeight * FACTOR
            };

            DrawTexturePro(
                textures[TEXTURE_CAVE_TILES],
                source,
                destination,
                (Vector2){0, 0},
                0,
                WHITE
            );
        }
    }
}
