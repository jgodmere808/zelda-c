
#include "game.h"

GameData gameData;

static Link link;

void resetGameData(void)
{
    gameData = (GameData){
        .name = "LINK"
    };
}

void initGame()
{
    resetGameData();
    link = initLink(loadMap(MAP_LOCATION_OVERWORLD_SPAWN));
}

void updateGame()
{
    updateMap();
    updateLink(&link);

    link.pos.x = fmaxf(0, fminf(link.pos.x, GAME_WIDTH - link.width));
    link.pos.y = fmaxf(MAP_TOP, fminf(link.pos.y, GAME_HEIGHT - link.height));

    // Capture a change map event (entering cave from overworld)
    Rectangle feet = getLinkFeetAt(&link, link.pos);
    MapLocation destination = getMapChangeEvent(feet, link.vel);
    if (destination != MAP_LOCATION_NONE) {
        link.pos = loadMap(destination);
    }
}

void drawGame()
{
    drawMap();
    drawLink(&link);
}
