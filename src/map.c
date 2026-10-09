
#include "map.h"

#define MAP_ROWS  8
#define MAP_COLS 16

#define MAP_SCREEN_ROWS 11
#define MAP_SCREEN_COLS 16

#define MAP_ATLAS_TILE_COUNT (18 * 8)

#define MAP_SCREEN_PATH "map/overworld/row_%02d/screen_x%02d_y%02d.txt"

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
    TILE_OPEN = 0,
    TILE_BLOCK_NW = 1,
    TILE_BLOCK_NE = 2,
    TILE_BLOCK_SW = 4,
    TILE_BLOCK_SE = 8,
    TILE_BLOCK_LEFT = TILE_BLOCK_NW | TILE_BLOCK_SW,
    TILE_BLOCK_RIGHT = TILE_BLOCK_NE | TILE_BLOCK_SE,
    TILE_BLOCK_TOP = TILE_BLOCK_NW | TILE_BLOCK_NE,
    TILE_BLOCK_BOTTOM = TILE_BLOCK_SW | TILE_BLOCK_SE,
    TILE_SOLID = TILE_BLOCK_TOP | TILE_BLOCK_BOTTOM
};

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

// primarily for music selection
typedef enum {
    MAP_REGION_OVERWORLD,
} MapRegion;

typedef unsigned char MapScreen[MAP_SCREEN_ROWS][MAP_SCREEN_COLS];

typedef struct {
    MapScreen mapScreens[MAP_ROWS][MAP_COLS];

    MapScreen currentScreen;
    int currentMapRow;
    int currentMapCol;

    MapScreen nextScreen;
    int nextMapRow;
    int nextMapCol;

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

static bool mapScreenIsBlocked(
    MapScreen screen, Rectangle location, bool isSword
)
{
    const float mapBottom = MAP_TOP + MAP_VISIBLE_HEIGHT;
    const float left = fmaxf(location.x, 0);
    const float top = fmaxf(location.y, MAP_TOP);
    const float right = fminf(location.x + location.width, GAME_WIDTH);
    const float bottom = fminf(location.y + location.height, mapBottom);

    /* Screen edges are handled by map transitions, not terrain collision. */
    if (!(right > left && bottom > top)) return false;

    for (int row = (int)(top - MAP_TOP) / 16;
         row < MAP_SCREEN_ROWS && MAP_TOP + row * 16 < bottom;
         row++) {
        for (int col = (int)left / 16;
             col < MAP_SCREEN_COLS && col * 16 < right;
             col++) {
            unsigned char tileId = screen[row][col];
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

bool mapIsBlocked(Rectangle location, bool isSword)
{
    return mapScreenIsBlocked(map.currentScreen, location, isSword);
}

bool beginMapTransition(MapTransition mapTransition, Rectangle destinationFeet)
{
    int nextRow = map.currentMapRow;
    int nextCol = map.currentMapCol;
    const float mapBottom = MAP_TOP + MAP_VISIBLE_HEIGHT;

    switch (mapTransition) {
        case MAP_TRANSITION_LEFT:
            if (map.currentMapCol <= 0) return false;
            nextCol--;
            break;
        case MAP_TRANSITION_RIGHT:
            if (map.currentMapCol >= MAP_COLS - 1) return false;
            nextCol++;
            break;
        case MAP_TRANSITION_UP:
            if (map.currentMapRow <= 0) return false;
            nextRow--;
            break;
        case MAP_TRANSITION_DOWN:
            if (map.currentMapRow >= MAP_ROWS - 1) return false;
            nextRow++;
            break;
    }

    if (!(destinationFeet.width > 0 && destinationFeet.height > 0)) {
        return false;
    }

    if (mapTransition == MAP_TRANSITION_LEFT ||
        mapTransition == MAP_TRANSITION_RIGHT) {
        if (destinationFeet.y < MAP_TOP ||
            destinationFeet.y + destinationFeet.height > mapBottom) {
            return false;
        }
    } else {
        if (destinationFeet.x < 0 ||
            destinationFeet.x + destinationFeet.width > GAME_WIDTH) {
            return false;
        }

        if (mapTransition == MAP_TRANSITION_UP) {
            /* Link arrives below the visible map; inspect its bottom row. */
            destinationFeet.y = mapBottom - destinationFeet.height;
        }
    }

    if (mapScreenIsBlocked(
        map.mapScreens[nextRow][nextCol], destinationFeet, false
    )) {
        return false;
    }

    map.nextMapRow = nextRow;
    map.nextMapCol = nextCol;
    memcpy(
        map.nextScreen,
        map.mapScreens[nextRow][nextCol],
        sizeof(map.nextScreen)
    );

    return true;
}

void finishMapTransition()
{
    memcpy(
        map.currentScreen,
        map.nextScreen,
        sizeof(map.currentScreen)
    );

    map.currentMapRow = map.nextMapRow;
    map.currentMapCol = map.nextMapCol;
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

static void drawMapScreen(
    unsigned char screen[MAP_SCREEN_ROWS][MAP_SCREEN_COLS],
    int offsetX,
    int offsetY
)
{
    for (int row = 0; row < MAP_SCREEN_ROWS; row++) {
        for (int col = 0; col < MAP_SCREEN_COLS; col++) {
            unsigned char tileId = screen[row][col];

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
                col * 16 * FACTOR + offsetX,
                MAP_SHIFT_DOWN + row * 16 * FACTOR + offsetY,
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

void drawMap()
{
    drawMapScreen(map.currentScreen, 0, 0);
}

void drawMapTransition(float progress, MapTransition mapTransition)
{
    int oldX = 0;
    int oldY = 0;
    int nextX = 0;
    int nextY = 0;

    int width = GAME_WIDTH * FACTOR;
    int height = MAP_VISIBLE_HEIGHT * FACTOR;

    switch (mapTransition) {
        case MAP_TRANSITION_RIGHT: {
            int distance = (int)(width * progress);
            oldX = -distance;
            nextX = width - distance;
            break;
        }
        case MAP_TRANSITION_LEFT: {
            int distance = (int)(width * progress);
            oldX = distance;
            nextX = distance - width;
            break;
        }
        case MAP_TRANSITION_DOWN: {
            int distance = (int)(height * progress);
            oldY = -distance;
            nextY = height - distance;
            break;
        }
        case MAP_TRANSITION_UP: {
            int distance = (int)(height * progress);
            oldY = distance;
            nextY = distance - height;
            break;
        }
    }

    BeginScissorMode(
        0,
        MAP_SHIFT_DOWN,
        SCREEN_WIDTH,
        MAP_VISIBLE_HEIGHT * FACTOR
    );

    drawMapScreen(map.currentScreen, oldX, oldY);
    drawMapScreen(map.nextScreen, nextX, nextY);

    EndScissorMode();
}
