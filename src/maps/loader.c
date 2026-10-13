
#include "loader.h"

/* Names follow the ItemType/tiles_16x16.png atlas order. */
static const char *const itemNames[] = {
    "arrow", "silver_arrow", "bomb", "book_of_magic", "boomerang",
    "magical_boomerang", "bow", "blue_candle", "red_candle", "clock",
    "compass", "fairy", "food", "heart", "heart_container", "key",
    "magical_key", "letter", "magical_rod", "magical_shield", "map",
    "life_potion", "second_potion", "power_bracelet", "raft", "recorder",
    "blue_ring", "red_ring", "rupee", "five_rupees", "stepladder",
    "sword", "white_sword", "magical_sword", "triforce",
    "fairy_frame_1", "heart_frame_1", "rupee_frame_1", "triforce_frame_1"
};

_Static_assert(sizeof itemNames / sizeof itemNames[0] == ITEM_TRIFORCE_FRAME_1 + 1,
               "Item names must match the item atlas");

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

static bool parseItemLine(char *line, Item *item)
{
    const char *separators = " \t\r\n";
    char *name = strtok(line, separators);
    char *xText = strtok(NULL, separators);
    char *yText = strtok(NULL, separators);
    if (!name || !xText || !yText || strtok(NULL, separators)) return false;

    int type = 0;
    while (type < (int)(sizeof itemNames / sizeof itemNames[0]) &&
           strcmp(name, itemNames[type]) != 0) {
        type++;
    }
    if (type == (int)(sizeof itemNames / sizeof itemNames[0])) return false;

    char *end;
    errno = 0;
    long x = strtol(xText, &end, 10);
    if (errno != 0 || *end != '\0') return false;
    errno = 0;
    long y = strtol(yText, &end, 10);
    if (errno != 0 || *end != '\0' ||
        x < 0 || x > GAME_WIDTH - 16 ||
        y < MAP_TOP || y > GAME_HEIGHT - 16) return false;

    *item = initItem((ItemType)type, true, (Vector2){ (float)x, (float)y });
    return true;
}

bool loaderLoadScreen(
    FILE *file,
    MapScreen *mapScreen,
    Item *items,
    int *itemCount,
    int mapRow,
    int mapCol
)
{
    char line[128];
    int fileCol, fileRow;
    char extra;

    if (!file || !mapScreen || !items || !itemCount) return false;
    *itemCount = 0;

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

            if (end - p != 2 || tileId >= MAP_ATLAS_TILE_COUNT ||
                (*end != '\0' && !isspace((unsigned char)*end))) {
                return false;
            }

            (*mapScreen)[screenRow][screenCol] = (unsigned char)tileId;
            p = end;
        }

        while (isspace((unsigned char)*p)) {
            p++;
        }

        if (*p != '\0') {
            return false;  /* More than 16 IDs on this row. */
        }
    }

    if (!readContentLine(file, line, sizeof line)) return !ferror(file);

    char *section = line;
    while (isspace((unsigned char)*section)) section++;
    char *end = section + strlen(section);
    while (end > section && isspace((unsigned char)end[-1])) end--;
    *end = '\0';
    if (strcmp(section, "[items]") != 0) return false;

    while (readContentLine(file, line, sizeof line)) {
        if (*itemCount == MAX_MAP_SCREEN_ITEMS ||
            !parseItemLine(line, &items[*itemCount])) return false;
        (*itemCount)++;
    }

    return !ferror(file);
}
