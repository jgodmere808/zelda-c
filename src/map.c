
#include "map.h"

#include "audio.h"
#include "texture_map.h"

#include "maps/overworld.h"
#include "maps/cave_x07_y07.h"

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
    return (
        initMapOverworld() &&
        initMapCavex07y07()
    );
}

Vector2 loadMap(MapLocation location)
{
    MapLocationData data;

    switch (location) {
        case MAP_LOCATION_OVERWORLD_SPAWN:
        case MAP_LOCATION_OVERWORLD_CAVE_x07_y07:
            data = loadMapOverworld(location);
            map.mapRegion = MAP_REGION_OVERWORLD;
            break;
        case MAP_LOCATION_CAVE_x07_y07:
            data = loadMapCavex07y07(location);
            map.mapRegion = MAP_REGION_CAVE_x07_y07;
            break;
        default:
            fprintf(stderr, "Invalid MapLocation: %d\n", location);
            exit(EXIT_FAILURE);
    }

    map.currentScreen = data.screen;
    return data.pos;
}

static unsigned char getMapTileCollisionMask(unsigned char tileId)
{
    switch (map.mapRegion) {
        case MAP_REGION_OVERWORLD:
            return getMapOverworldTileCollisionMask(tileId);
            break;
        case MAP_REGION_CAVE_x07_y07:
            return getMapCavex07y07CollisionMask(tileId);
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

MapChangeEvent getMapChangeEvent(Rectangle feet, Vector2 velocity)
{
    MapChangeEvent event;

    if (map.mapRegion == MAP_REGION_OVERWORLD) {
        MapScreen *entranceScreen =
            loadMapOverworld(MAP_LOCATION_OVERWORLD_CAVE_x07_y07).screen;

        /*
         * Entrance tile 0x16 is at column 4, row 1:
         * x = 64..80; its walkable lower half is y = 80..88.
         */
        if (
            map.currentScreen == entranceScreen &&
            velocity.y < 0 &&
            feet.x >= 64 && feet.x + feet.width <= 80 &&
            feet.y >= 80 && feet.y + feet.height <= 88
        ) {
            event.origin = MAP_LOCATION_OVERWORLD_CAVE_x07_y07;
            event.destination = MAP_LOCATION_CAVE_x07_y07;
            return event;
        }
    }

    if (map.mapRegion == MAP_REGION_CAVE_x07_y07) {
        /*
         * The cave's bottom doorway occupies columns 7 and 8.
         * The final tile row begins at y = 216.
         */
        if (
            velocity.y > 0 &&
            feet.x >= 112 && feet.x + feet.width <= 144 &&
            feet.y >= 216 && feet.y + feet.height <= GAME_HEIGHT
        ) {
            event.origin = MAP_LOCATION_CAVE_x07_y07;
            event.destination = MAP_LOCATION_OVERWORLD_CAVE_x07_y07;
            return event;
        }
    }

    event.origin = MAP_LOCATION_NONE;
    event.destination = MAP_LOCATION_NONE;

    return event;
}

void updateMap()
{
    switch (map.mapRegion) {
        case MAP_REGION_OVERWORLD:
            changeMusic(MUSIC_OVERWORLD);
            updateMapOverworld();
            break;
        case MAP_REGION_CAVE_x07_y07:
            changeMusic(MUSIC_NONE);
            updateMapCavex07y07();
            break;
        default:
            break;
    }
}

void updateMapEntering(MapChangeEvent *event, float *progress)
{
    switch (event->origin) {
        case MAP_LOCATION_OVERWORLD_CAVE_x07_y07:
            updateMapOverworldEntering(progress);
            break;
        case MAP_LOCATION_CAVE_x07_y07:
            updateMapCavex07y07Entering(progress);
            break;
        default:
            break;
    }
}

void updateMapExiting(MapChangeEvent *event, float *progress)
{
    switch (event->destination) {
        case MAP_LOCATION_OVERWORLD_CAVE_x07_y07:
            updateMapOverworldExiting(progress);
            break;
        case MAP_LOCATION_CAVE_x07_y07:
            updateMapCavex07y07Exiting(progress);
            break;
        default:
            break;
    }
}

void drawMap()
{
    switch (map.mapRegion) {
        case MAP_REGION_OVERWORLD:
            drawMapOverworld(map.currentScreen);
            break;
        case MAP_REGION_CAVE_x07_y07:
            drawMapCavex07y07(map.currentScreen);
            break;
        default:
            return;
    }
}

void drawMapEntering(MapChangeEvent *event, float *progress)
{
    switch (event->origin) {
        case MAP_LOCATION_OVERWORLD_CAVE_x07_y07:
            drawMapOverworldEntering(map.currentScreen, progress);
            break;
        case MAP_LOCATION_CAVE_x07_y07:
            drawMapCavex07y07Entering(map.currentScreen, progress);
            break;
        default:
            break;
    }
}

void drawMapExiting(MapChangeEvent *event, float *progress)
{
    switch (event->destination) {
        case MAP_LOCATION_OVERWORLD_CAVE_x07_y07:
            drawMapOverworldExiting(map.currentScreen, progress);
            break;
        case MAP_LOCATION_CAVE_x07_y07:
            drawMapCavex07y07Exiting(map.currentScreen, progress);
            break;
        default:
            break;
    }
}