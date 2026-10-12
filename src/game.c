
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
    link = initLink(loadMapScreen(MAP_LOCATION_OVERWORLD_CAVE_x07_y07));
}

void updateGame()
{
    updateMap();
    updateLink(&link);

    link.pos.x = fmaxf(0, fminf(link.pos.x, GAME_WIDTH - link.width));
    link.pos.y = fmaxf(MAP_TOP, fminf(link.pos.y, GAME_HEIGHT - link.height));
}

void drawGame()
{
    drawMap();
    drawLink(&link);
}
