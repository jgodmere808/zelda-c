
#include "map.h"

#define MAP_ROWS  8
#define MAP_COLS 16

#define MAP_SCREEN_ROWS 11
#define MAP_SCREEN_COLS 16

#define MAP_ATLAS_TILE_COUNT (18 * 8)

#define MAP_SCREEN_PATH "map/overworld/row_%02d/screen_x%02d_y%02d.txt"

// primarily for music selection
typedef enum {
    MAP_REGION_OVERWORLD,
} MapRegion;

typedef unsigned char MapScreen[MAP_SCREEN_ROWS][MAP_SCREEN_COLS];

typedef struct {
    MapScreen mapScreens[MAP_ROWS][MAP_COLS];
    int currentMapRow;
    int currentMapCol;
    MapScreen currentScreen;
    MapScreen nextScreen;
    MapRegion mapRegion;
} Map;

static Map map;

static bool readContentLine(FILE *file, char *line, size_t size)
{
    while (fgets(line, size, file) != NULL) {
        char *p = line;

        while (isspace((unsigned char)*p)) p++;
        
        if (*p != '\0' && *p != '#') {
            return true;
        }
    }

    return false;
}

static bool initMapScreen(FILE *file, int mapRow, int mapCol)
{
    char line[128];
    int fileCol, fileRow;
    char extra;

    if (!readContentLine(file, line, sizeof line) ||
        sscanf(line, "screen %d %d %c", &fileCol, &fileRow, &extra) != 2 ||
        fileCol != mapCol || fileRow != mapRow) {
        return false;
    }

    for (int screenRow = 0; screenRow < MAP_SCREEN_ROWS; screenRow++) {
        if (!readContentLine(file, line, sizeof line)) {
            return false;
        }

        char *p = line;

        for (int screenCol = 0; screenCol < MAP_SCREEN_COLS; screenCol++) {
            char *end;

            while (isspace((unsigned char)*p)) {
                p++;
            }

            if (!isxdigit((unsigned char)p[0]) ||
                !isxdigit((unsigned char)p[1])) {
                return false;
            }

            unsigned long tileId = strtoul(p, &end, 16);

            if (end - p != 2 ||
                tileId >= MAP_ATLAS_TILE_COUNT ||
                (*end != '\0' && !isspace((unsigned char)*end))) {
                return false;
            }

            map.mapScreens[mapRow][mapCol][screenRow][screenCol] =
                (unsigned char)tileId;
            p = end;
        }

        while (isspace((unsigned char)*p)) {
            p++;
        }

        if (*p != '\0') {
            return false;  /* More than 16 IDs on this row. */
        }
    }

    /* Reject extra non-comment data after the 11 tile rows. */
    return !readContentLine(file, line, sizeof line) && !ferror(file);
}

bool initMap()
{
    FILE *file;
    int row, col, length;
    char path[64];
    bool loaded;

    // load the map
    for (row = 0; row < MAP_ROWS; row++) {
        for (col = 0; col < MAP_COLS; col++) {
            length = snprintf(
                path, sizeof(path), MAP_SCREEN_PATH, row, col, row
            );

            if (length < 0 || (size_t)length >= sizeof(path)) {
                return false;
            }

            file = fopen(path, "r");
            if (file == NULL) {
                perror(path);
                return false;
            }

            loaded = initMapScreen(file, row, col);
            fclose(file);

            if (!loaded) {
                fprintf(stderr, "Invalid map screen: %s\n", path);
                return false;
            }
        }
    }

    map.currentMapRow = 0;
    map.currentMapCol = 0;
    map.mapRegion = 0;

    return true;
}

void loadMapScreen(SpawnLocation spawnLocation)
{
    int mapRow, mapCol, screenRow, screenCol;

    switch (spawnLocation) {
        case SPAWN_GAME_START:
            mapRow = 7;
            mapCol = 7;
            map.mapRegion = MAP_REGION_OVERWORLD;
            break;
        default:
            changeMusic(MUSIC_NONE);
            mapRow = 0;
            mapCol = 0;
            break;
    }

    map.currentMapRow = mapRow;
    map.currentMapCol = mapCol;

    for (screenRow = 0; screenRow < MAP_SCREEN_ROWS; screenRow++) {
        for (screenCol = 0; screenCol < MAP_SCREEN_COLS; screenCol++) {
            map.currentScreen[screenRow][screenCol] = 
                map.mapScreens[mapRow][mapCol][screenRow][screenCol];
        }
    }
}

void transitionMap(MapTransition mapTransition)
{
    switch (mapTransition) {
        case MAP_TRANSITION_LEFT:
            if (map.currentMapCol <= 0) return;
            break;
        case MAP_TRANSITION_RIGHT:
            if (map.currentMapCol >= MAP_COLS) return;
            break;
        case MAP_TRANSITION_UP:
            if (map.currentMapRow <= 0) return;
            break;
        case MAP_TRANSITION_DOWN:
            if (map.currentMapRow >= MAP_ROWS) return;
            break;
    }
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
    int tileHeight;
    unsigned char tileId;
    int screenRow, screenCol;
    int atlasRow, atlasCol, sourceX, sourceY;

    for (screenRow = 0; screenRow < MAP_SCREEN_ROWS; screenRow++) {
        for (screenCol = 0; screenCol < MAP_SCREEN_COLS; screenCol++) {
            tileId = map.currentScreen[screenRow][screenCol];

            atlasCol = tileId % 18;
            atlasRow = tileId / 18;
            sourceX = atlasCol * 16;
            sourceY = atlasRow * 16;

            // last row only shows half on screen (to match original NES game)
            tileHeight = (screenRow == MAP_SCREEN_ROWS - 1) ? 8 : 16;

            Rectangle source = {
                sourceX, sourceY, 16, tileHeight
            };

            Rectangle destination = {
                screenCol * 16 * FACTOR,
                MAP_SHIFT_DOWN + (screenRow * 16) * FACTOR,
                16 * FACTOR,
                tileHeight * FACTOR
            };

            DrawTexturePro(
                textures[TEXTURE_OVERWORLD_TILES],
                source,
                destination,
                (Vector2){ 0, 0 },
                0,
                WHITE
            );
        }
    }
}