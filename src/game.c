
#include "game.h"

GameData gameData;

typedef enum {
    MAP_CHANGE_EVENT_NONE,
    MAP_CHANGE_EVENT_ENTERING,
    MAP_CHANGE_EVENT_EXITING
} MapChangeEvent;

typedef struct {
    Link link;
    MapChangeEvent mapChangeEvent;
} Game;

static Game game;

void resetGameData(void)
{
    gameData = (GameData){
        .name = "LINK"
    };
}

void initGame()
{
    resetGameData();

    game = (Game){
        .link = initLink(loadMap(MAP_LOCATION_OVERWORLD_SPAWN)),
        .mapChangeEvent = MAP_CHANGE_EVENT_NONE
    };
}

void updateGame()
{
    updateMap();
    updateLink(&game.link);

    game.link.pos.x = fmaxf(0, fminf(game.link.pos.x, GAME_WIDTH - game.link.width));
    game.link.pos.y = fmaxf(MAP_TOP, fminf(game.link.pos.y, GAME_HEIGHT - game.link.height));

    // Capture a change map event (entering cave from overworld)
    Rectangle feet = getLinkFeetAt(&game.link, game.link.pos);
    MapLocation destination = getMapChangeEvent(feet, game.link.vel);
    if (destination != MAP_LOCATION_NONE) {
        game.link.pos = loadMap(destination);
    }
}

void drawGame()
{
    drawMap();
    drawLink(&game.link);
}
