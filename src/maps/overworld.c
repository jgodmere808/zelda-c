
#include "overworld.h"

#define MAP_OVERWORLD_ROWS  8
#define MAP_OVERWORLD_COLS 16

#define MAP_ATLAS_TILE_COUNT (18 * 8)

#define MAP_OVERWORLD_SCREEN_PATH "map/overworld/row_%02d/screen_x%02d_y%02d.txt"

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
    for (row = 0; row < MAP_ROWS; row++) {
        for (col = 0; col < MAP_COLS; col++) {
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

            loaded = loaderLoadScreen(file, row, col);
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
            pos = (Vector2){ 100, 100 };
            break;
        default:
            fprintf(stderr, "Invalid Overworld MapLocation: %d", location);
            exit(EXIT_FAILURE);
    }

    locationData = (MapLocationData){
        .screen = &mapOverworld.screens[row][col],
        .row = row,
        .col = col,
        .pos = pos,
        .location = location
    };

    return locationData;
}

// bool isMapSolidAt()
// {
//     return;
// }