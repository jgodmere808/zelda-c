
#include "loader.h"

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

bool loaderLoadScreen(
    FILE *file,
    MapScreen *mapScreen,
    int mapRow,
    int mapCol
)
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

            if (end - p != 2 || (*end != '\0' && !isspace((unsigned char)*end))) {
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

    /* Reject extra non-comment data after the 11 tile rows. */
    return !readContentLine(file, line, sizeof line) && !ferror(file);
}