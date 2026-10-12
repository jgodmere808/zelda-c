
#include "overworld.h"
#include "loader.h"
#include "../texture_map.h"

#define MAP_OVERWORLD_ENTERING_TIME 1.0f
#define MAP_OVERWORLD_EXITING_TIME  1.0f

#define MAP_OVERWORLD_ROWS  8
#define MAP_OVERWORLD_COLS 16

#define MAP_OVERWORLD_SCREEN_PATH "map/overworld/row_%02d/screen_x%02d_y%02d.txt"

/*
 * Terrain collision metadata, indexed by the same ID as the tile atlas.
 * Each bit covers one 8 x 8 quadrant of a 16 x 16 tile. These are base 
 * terrain rules; individual screens can later override them for 
 * entrances and changing terrain.
 *
 *   1 2
 *   4 8
 */

static const unsigned char tileCollisionMask[MAP_ATLAS_TILE_COUNT] = {
    /* 00-11: cliff edges and open ground in the three palette variants. */
    TILE_BLOCK_TOP, TILE_SOLID, TILE_OPEN,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID, TILE_BLOCK_LEFT,
    TILE_BLOCK_TOP, TILE_SOLID, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID, TILE_BLOCK_LEFT,
    TILE_BLOCK_TOP, TILE_SOLID, TILE_OPEN,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID, TILE_BLOCK_LEFT,

    /* 12, 18, 1E are passages; only the south half of cave tile 16 is open. */
    TILE_OPEN, TILE_SOLID, TILE_SOLID, TILE_BLOCK_RIGHT, TILE_BLOCK_TOP, TILE_BLOCK_LEFT,
    TILE_OPEN, TILE_SOLID, TILE_SOLID, TILE_BLOCK_RIGHT, TILE_SOLID, TILE_BLOCK_LEFT,
    TILE_OPEN, TILE_SOLID, TILE_SOLID, TILE_BLOCK_RIGHT, TILE_SOLID, TILE_BLOCK_LEFT,

    /* 24-35: tree and rock walls with open outer upper corners. */
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE,
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE,
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE,
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE,
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE,
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE,

    /* 36-47: wall bases; tree plus water tiles block both sides. */
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SW,
    TILE_SOLID, TILE_OPEN, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SW,
    TILE_SOLID, TILE_SOLID, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SW,
    TILE_SOLID, TILE_OPEN, TILE_SOLID,

    /* 48-59: water and neighboring ground. */
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_BLOCK_NE | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_SW | TILE_BLOCK_SE, TILE_OPEN, TILE_OPEN, TILE_OPEN,

    /* 5A-6B: water and neighboring ground. */
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_SOLID, TILE_SOLID, TILE_SOLID, TILE_OPEN, TILE_OPEN, TILE_OPEN,

    /* 6C-7D: water and neighboring ground. */
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SW, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SW, TILE_OPEN, TILE_OPEN, TILE_OPEN,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SE, TILE_SOLID,
    TILE_BLOCK_NW | TILE_BLOCK_NE | TILE_BLOCK_SW, TILE_OPEN, TILE_OPEN, TILE_OPEN,

    /* 7E-8F: shoreline corners and land; 83, 89, 8F are wooden bridges. */
    TILE_BLOCK_NW, TILE_BLOCK_NE, TILE_BLOCK_SW, TILE_BLOCK_SE, TILE_SOLID, TILE_OPEN,
    TILE_BLOCK_NW, TILE_BLOCK_NE, TILE_BLOCK_SW, TILE_BLOCK_SE, TILE_SOLID, TILE_OPEN,
    TILE_BLOCK_NW, TILE_BLOCK_NE, TILE_BLOCK_SW, TILE_BLOCK_SE, TILE_SOLID, TILE_OPEN
};

unsigned char getMapOverworldTileCollisionMask(unsigned char tileId)
{
    if (tileId >= MAP_ATLAS_TILE_COUNT) return TILE_SOLID;
    return tileCollisionMask[tileId];
}

typedef struct {
    MapScreen screens[MAP_OVERWORLD_ROWS][MAP_OVERWORLD_COLS];
} MapOverworld;

static MapOverworld mapOverworld;

bool initMapOverworld()
{
    FILE *file;
    int row, col, length;
    char path[64];
    bool loaded;

    // load the map
    for (row = 0; row < MAP_OVERWORLD_ROWS; row++) {
        for (col = 0; col < MAP_OVERWORLD_COLS; col++) {
            length = snprintf(
                path, sizeof(path), MAP_OVERWORLD_SCREEN_PATH, row, col, row
            );

            if (length < 0 || (size_t)length >= sizeof(path)) {
                return false;
            }

            file = fopen(path, "r");
            if (file == NULL) {
                perror(path);
                return false;
            }

            loaded = loaderLoadScreen(
                file, &mapOverworld.screens[row][col], row, col
            );
            fclose(file);

            if (!loaded) {
                fprintf(stderr, "Invalid map screen: %s\n", path);
                return false;
            }
        }
    }

    return true;
}

MapLocationData loadMapOverworld(MapLocation location)
{
    Vector2 pos;
    int row, col;
    MapLocationData locationData;

    switch (location) {
        case MAP_LOCATION_OVERWORLD_SPAWN:
            row = 7;
            col = 7;
            pos = (Vector2){ 120, 112 };
            break;
        case MAP_LOCATION_OVERWORLD_CAVE_x07_y07:
            row = 7;
            col = 7;
            pos = (Vector2){ 64, 73 };
            break;
        default:
            fprintf(stderr, "Invalid Overworld MapLocation: %d", location);
            exit(EXIT_FAILURE);
    }

    locationData = (MapLocationData){
        .screen = &mapOverworld.screens[row][col],
        .pos = pos
    };

    return locationData;
}

void updateMapOverworld()
{
    return;
}

void updateMapOverworldEntering(float *progress)
{
    *progress += GetFrameTime() / MAP_OVERWORLD_ENTERING_TIME;
}

void updateMapOverworldExiting(float *progress)
{
    *progress += GetFrameTime() / MAP_OVERWORLD_EXITING_TIME;
}

void drawMapOverworld(MapScreen *screen)
{
    for (int row = 0; row < MAP_SCREEN_ROWS; row++) {
        for (int col = 0; col < MAP_SCREEN_COLS; col++) {
            unsigned char tileId = (*screen)[row][col];

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

void drawMapOverworldEntering(MapScreen *screen, float *progress)
{
    drawMapOverworld(screen);
}

void drawMapOverworldExiting(MapScreen *screen, float *progress)
{
    drawMapOverworld(screen);
}