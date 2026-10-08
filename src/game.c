
#include "game.h"
#include "save.h"

typedef struct {
    Link link;
} Game;

GameState gameState;

static Game game;

void resetGameState(void)
{
    gameState = (GameState){
        .name = "LINK"
    };
}

void initGame()
{
    resetGameState();
    loadMapScreen(SPAWN_GAME_START);

    game.link = initLink((Vector2){ 120, 112 });
}

void updateGame()
{
    updateMap();
    updateLink(&game.link);
}

void drawGame()
{
    drawMap();
    drawLink(&game.link);
}
